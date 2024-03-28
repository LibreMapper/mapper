/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef OPENORIENTEERING_SCALING_ICON_ENGINE_H
#define OPENORIENTEERING_SCALING_ICON_ENGINE_H

#include <QIcon>
#include <QIconEngine>
#include <QList>
#include <QPixmap>
#include <QSize>
#include <QString>

class QPainter;
class QRect;

namespace OpenOrienteering {


/**
 * An icon engine that can scale up icons.
 * 
 * This engine relies on BMP icons being handled by the regular Qt engine.
 */
class ScalingIconEngine : public QIconEngine
{
public:
	~ScalingIconEngine();
	
	explicit ScalingIconEngine(const QString& filename);

	explicit ScalingIconEngine(const QIcon& icon);

	ScalingIconEngine(const ScalingIconEngine&) = default;
	
	ScalingIconEngine& operator=(const ScalingIconEngine&) = delete;
	
	void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) override;
	
	QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override;
	
	QIconEngine* clone() const override;
	
	void addPixmap(const QPixmap& pixmap, QIcon::Mode mode, QIcon::State state) override;
	
	void addFile(const QString& fileName, const QSize& size, QIcon::Mode mode, QIcon::State state) override;
	
	QSize actualSize(const QSize& size, QIcon::Mode mode, QIcon::State state) override;
	
	QString key() const override;
	
	QList<QSize> availableSizes(QIcon::Mode mode, QIcon::State state) const override;
	
	QString iconName() const override;
	
private:
	QIcon icon;
	
};


}  // namespace OpenOrienteering

#endif // OPENORIENTEERING_SCALING_ICON_ENGINE_H
