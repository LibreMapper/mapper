/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2018,2026 Libor Pecháček
 *
 * This file is part of LibreMapper.
 */

#include <memory>
#include <stddef.h>
#include <stdint.h>

#include <QBuffer>
#include <QByteArray>
#include <QGuiApplication>
#include <QIODevice>
#include <QString>

#include "fileformats/ocd_file_format.h"
#include "fileformats/ocd_file_import.h" // IWYU pragma: keep
#include "fileformats/ocd_types_v12.h"
#include "fileformats/file_format.h"
#include "core/map.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
	// don't bother with files shorter than header size
	// Mapper will crash safely on them with null pointer dereference
	if (Size < sizeof(Ocd::FormatV12::FileHeader))
		return 0;

	char arg0[] = "fuzz-fileformat";
	char* argv[] = { arg0, nullptr };
	int argc = sizeof(argv)/sizeof(argv[0]) - 1;
	QGuiApplication app(argc, argv);

	QByteArray data_array(reinterpret_cast<const char *>(Data), Size);
	QBuffer buffer(&data_array);
	auto map = std::make_unique<LibreMapper::Map>();

	buffer.open(QIODevice::ReadWrite);
	LibreMapper::OcdFileFormat format(LibreMapper::OcdFileFormat::autoDeterminedVersion());

	try {
		auto support = format.understands(reinterpret_cast<const char*>(Data), Size);
		if (support == LibreMapper::FileFormat::NotSupported)
			return 0;

		auto importer = format.makeImporter({}, map.get(), nullptr);

		if (importer)
		{
			importer->setDevice(&buffer);
			importer->doImport();
		}
	}
	catch (LibreMapper::FileFormatException &e)
	{
		// FileFormatExceptions are expected, ignore them
	}

	return 0;  // Non-zero return values are reserved for future use.
}
