/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012 Thomas Schöps (OpenOrienteering)
 * Copyright 2015-2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "world_file.h"

#include <QChar>
#include <QFile>
#include <QFileDevice>
#include <QFileInfo>
#include <QIODevice>
#include <QLatin1Char>
#include <QLatin1String>
#include <QTextStream>


namespace LibreMapper {

WorldFile::WorldFile() noexcept
: parameters { 1, 0, 0, 1, 0.5, 0.5 }
{
	// nothing else
}

WorldFile::WorldFile(double xw, double xh, double yw, double yh, double dx, double dy) noexcept
: parameters { xw, xh, yw, yh, dx, dy }
{
	// nothing else
}

WorldFile::WorldFile(const QTransform& wld) noexcept
: parameters { wld.m11(), wld.m12(), wld.m21(), wld.m22(), wld.m31() + wld.m11()/2, wld.m32() + wld.m22()/2 }
{
	// nothing else
}


WorldFile::operator QTransform() const
{
	// The world file parameters refer to the center of the top-left pixel,
	// but for QTransform, we want the top-left corner of this pixel.
	auto offset_x = (parameters[0] + parameters[2]) / 2;
	auto offset_y = (parameters[1] + parameters[3]) / 2;
	return QTransform {
	  parameters[0], parameters[1], 0,
	  parameters[2], parameters[3], 0,
	  parameters[4] - offset_x, parameters[5] - offset_y, 1
	};
}


bool WorldFile::load(const QString& path)
{
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return false;
	
	QTextStream text_stream(&file);
	bool ok = false;
	for (auto& parameter : parameters)
	{
		parameter = text_stream.readLine().toDouble(&ok);
		if (!ok)
			return false;
	}
	return true;
}

bool WorldFile::save(const QString &path) const
{
	QFile file(path);
	if (file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream stream(&file);
		stream.setRealNumberPrecision(10);
		for (auto value : parameters)
			stream << value << Qt::endl;
		file.close();
	}
	return file.error() == QFileDevice::NoError;
}

bool WorldFile::tryToLoadForImage(const QString& image_path)
{
	int last_dot_index = image_path.lastIndexOf(QLatin1Char('.'));
	if (last_dot_index < 0)
		return false;
	QString path_without_ext = image_path.left(last_dot_index + 1);
	QString ext = image_path.right(image_path.size() - (last_dot_index + 1));
	if (ext.size() <= 2)
		return false;
	
	// Possibility 1: use first and last character from image filename extension and 'w'
	QString test_path = path_without_ext + ext[0] + ext[ext.size() - 1] + QLatin1Char('w');
	if (load(test_path))
		return true;
	
	// Possibility 2: append 'w' to original extension
	test_path = image_path + QLatin1Char('w');
	if (load(test_path))
		return true;
	
	// Possibility 3: replace original extension by 'wld'
	test_path = path_without_ext + QLatin1String("wld");
	if (load(test_path))
		return true; // NOLINT
	
	return false;
}

QString WorldFile::pathForImage(const QString& image_path)
{
	// Just use QFileInfo rather than rolling our own path processing.
	const QFileInfo info(image_path);
	const QString base = info.path() + QLatin1Char('/') + info.completeBaseName(); 
	const auto suffix = info.suffix();
	switch (suffix.length())
	{
	case 0:
		// If there is no suffix, append ".wld"
		return base + QLatin1String(".wld");
	case 3:
		// If there are three chars, remove middle char and append 'w'
		return base + QLatin1Char('.') + suffix[0] + suffix[2] + QLatin1Char('w');
	default:
		// Otherwise, just append 'w'
		return base + QLatin1Char('.') + suffix + QLatin1Char('w');
	}
}


}  // namespace LibreMapper
