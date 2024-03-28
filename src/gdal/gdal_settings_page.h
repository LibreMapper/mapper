/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2016-2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef OPENORIENTEERING_GDAL_SETTINGS_PAGE_H
#define OPENORIENTEERING_GDAL_SETTINGS_PAGE_H

#include <QObject>
#include <QString>

#include "gui/widgets/settings_page.h"

class QCheckBox;
class QTableWidget;
class QWidget;

namespace OpenOrienteering {


class GdalSettingsPage : public SettingsPage
{
Q_OBJECT
public:
	explicit GdalSettingsPage(QWidget* parent = nullptr);
	
	~GdalSettingsPage() override;
	
	QString title() const override;

	void apply() override;
	
	void reset() override;
	
protected:
	void updateWidgets();
	
	void cellChange(int row, int column);
	
	int findDuplicateKey(const QString& key, int row) const;
	
private:
	QCheckBox* clip_layers;
	QCheckBox* import_gpx;
	QCheckBox* view_hatch;
	QCheckBox* view_baseline;
	QCheckBox* export_one_layer_per_symbol;
	QTableWidget* parameters;
};


}  // namespace OpenOrienteering

#endif
