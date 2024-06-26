/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012 Thomas Schöps (OpenOrienteering)
 * Copyright 2015-2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_WORLD_FILE_H
#define LIBREMAPPER_WORLD_FILE_H

#include <QString>
#include <QTransform>


namespace LibreMapper {

/**
 * Handles pixel-to-world transformations given by world files.
 * 
 * \see https://en.wikipedia.org/wiki/World_file
 */
struct WorldFile
{
	/// The six world file parameters, order as in the text file.
	double parameters[6];
	
	/// Creates an default world file.
	WorldFile() noexcept;
	
	/// Creates a world file with the given parameters (in-order).
	WorldFile(double xw, double xh, double yw, double yh, double dx, double dy) noexcept;

	/// Creates a world file from a QTransform.
	explicit WorldFile(const QTransform& wld) noexcept;
	
	WorldFile(const WorldFile&) noexcept = default;
	WorldFile(WorldFile&&) noexcept = default;
	~WorldFile() = default;
	
	WorldFile& operator=(const WorldFile&) noexcept = default;
	WorldFile& operator=(WorldFile&&) noexcept = default;
	
	/// Returns a QTransform from pixels to projected coordinates,
	/// with (0,0) being the top-left corner of the top left pixel.
	operator QTransform() const;

	/// Tries to load the given path as world file.
	/// Returns true on success and sets loaded to true or false.
	bool load(const QString& path);

	/// Writes the world file to the given path.
	bool save(const QString& path) const;
	
	/// Tries to find and load a world file for the given image path.
	bool tryToLoadForImage(const QString& image_path);
	
	/// Returns the proposed world file path for the given image path.
	static QString pathForImage(const QString& image_path);
};


}  // namespace LibreMapper

#endif
