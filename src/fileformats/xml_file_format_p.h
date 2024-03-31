/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Pete Curtis (OpenOrienteering)
 * Copyright 2012-2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_FILE_FORMAT_XML_P_H
#define LIBREMAPPER_FILE_FORMAT_XML_P_H

#include <functional>

#include <QCoreApplication>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "core/symbols/symbol.h"
#include "fileformats/file_import_export.h"

namespace OpenOrienteering {

/** Map exporter for the xml based map format. */
class XMLFileExporter : public Exporter
{
	Q_DECLARE_TR_FUNCTIONS(OpenOrienteering::XMLFileExporter)
	
public:
	XMLFileExporter(const QString& path, const Map* map, const MapView* view);
	XMLFileExporter() = delete;
	XMLFileExporter(const XMLFileExporter&) = delete;
	XMLFileExporter(XMLFileExporter&&) = delete;
	~XMLFileExporter() override;
	
	XMLFileExporter& operator=(const XMLFileExporter&) = delete;	
	XMLFileExporter& operator=(XMLFileExporter&&) = delete;	
	
protected:
	bool exportImplementation() override;
	
	void exportGeoreferencing();
	void exportColors();
	void exportSymbols();
	void exportMapParts();
	void exportTemplates();
	void exportView();
	void exportPrint();
	void exportUndo();
	void exportRedo();
	
private:
	QXmlStreamWriter xml;
};


/** Map importer for the xml based map format. */
class XMLFileImporter : public Importer
{
	Q_DECLARE_TR_FUNCTIONS(OpenOrienteering::XMLFileImporter)
	
public:
	XMLFileImporter(const QString& path, Map *map, MapView *view);
	XMLFileImporter() = delete;
	XMLFileImporter(const XMLFileImporter&) = delete;
	XMLFileImporter(XMLFileImporter&&) = delete;
	~XMLFileImporter() override;
	
	XMLFileImporter& operator=(const XMLFileImporter&) = delete;	
	XMLFileImporter& operator=(XMLFileImporter&&) = delete;	
	
protected:
	bool importImplementation() override;
	
	void importElements();
	
	void handleBarrier(const std::function<void()>& reader);
	
	void addWarningUnsupportedElement();
	void importMapNotes();
	void importGeoreferencing();
	void validateGeoreferencing();
	void importColors();
	void importSymbols();
	void importMapParts();
	void importTemplates();
	void importView();
	void importPrint();
	void importUndo();
	void importRedo();
	
private:
	QXmlStreamReader xml;
	SymbolDictionary symbol_dict;
	int version = -1;
	bool georef_offset_adjusted;
};


}  // namespace OpenOrienteering

#endif
