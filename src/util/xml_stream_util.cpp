/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2013-2024 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "xml_stream_util.h"

#include <algorithm>
#include <array>
#include <exception>
#include <limits>
#include <stdexcept>

#include <QtGlobal>
#include <QBuffer>
#include <QByteArray>
#include <QChar>
#include <QIODevice>
#include <QLatin1Char>
#include <QScopedValueRollback>
#include <QTextCodec>
// IWYU pragma: no_include <qxmlstream.h>

#include "core/map_coord.h"
#include "fileformats/file_format.h"
#include "fileformats/file_import_export.h"
#include "fileformats/xml_file_format.h"
#include "util/key_value_container.h"


namespace LibreMapper {

void writeLineBreak(QXmlStreamWriter& xml)
{
	if (!xml.autoFormatting())
	{
		static const QString linebreak = QLatin1String{"\n"};
		xml.writeCharacters(linebreak);
	}
}


QString numberToString(double value, int precision)
{
	auto number = QString::number(value, 'f', precision);
	auto i = number.length() - 1;
	if (number.contains(QLatin1Char('.')))
	{
		// Cut off trailing zeros
		while (i > 0 && number.at(i) == QLatin1Char('0'))
			--i;
		if (number.at(i) == QLatin1Char('.'))
			--i;
	}
	number.resize(++i);
	return number;
}



//### XmlRecoveryHelper ###

bool XmlRecoveryHelper::operator() ()
{
	auto offending_index_64 = xml.characterOffset();
	auto offending_message = xml.errorString();
	auto stream = xml.device();
	
	if (xml.hasError()
	    && offending_index_64 <= std::numeric_limits<int>::max()
	    && offending_index_64 > recovery_start
	    && stream
	    && stream->seek(0))
	{
		// Get the whole input
		auto raw_data = stream->readAll();
		
		// Determine the encoding
		QTextCodec* codec = nullptr;
		for (QXmlStreamReader probe(raw_data); !probe.atEnd(); probe.readNext())
		{
			if (probe.tokenType() == QXmlStreamReader::StartDocument)
			{
				codec = QTextCodec::codecForName(probe.documentEncoding().toLatin1());
				break;
			}
		}
		if (Q_UNLIKELY(!codec))
		{
			return false;
		}
		
		// Convert to QString, and release the raw data
		auto data = codec->toUnicode(raw_data);
		raw_data = {};
		
		// Loop until the current element is fixed
		for (auto index = int(offending_index_64 - 1); xml.hasError(); index = int(xml.characterOffset() - 1))
		{
			// Cf. https://www.w3.org/TR/2008/REC-xml-20081126/#NT-Char
			// and QXmlStreamReaderPrivate::scanUntil,
			// http://code.qt.io/cgit/qt/qtbase.git/tree/src/corelib/xml/qxmlstream.cpp?h=5.6.2#n961
			auto offending_char = static_cast<unsigned int>(data.at(index).unicode());
			if (xml.error() != QXmlStreamReader::NotWellFormedError
			    || offending_char == 0x9     // horizontal tab
			    || offending_char == 0xa  // line feed
			    || offending_char == 0xd  // carriage return
			    || (offending_char >= 0x20 && offending_char <= 0xfffd)
			    || offending_char > QChar::LastValidCodePoint)
			{
				// Another error than an invalid character
				break;
			}
			
			// Remove the offending character
			data.remove(index, 1);
			
			// Read again to the element start
			xml.clear();
			xml.addData(data);
			while (!xml.hasError() && xml.characterOffset() < recovery_start)
			{
				xml.readNext();
			}
			Q_ASSERT(!xml.hasError());
			
			// Read the current element completely
			xml.skipCurrentElement();
		}
		
		// Restore the XML stream state with the modified data.
		// We must use a QIODevice again, for later recovery attempts.
		auto buffer = new QBuffer(stream); // buffer will live as long as the original stream
		buffer->setData(codec->fromUnicode(data));
		buffer->open(QIODevice::ReadOnly);
		
		xml.clear();
		xml.setDevice(buffer);
		while (!xml.atEnd() && xml.characterOffset() < recovery_start)
		{
			xml.readNext();
		}
		
		if (xml.characterOffset() == recovery_start)
		{
			return true;
		}
		else if (!xml.hasError())
		{
			xml.raiseError(offending_message);
		}
	}
	return false;
}



//### XmlElementWriter ###

void XmlElementWriter::write(const MapCoordVector& coords)
{
	namespace literal = XmlStreamLiteral;
	
	writeAttribute(literal::count, coords.size());
	
	if (XMLFileFormat::active_version < 6 || xml.autoFormatting())
	{
		// XMAP files and old format: syntactically rich output
		for (auto& coord : coords)
			coord.save(xml);
	}
	else if (auto* device = xml.device())
	{
		// Default: efficient plain text format
		// Direct UTF-8 writing without unnecessary allocations, escaping or
		// conversions, but also without handling of device errors.
		xml.writeCharacters({});  // Finish the start element
		MapCoord::StringBuffer<char> buffer;
		for (auto& coord : coords)
			device->write(coord.toUtf8(buffer));
	}
	else
	{
		// Default: efficient plain text format
		MapCoord::StringBuffer<QChar> buffer;
		for (auto& coord : coords)
			xml.writeCharacters(coord.toString(buffer));
	}
}

void LibreMapper::XmlElementWriter::write(const KeyValueContainer& tags)
{
	namespace literal = XmlStreamLiteral;
	
	for (auto const& tag : tags)
	{
		XmlElementWriter tag_element(xml, literal::t);
		tag_element.writeAttribute(literal::k, tag.key);
		xml.writeCharacters(tag.value);
	}
}



//### XmlElementReader ###

void XmlElementReader::read(MapCoordVector& coords)
{
	namespace literal = XmlStreamLiteral;
	
	coords.clear();
	
	const auto num_coords = attribute<unsigned int>(literal::count);
	coords.reserve(std::min(num_coords, 500000u));
	
	try
	{
		for( xml.readNext(); xml.tokenType() != QXmlStreamReader::EndElement; xml.readNext() )
		{
			const QXmlStreamReader::TokenType token = xml.tokenType();
			if (xml.error() || token == QXmlStreamReader::EndDocument)
			{
				throw FileFormatException(::LibreMapper::ImportExport::tr("Could not parse the coordinates."));
			}
			else if (token == QXmlStreamReader::Characters && !xml.isWhitespace())
			{
				QStringView text = xml.text();
				try
				{
					/* \fixme Please mind that the text snippet (QStringView)
					 * is modified by the MapCoord constructor. The loop tests
					 * whether the constructor consumed all the charasters in 
					 * the view. I (Libor) see this as an example violation of
					 * the principle of least surprise.
					 */
					while (text.length())
					{
						coords.emplace_back(text);
					}
				}
				catch (std::exception& e)
				{
					Q_UNUSED(e)
					qDebug("Could not parse the coordinates: %s", e.what());
					throw FileFormatException(::LibreMapper::ImportExport::tr("Could not parse the coordinates."));
				}
			}
			else if (token == QXmlStreamReader::StartElement)
			{
				if (xml.name() == literal::coord)
				{
					coords.emplace_back(MapCoord::load(xml));
				}
				else
				{
					xml.skipCurrentElement();
				}
			}
			// otherwise: ignore element
		}
	}
	catch (std::range_error &e)
	{
		throw FileFormatException(::LibreMapper::MapCoord::tr(e.what()));
	}
	
	if (coords.size() != num_coords)
	{
		throw FileFormatException(::LibreMapper::ImportExport::tr("Expected %1 coordinates, found %2.").arg(num_coords).arg(coords.size()));
	}
}


void XmlElementReader::readForText(MapCoordVector& coords)
{
	namespace literal = XmlStreamLiteral;
	
	coords.clear();
	coords.reserve(2);
	
	const auto num_coords = attribute<unsigned int>(literal::count);
	
	QScopedValueRollback<MapCoord::BoundsOffset> offset{MapCoord::boundsOffset()};
	
	try
	{
		for( xml.readNext(); xml.tokenType() != QXmlStreamReader::EndElement; xml.readNext() )
		{
			const QXmlStreamReader::TokenType token = xml.tokenType();
			if (xml.error() || token == QXmlStreamReader::EndDocument)
			{
				throw FileFormatException(::LibreMapper::ImportExport::tr("Could not parse the coordinates."));
			}
			else if (token == QXmlStreamReader::Characters && !xml.isWhitespace())
			{
				QStringView text = xml.text();
				try
				{
					while (text.length())
					{
						if (coords.size() == 1)
						{
							// Don't apply an offset to text box size.
							offset.commit();
							MapCoord::boundsOffset().reset(false);
						}
						coords.emplace_back(text);
					}
				}
				catch (std::exception& e)
				{
					Q_UNUSED(e)
					qDebug("Could not parse the coordinates: %s", e.what());
					throw FileFormatException(::LibreMapper::ImportExport::tr("Could not parse the coordinates."));
				}
			}
			else if (token == QXmlStreamReader::StartElement)
			{
				if (xml.name() == literal::coord)
				{
					if (coords.size() == 1)
					{
						// Don't apply an offset to text box size.
						offset.commit();
						MapCoord::boundsOffset().reset(false);
					}
					coords.emplace_back(MapCoord::load(xml));
				}
				else
				{
					xml.skipCurrentElement();
				}
			}
			// otherwise: ignore element
		}
	}
	catch (std::range_error &e)
	{
		throw FileFormatException(::LibreMapper::MapCoord::tr(e.what()));
	}
	
	if (coords.size() != num_coords)
	{
		throw FileFormatException(::LibreMapper::ImportExport::tr("Expected %1 coordinates, found %2.").arg(num_coords).arg(coords.size()));
	}
}

void LibreMapper::XmlElementReader::read(KeyValueContainer& tags)
{
	namespace literal = XmlStreamLiteral;
	
	tags.clear();
	while (xml.readNextStartElement())
	{
		if (xml.name() == literal::t)
		{
			const QString key(xml.attributes().value(literal::k).toString());
			tags.insert_or_assign(key, xml.readElementText());
		}
		else
		{
			xml.skipCurrentElement();
		}
	}
}


}  // namespace LibreMapper
