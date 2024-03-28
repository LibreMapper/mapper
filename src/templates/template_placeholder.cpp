/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "template_placeholder.h"

#include <utility>

#include <QByteArray>
#include <QList>
#include <QRectF>
#include <QStringRef>
#include <QVariant>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <util/util.h>

// IWYU pragma: no_include <qxmlstream.h>

namespace OpenOrienteering {

namespace {

bool copyXml(QXmlStreamReader& xml, QXmlStreamWriter& writer)
{
	auto token = xml.tokenType();
	if (token != QXmlStreamReader::StartElement)
	{
		xml.skipCurrentElement();
		return false;
	}
	
	writer.writeStartElement(xml.name().toString());
	writer.writeAttributes(xml.attributes());
	for (auto depth = 1; depth > 0; )
	{
		if (xml.error() != QXmlStreamReader::NoError)
			return false;
		
		token = xml.readNext();
		switch (token)
		{
		case QXmlStreamReader::NoToken:
		case QXmlStreamReader::Invalid:
		case QXmlStreamReader::DTD:
		case QXmlStreamReader::StartDocument:
		case QXmlStreamReader::EndDocument:
			return false;
		case QXmlStreamReader::StartElement:
			writer.writeStartElement(xml.name().toString());
			writer.writeAttributes(xml.attributes());
			++depth;
			break;
		case QXmlStreamReader::EndElement:
			--depth;
			writer.writeEndElement();
			break;
		case QXmlStreamReader::Characters:
			writer.writeCharacters(xml.text().toString());
			break;
		case QXmlStreamReader::EntityReference:
			writer.writeEntityReference(xml.name().toString());
			break;
		case QXmlStreamReader::Comment:
		case QXmlStreamReader::ProcessingInstruction:
			// skip
			break;
		}
	}
	return true;
}

}


TemplatePlaceholder::TemplatePlaceholder(QByteArray type, const QString& path, not_null<Map*> map)
: Template(path, map)
, original_type(std::move(type))
{}

TemplatePlaceholder::TemplatePlaceholder(const TemplatePlaceholder& proto) = default;

TemplatePlaceholder::~TemplatePlaceholder() = default;

TemplatePlaceholder* TemplatePlaceholder::duplicate() const
{
	return new TemplatePlaceholder(*this);
}

const char* TemplatePlaceholder::getTemplateType() const
{
	return original_type;
}

std::unique_ptr<Template> TemplatePlaceholder::makeActualTemplate() const
{
	auto maybe_open = true;
	QString buffer;
	{
		QXmlStreamWriter writer(&buffer);
		saveTemplateConfiguration(writer, maybe_open);
	}
	QXmlStreamReader reader(buffer);
	reader.readNextStartElement();  // <template ...>
	
	auto actual_template = Template::loadTemplateConfiguration(reader, *map, maybe_open);
	if (actual_template)
	{
		auto const property_names = dynamicPropertyNames();
		for (auto const& name : property_names)
			actual_template->setProperty(name, property(name));
	}
	return actual_template;
}

bool TemplatePlaceholder::isRasterGraphics() const
{
	return true;
}

void TemplatePlaceholder::drawTemplate(QPainter* /*painter*/, const QRectF& /*clip_rect*/, double /*scale*/, bool /*on_screen*/, qreal /*opacity*/) const
{}


void TemplatePlaceholder::setTemplateAreaDirty()
{}

QRectF TemplatePlaceholder::getTemplateExtent() const
{
	return infiniteRectF();
}


void TemplatePlaceholder::saveTypeSpecificTemplateConfiguration(QXmlStreamWriter& xml) const
{
	if (config.isEmpty())
		return;
	
	QXmlStreamReader reader(config);
	reader.readNextStartElement();  // Enter root element
	while (reader.readNextStartElement())
		copyXml(reader, xml);
}

bool TemplatePlaceholder::loadTypeSpecificTemplateConfiguration(QXmlStreamReader& xml)
{
	QXmlStreamWriter writer(&config);
	if (config.isEmpty())
	{
		writer.writeStartDocument();
		// We need an enclosing root element.
		writer.writeStartElement(QString::fromLatin1("root"));
	}
	auto result = copyXml(xml, writer);
	//writer.writeEndDocument(); // No, we may need to append.
	return result;
}

bool TemplatePlaceholder::loadTemplateFileImpl()
{
	setErrorString(tr("Unknown file format"));
	return false;
}

void TemplatePlaceholder::unloadTemplateFileImpl()
{}


}  // namespace OpenOrienteering
