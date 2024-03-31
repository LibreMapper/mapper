/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (C) 2013-2020 Mattia Basaglia (OpenOrienteering)
 * Copyright 2021 Libor Pecháček (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_COLOR_WHEEL_WIDGET_H
#define LIBREMAPPER_COLOR_WHEEL_WIDGET_H

#include <QColor>
#include <QDialog>
#include <QFrame>
#include <QImage>
#include <QLineF>
#include <QObject>
#include <QPixmap>
#include <QPointF>
#include <QSize>
#include <QString>
#include <QWidget>
#include <Qt>
#include <QtGlobal>

class QLineEdit;
class QLineF;
class QMouseEvent;
class QPaintEvent;
class QPainter;
class QPoint;
class QPointF;
class QResizeEvent;

namespace LibreMapper {


/**
 * \brief Display an analog widget that allows the selection of a HSV color
 *
 * It has an outer wheel to select the Hue and an internal square to select
 * Saturation and Lightness.
 */
class ColorWheel : public QWidget
{
	Q_OBJECT
public:
	explicit ColorWheel(QWidget* parent = 0);
	~ColorWheel();

	/// Get current color
	QColor color() const;

public slots:

	/// Set current color
	void setColor(QColor c);

signals:
	/**
	 * Emitted when the user selects a color or setColor is called
	 */
	void colorChanged(QColor);

	/**
	 * Emitted when the user selects a color
	 */
	void colorSelected(QColor);

	/**
	 * Emitted when the user releases from dragging
	 */
	void editingFinished();

protected:
	enum MouseStatus
	{
		Nothing,
		DragCircle,
		DragInside
	};

	qreal hue = {};
	qreal sat = {};
	qreal val = {};
	MouseStatus mouse_status = Nothing;
	bool background_is_dark = {};

	unsigned int wheel_width = 20;
	static constexpr qreal selector_radius = 6;

	QPixmap hue_ring;
	QImage inner_selector;

	void paintEvent(QPaintEvent*event) override;
	void mouseMoveEvent(QMouseEvent*event) override;
	void mousePressEvent(QMouseEvent*event) override;
	void mouseReleaseEvent(QMouseEvent*event) override;
	void resizeEvent(QResizeEvent*event) override;

	void renderHueRing();
	void renderValSatTriangle();
	qreal outerRadius() const;
	void drawRingEditor(double editor_hue, QPainter& painter, QColor color);
	qreal innerRadius() const;
	QLineF lineToPoint(const QPoint& p) const;
	qreal selectorImageAngle() const;
	QPointF selectorImageOffset();
	qreal triangleSide() const;
	qreal triangleHeight() const;
};


/**
 * \brief A helper widget that displays chosen color on its face.
 *
 * Intended as a color preview area in ColorWheelDialog.
 */
class ColorPreview : public QFrame
{
	Q_OBJECT

public:
	ColorPreview(QWidget* parent = nullptr);
	virtual void paintEvent(QPaintEvent* event) override;
	virtual QSize sizeHint() const override;
	QColor color();

public slots:
	void setColor(QColor c);

private:
	QColor display_color;
};


/**
 * \brief A minimalistic color selection dialog intended for on mobile devices.
 *
 * It offers a color wheel, color preview in a rectangle and input field for
 * a direct text entry. The programming interface tries to mimic QColorDialog.
 */
class ColorWheelDialog : public QDialog
{
	Q_OBJECT
public:
	ColorWheelDialog(QWidget* parent = nullptr,
	                 Qt::WindowFlags f = Qt::WindowFlags());
	QColor currentColor() const;
	void setCurrentColor(const QColor& c);
	static QColor getColor(const QColor& initial, QWidget* parent = nullptr);

private:
	ColorWheel* wheel = {};
	QLineEdit* text_edit = {};
};


} //  namespace LibreMapper

#endif
