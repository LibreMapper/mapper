/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Pete Curtis (OpenOrienteering)
 * Copyright 2013, 2016-2018 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_FILE_FORMAT_REGISTRY_H
#define LIBREMAPPER_FILE_FORMAT_REGISTRY_H

#include <functional>
#include <memory>
#include <vector>

#include <QString>

#include "fileformats/file_format.h"

namespace LibreMapper {


/** Provides a registry for file formats, and takes ownership of the supported format objects.
 */
class FileFormatRegistry
{
public:
	/** Creates an empty file format registry.
	 */
	FileFormatRegistry() noexcept;
	
	FileFormatRegistry(const FileFormatRegistry&) = delete;
	
	/** Destroys a file format registry.
	 */
	~FileFormatRegistry();
	
	FileFormatRegistry& operator=(const FileFormatRegistry&) = delete;
	
	/** Returns an immutable list of the available file formats.
	 */
	inline const std::vector<FileFormat *> &formats() const { return fmts; }
	
	
	/** Finds a file format which satisfies the given predicate, or returns 
	 *  nullptr if no format is found.
	 */
	const FileFormat* findFormat(std::function<bool(const FileFormat*)> predicate) const;
	
	
	/** Finds a file format with the given internal ID, or returns nullptr if no format
	 *  is found.
	 */
	const FileFormat *findFormat(const char* id) const;
	
	/** Finds a file format which implements the given filter, or returns nullptr if no 
	 * format is found.
	 * 
	 * Only the file format's filter string before the closing ')' is taken into
	 * account for matching, i.e. the given parameter 'filter' may contain
	 * additional extensions following the original ones.
	 * 
	 * The predicate is intended to select either import or export formats.
	 */
	const FileFormat *findFormatByFilter(const QString& filter, bool (FileFormat::*predicate)() const) const;
	
	/** Finds a file format whose file extension matches the file extension of the given
	 *  path, or returns nullptr if no matching format is found.
	 * 
	 * The predicate is intended to select either import or export formats.
	 */
	const FileFormat *findFormatForFilename(const QString& filename, bool (FileFormat::*predicate)() const) const;
	
	/** Finds an import file format by looking at the existing data.
	 */
	const FileFormat* findFormatForData(const QString& path, FileFormat::FileTypes types) const;
	
	
	/** Returns the ID of default file format for this registry. This will automatically
	 *  be set to the first registered format.
	 */
	const char* defaultFormat() const { return default_format_id; }
	
	/** Registers a new file format. The registry takes ownership of the provided Format.
	 */
	void registerFormat(FileFormat *format);
	
	/**
	 * Unregisters a file format.
	 * 
	 * Returns a non-const pointer to the file format and transfers ownership to the caller.
	 */
	std::unique_ptr<FileFormat> unregisterFormat(const FileFormat *format);
	
	
	/** Creates an importer for the given path, if possible.
	 */
	std::unique_ptr<Importer> makeImporter(const QString& path, Map& map, MapView* view = nullptr);
	
	/** Creates an exporter for the given path, if possible.
	 */
	std::unique_ptr<Exporter> makeExporter(const QString& path, const Map* map, const MapView* view);
	
	
private:
	std::vector<FileFormat *> fmts;
	const char* default_format_id;
};

/** A FileFormatRegistry that is globally defined for convenience. Within the scope of a single
 *  application, you can simply use calls of the form "FileFormats.findFormat(...)".
 */
extern FileFormatRegistry FileFormats;


}  // namespace LibreMapper

#endif // LIBREMAPPER_FILE_FORMAT_REGISTRY_H
