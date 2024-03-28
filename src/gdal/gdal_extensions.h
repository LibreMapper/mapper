/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2019-2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


namespace OpenOrienteering {

namespace gdal {

/**
 * Get a list of extensions to be used with QImageReader/TemplateImage.
 * 
 * The GDAL raster driver will be regarded as secondary for the listed
 * extension and get its ambiguous extensions prefixed with "raster."
 */
template <class T>
T qImageReaderExtensions() {
	return {
		"bmp",
		"gif",
		"jpeg",
		"jpg",
		"png",
	};
}

}  // namespace gdal

}  // namespace OpenOrienteering
