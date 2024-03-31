/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2016 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_TRANSFORM_T_H
#define LIBREMAPPER_TRANSFORM_T_H

#include <QObject>


/**
 * @test Tests coordinate transformation.
 */
class TransformTest : public QObject
{
Q_OBJECT
public:
	explicit TransformTest(QObject* parent = nullptr);
	
private slots:
	/**
	 * Tests basic transform operators.
	 */
	void testTransformBasics();
	
	/**
	 * Tests conversion of identity QTransform to TemplateTransform
	 */
	void testTransformIdentity();
	
	/**
	 * Tests conversion of translating QTransform to TemplateTransform
	 */
	void testTransformTranslate();
	
	/**
	 * Tests conversion of projecting QTransform to TemplateTransform
	 */
	void testTransformProject();
	
	/**
	 * Tests conversion of rotating QTransform to TemplateTransform
	 */
	void testTransformRotate();
	void testTransformRotate_data();
	
	/**
	 * Tests conversion of non-trivial QTransform to TemplateTransform
	 */
	void testTransformCombined();
	void testTransformCombined_data();
	
	/**
	 * Tests round-trip conversion TemplateTransform -> QTransform -> TemplateTransform.
	 */
	void testTransformRoundTrip();
	void testTransformRoundTrip_data();
	
	/**
	 * Test PassPointList::estimateNonIsometricSimilarityTransform.
	 */
	void testEstimateNonIsometric();
	
	/**
	 * Test PassPointList::estimateSimilarityTransformation.
	 * 
	 * \todo Cover transformation with significant error.
	 */
	void testEstimateSimilarityTransformation();
};

#endif
