/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_GDAL_IMAGE_READER_H
#define LIBREMAPPER_GDAL_IMAGE_READER_H

#include <functional>

#include <QByteArray>
#include <QCoreApplication>
#include <QImage>
#include <QImageReader>
#include <QRgb>
#include <QSize>
#include <QString>
#include <QVarLengthArray>
#include <QVector>

#include <gdal.h>

#include "templates/template_image.h"

namespace LibreMapper {


/**
 * A GDAL image reader modeled like QImageReader.
 */
class GdalImageReader
{
public:
	GdalImageReader() = delete;
	GdalImageReader(const GdalImageReader&) = delete;
	GdalImageReader(GdalImageReader&&) = delete;
	GdalImageReader& operator=(const GdalImageReader&) = delete;
	GdalImageReader& operator=(GdalImageReader&&) = delete;
	
	// QImageReader related API
	
	explicit GdalImageReader(const QString& path);
	
	~GdalImageReader();
	
	bool canRead() const;
	
	QImageReader::ImageReaderError error() const;
	
	QString errorString() const;
	
	QString filename() const;
	
	QByteArray format() const;
	
	QImage read();
	
	bool read(QImage* image);
	
	
	// GDAL related API
	
	QByteArray rasterBandsAsText() const;
	
	int findRasterBand(GDALColorInterp color_interpretation, GDALDataType data_type) const;
	
	GDALColorInterp getColorInterpretationWithType(int index, GDALDataType data_type) const;
	
	struct RasterInfo
	{
		QVarLengthArray<int, 4> bands;
		QSize size;
		QImage::Format image_format = QImage::Format_Invalid;
		std::function<void(QImage&)> postprocessing = GdalImageReader::noop;
		int pixel_space = 1;   ///< The byte offset from one pixel to the next one.
		int band_space  = 1;   ///< The in-pixel byte offset from one band value to the next one.
		int band_offset = 0;   ///< The in-pixel byte offset of the first band value.
	};
	
	RasterInfo readRasterInfo() const;
	
	QVector<QRgb> readColorTable(int band) const;
	
	/**
	 * Returns the file's geotransform in a type suitable for TemplateImage.
	 * 
	 * For valid data, the result's type member is set to Georeferencing_GDAL.
	 */
	TemplateImage::GeoreferencingOption readGeoTransform();
	
	static QString toProjSpec(const QByteArray& gdal_spec);
	
	
	// Translation
	
	Q_DECLARE_TR_FUNCTIONS(LibreMapper::GdalImageReader)
	
	
protected:
	static void noop(QImage& /*image*/);
	
	static void premultiplyARGB32(QImage& image);
	
	static void premultiplyGray8(QImage& image);
	
	
private:
	QString path;
	QImageReader::ImageReaderError err = QImageReader::UnknownError;
	QString error_string;
	GDALDatasetH dataset = nullptr;
	int raster_count = 0;
	
};


/**
 * Returns the file's geotransform via GDAL in a type suitable for TemplateImage.
 * 
 * This is a convenience function which can beforward-declared to avoid the
 * dependency on GDAL header files which results from including this header.
 * 
 * \see GdalImageReader::readGeoTransform()
 */
TemplateImage::GeoreferencingOption readGdalGeoTransform(const QString& filepath);


}  // namespace LibreMapper

#endif // LIBREMAPPER_GDAL_IMAGE_READER_H
