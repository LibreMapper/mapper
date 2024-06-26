/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "scaling_icon_engine.h"

#include <algorithm>

#include <Qt>
#include <QtGlobal>
#include <QCoreApplication>
#include <QIcon>
#include <QIconEngine>
#include <QLatin1String>
#include <QList>
#include <QPaintDevice>
#include <QPainter>
#include <QPixmap>
#include <QRect>
#include <QSize>


namespace {

/**
 * Utility class which helps determine the required scaling for a given source
 * size ("from") and requested size ("to").
 * 
 * Scaling is applied when the requested size significantly exceeds the source
 * size in both dimensions. The aspect ratio is kept.
 */
struct Scaling
{
	QSize size;
	QSize requested_size;
	qreal factor;
	
	struct ScalingFrom {
		QSize size;
		constexpr Scaling to(const QSize& requested_size) const noexcept
		{
			qreal factor = 100.0;  // maximum scaling (guessed)
			if (size.width() > 0)
				factor = std::min(factor, qreal(requested_size.height()) / size.height());
			if (size.height() > 0)
				factor = std::min(factor, qreal(requested_size.height()) / size.height());
			return { size, requested_size, factor };
		}
	};
	
	static constexpr ScalingFrom from(const QSize& size) noexcept
	{
		return { size };
	}
	
	constexpr bool isNeeded() const noexcept
	{
		return factor > 1.3;
	}
	
	constexpr QSize scaledSize() const noexcept
	{
		auto s = size;
		if (isNeeded())
			s *= factor;
		return s;
	}
};


}  // namespace



namespace LibreMapper {

// override
ScalingIconEngine::~ScalingIconEngine() = default;

ScalingIconEngine::ScalingIconEngine(const QString& filename)
: QIconEngine()
, icon(filename + QLatin1String(".bmp"))
{
	icon.addFile(filename);
}

ScalingIconEngine::ScalingIconEngine(const QIcon& icon)
: QIconEngine()
, icon(icon)
{}

// override
void ScalingIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state)
{
	auto const device_pixel_ratio = painter->device()->devicePixelRatioF();
	QSize pixmapSize = rect.size() * device_pixel_ratio;
	painter->drawPixmap(rect, pixmap(pixmapSize, mode, state));
}

// override
QPixmap ScalingIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state)
{
	auto pm = icon.pixmap(size, mode, state);
	auto const scaling = Scaling::from(pm.size()).to(size);
	if (scaling.isNeeded())
	{
		pm = pm.scaled(scaling.scaledSize(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		addPixmap(pm, mode, state);
	}
	return pm;
}

// override
QIconEngine* ScalingIconEngine::clone() const
{
	return new ScalingIconEngine(*this);
}

// override
void ScalingIconEngine::addPixmap(const QPixmap& pixmap, QIcon::Mode mode, QIcon::State state)
{
	icon.addPixmap(pixmap, mode, state);
}

// override
void ScalingIconEngine::addFile(const QString& fileName, const QSize& size, QIcon::Mode mode, QIcon::State state)
{
	icon.addFile(fileName, size, mode, state);
}

// override
QSize ScalingIconEngine::actualSize(const QSize& size, QIcon::Mode mode, QIcon::State state)
{
	auto actual_size = icon.actualSize(size, mode, state);
	auto const scaling = Scaling::from(actual_size).to(size);
	if (scaling.isNeeded())
	{
		actual_size = scaling.scaledSize();
	}
	return actual_size;
}

// override
QString ScalingIconEngine::key() const
{
	return QStringLiteral("ScalingIconEngine");
}

// override
QList<QSize> ScalingIconEngine::availableSizes(QIcon::Mode mode, QIcon::State state)
{
	return icon.availableSizes(mode, state);
}

// override
QString  ScalingIconEngine::iconName()
{
	return icon.name();
}


}  // namespace LibreMapper
