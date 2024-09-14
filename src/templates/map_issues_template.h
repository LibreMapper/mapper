/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2024 Libor Pecháček
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_MAP_ISSUES_TEMPLATE_H
#define LIBREMAPPER_MAP_ISSUES_TEMPLATE_H

#include <QObject>
#include <QRectF>
#include <QtTypes>

#include "templates/template.h"

class QDir;
class QPainter;
class QXmlStreamWriter;

namespace LibreMapper {

class Map;

class MapIssuesTemplate : public Template
{
	Q_OBJECT
public:
	MapIssuesTemplate() = delete;
	MapIssuesTemplate(Map* map);
	~MapIssuesTemplate();
	virtual void drawTemplate(QPainter* painter, const QRectF& clip_rect, double scale, bool on_screen, qreal opacity) const override;
	virtual Template* duplicate() const override;
	virtual const char* getTemplateType() const override;
	virtual bool fileExists() const override;
	virtual bool isRasterGraphics() const override;
	virtual bool loadTemplateFileImpl() override;
	virtual void unloadTemplateFileImpl() override;
	virtual QRectF calculateTemplateBoundingBox() const override;
	void saveTemplateConfiguration(QXmlStreamWriter& xml, bool open, const QDir* map_dir) const override;
	void ensureVisible();

public slots:
	void mapObjectsEdited();

private:
	Map* map;
	int our_template_pos = -1;
};

} // namespace LibreMapper

#endif // LIBREMAPPER_MAP_ISSUES_TEMPLATE_H
