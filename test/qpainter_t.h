/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012, 2013 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef OPENORIENTEERING_QPAINTER_T_H
#define OPENORIENTEERING_QPAINTER_T_H

#include <QImage>
#include <QObject>
#include <QPainter>


/**
 * @test Tests QPainter properties we rely on.
 */
class QPainterTest : public QObject
{
Q_OBJECT
public:
	/**
	 * The constructor initializes a number of single pixel images (black, 
	 * white, transparent) which are used by the tests.
	 */
	explicit QPainterTest(QObject* parent = nullptr);
	
private slots:
	/**
	 * Verifies basic assumptions about the colors of pixels.
	 */
	void initTestCase();
	
	/** 
	 * QPainter::CompositionMode_SourceOver is the default mode of composing
	 * map elements. The alpha of the source is used to blend the pixel on top 
	 * of the destination.
	 */
	void sourceOverCompostion();
	
	/** 
	 * QPainter::CompositionMode_Multiply is a mode suitable for spot color
	 * overprinting simulation. The output is the source color multiplied by 
	 * the destination. Multiplying a color with white shall leave the color 
	 * unchanged. Multiplying a color with black shall produce black. The alpha 
	 * of the source shall be used to blend the pixel on top of the destination.
	 */
	void multiplyComposition();
	
	/** 
	 * QPainter::CompositionMode_Darken is a mode suitable for spot color
	 * overprinting simulation. The darker of the source and destination colors
	 * shall be selected.
	 */
	void darkenComposition();
	
protected:
	/** 
	 * Creates a single pixel image of the given color.
	 */
	template <typename ColorT>
	QImage makeImage(ColorT color) const;
	
	/**
	 * Composes two images with the given mode, and returns the result.
	 */
	QImage compose(const QImage& source, const QImage& dest, QPainter::CompositionMode mode);
	
	/** A single-pixel white image. */
	const QImage white_img;
	
	/** A single-pixel black image. */
	const QImage black_img;
	
	/** A single-pixel transparent image. */
	const QImage trans_img;
};

#endif
