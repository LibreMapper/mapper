/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Thomas Schöps (OpenOrienteering)
 * Copyright 2012-2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_TEMPLATE_MAP_H
#define LIBREMAPPER_TEMPLATE_MAP_H

#include <memory>
#include <vector>

#include <QtGlobal>
#include <QObject>
#include <QString>

#include "templates/template.h"

class QByteArray;
class QPainter;
class QRectF;
class QTransform;
class QWidget;

namespace LibreMapper {

class Map;


/**
 * Template displaying a map file.
 */
class TemplateMap : public Template
{
Q_OBJECT
public:
	/**
	 * Returns the filename extensions supported by this template class.
	 */
	static const std::vector<QByteArray>& supportedExtensions();
	
	
	TemplateMap(const QString& path, Map* map);
	
protected:
	TemplateMap(const TemplateMap& proto);
	
public:
	~TemplateMap() override;
	
	TemplateMap* duplicate() const override;
	
	
	const char* getTemplateType() const override;
	
	bool isRasterGraphics() const override;
	
	bool preLoadSetup(QWidget* dialogParent) override;
	
	bool loadTemplateFileImpl() override;
	
	bool postLoadSetup(QWidget* dialog_parent, bool& out_center_in_view) override;
	
	void unloadTemplateFileImpl() override;
	
	
	void drawTemplate(QPainter* painter, const QRectF& clip_rect, double scale, bool on_screen, qreal opacity) const override;
	
	QRectF getTemplateExtent() const override;
	
	
	bool hasAlpha() const override;
	
	
	bool canChangeTemplateGeoreferenced() const override;
	
	bool trySetTemplateGeoreferenced(bool value, QWidget* dialog_parent) override;
	
	
	const Map* templateMap() const;
	
	/**
	 * Returns the template's map.
	 * 
	 * The template must be in loaded state before calling this method.
	 * The template will be in unloaded state afterwards.
	 */
	std::unique_ptr<Map> takeTemplateMap();
	
protected:
	Map* templateMap();
	
	void setTemplateMap(std::unique_ptr<Map>&& map);
	
	
	void mapProjectionChanged();
	
	virtual void mapTransformationChanged();
	
	
	void reloadLater();
	
	void reload();
	
	
	bool calculateTransformation(QTransform& q_transform) const;
	
public:
	/**
	 * Returns the template transformation for a pure OCD configuration.
	 * 
	 * OCD templates in OCD files must use the map's scale and georeferencing.
	 * Only the template file's grid parameters are taken into account, i.e.
	 * the template OCD file must be loaded in order to get this transform.
	 * 
	 * If template_map is null (i.e. the template is not in loaded state), this
	 * function returns the current transformation.
	 */
	TemplateTransform transformForOcd() const;
	
	/**
	 * The name of the QObject property which activates a pure OCD transformation.
	 * 
	 * When this property is set for a TemplateMap, it applies transformForOCD()
	 * upon first loading of the file, and resets the property after loading.
	 */
	static const char* ocdTransformProperty();
	
private:
	bool georeferencedStateSupported() const;
	
	std::unique_ptr<Map> template_map;
	bool reload_pending = false;
	
	/**
	 * Flag to request end of georeferenced state.
	 * 
	 * Even if georeferencing is available, it is useful to avoid the
	 * georeferenced template state because it cause different transformations
	 * of map objects and map symbols on map loading. By setting this flag,
	 * the template map's georeferencing will be used only to calculate an
	 * initial template transformation, and then the georeferenced state will
	 * be cleared. Note that this initialization matches the behaviour before
	 * introducing support for georeferenced maps.
	 */
	bool block_georeferencing = false;
	
	static QStringList locked_maps;
};


}  // namespace LibreMapper

#endif
