/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012-2014 Pete Curtis (OpenOrienteering)
 * Copyright 2012-2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_FILE_FORMAT_XML_H
#define LIBREMAPPER_FILE_FORMAT_XML_H

#include <memory>

#include <QString>

#include "fileformats/file_format.h"

namespace OpenOrienteering {

class Exporter;
class Importer;
class Map;
class MapView;


/** @brief Interface for dealing with XML files of maps.
 */
class XMLFileFormat : public FileFormat
{
public:
	/** @brief Creates a new file format of type XML.
	 */
	XMLFileFormat();
	
	
	/** @brief Returns true for an XML file using the Mapper namespace.
	 */
	ImportSupportAssumption understands(const char* buffer, int size) const override;
	
	
	/** @brief Creates an importer for XML files.
	 */
	std::unique_ptr<Importer> makeImporter(const QString& path, Map* map, MapView* view) const override;
	
	/** @brief Creates an exporter for XML files.
	 */
	std::unique_ptr<Exporter> makeExporter(const QString& path, const Map* map, const MapView* view) const override;
	
	
	/** @brief The minimum XML file format version supported by this implementation.
	 */
	static const int minimum_version;
	
	/** @brief The optimal XML file format version created by this implementation.
	 */
	static const int current_version;
	
	/** @brief The actual XML file format version to be written.
	 * 
	 * This value must be less than or equal to current_version.
	 */
	static int active_version;
	
};


}  // namespace OpenOrienteering


/**
  
\page file_format OpenOrienteering Mapper XML File Format Documentation

\date 2018-10-01
\author Kai Pastor

\todo Review and update.


\section changes Changes

\subsection version-10 (Planned for) Version 10 (Mapper 1.0)

- For writing, drop compatibility with Mapper versions before 0.9.
- Use the streaming variant when writing `barrier` elements.
- Stop writing text object box sizes to the coordinates stream.


\subsection version-9  Version 9

- 2019-10-01 Added reading of a streaming variant of the `barrier` element.
             After `<barrier ... action="skip"/>`, skip the element immediately
             following this barrier.
- 2019-10-01 Added reading and writing of an explicit `size` element for text
             object box size, in native (0.001 mm) coordinates.
- 2019-10-02 Added a text symbol `rotatable` property. This must be exported as
             `true` now when the text symbol is rotatable, but default to `true`
             when reading previous versions of the format.


\subsection version-8 Version 8

- 2018-10-08 Added `screen_angle` and `screen_frequency` attributes to element
             `color/spotcolors/namedcolor` element.
- 2018-09-16 Added `start_offset` and `end_offset` attributes to the `line_symbol`
             element, replacing the now deprecated `pointed_cap_length` attribute.
- 2018-08-25 Added optional `icon` element to symbol `element`.
- 2018-03-10 Added `mid_symbol_placement` attribute to `line_symbol` element.


\subsection version-7 Version 7

- 2018-03-10 First file format changelog entry

*/

#endif // LIBREMAPPER_FILE_FORMAT_XML_H
