/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2005-2019 Libor Pecháček.
 *
 * This file is part of CoVe.
 */

#ifndef COVE_SETTINGS_H
#define COVE_SETTINGS_H

#include <vector>

#include <QRgb>
#include <QString>

namespace cove {
class Settings
{
	struct DoubleParam
	{
		QString name;
		double value;
	};
	std::vector<DoubleParam> doubleTab;
	struct ColorParam
	{
		QRgb color;
		QString comment;
	};
	std::vector<ColorParam> colorsTab;

public:
	Settings();
	double getDouble(const QString& attname) const;
	int getInt(const QString& attname) const;
	std::vector<QRgb> getInitColors(std::vector<QString>& comments) const;
	std::vector<QRgb> getInitColors() const;
	bool setDouble(const QString& attname, double attvalue);
	bool setInt(const QString& attname, int attvalue);
	bool setInitColors(const std::vector<QRgb>& clrs,
	                   const std::vector<QString>& comments);
};
} // cove

#endif
