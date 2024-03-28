/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2005-2019 Libor Pecháček.
 * Copyright 2020 Kai Pastor
 *
 * This file is part of CoVe.
 */

#ifndef COVE_VECTORIZER_H
#define COVE_VECTORIZER_H

#include <memory>
#include <vector>

#include <QImage>
#include <QRgb>

namespace cove {

class MapColor;
class ProgressObserver;

class Vectorizer
{
public:
	enum ColorSpace
	{
		COLSPC_RGB,
		COLSPC_HSV
	};
	enum LearningMethod
	{
		KOHONEN_CLASSIC,
		KOHONEN_BATCH
	};
	enum AlphaStrategy
	{
		ALPHA_CLASSIC,
		ALPHA_NEUQUANT
	};
	enum PatternStrategy
	{
		PATTERN_RANDOM,
		PATTERN_NEUQUANT
	};
	enum MorphologicalOperation
	{
		EROSION,
		DILATION,
		THINNING_ROSENFELD,
		PRUNING
	};

protected:
	QImage sourceImage;
	QImage classifiedImage;
	QImage bwImage;
	QImage thinnedBWImage;
	std::vector<std::shared_ptr<MapColor>> sourceImageColors;
	std::unique_ptr<MapColor> mc;
	std::vector<bool> selectedColors;  // companion to bwImage
	int E;
	double initAlpha, q, minAlpha, p, quality;
	LearningMethod learnMethod;
	ColorSpace colorSpace;
	AlphaStrategy alphaStrategy;
	PatternStrategy patternStrategy;

	void deleteColorsTable();

public:
	virtual ~Vectorizer();
	Vectorizer();
	explicit Vectorizer(QImage& im);
	virtual void setClassificationMethod(LearningMethod learnMethod);
	virtual void setColorSpace(ColorSpace colorSpace);
	virtual void setP(double p);
	virtual void setAlphaStrategy(AlphaStrategy alphaStrategy);
	virtual void setPatternStrategy(PatternStrategy patternStrategy);
	virtual void setInitAlpha(double initAlpha);
	virtual void setMinAlpha(double minAlpha);
	virtual void setQ(double q);
	virtual void setE(int E);
	virtual void setNumberOfColors(int nColors);
	virtual void setInitColors(const std::vector<QRgb>& initColors);
	virtual bool performClassification(ProgressObserver* progressObserver = nullptr);
	std::vector<QRgb> getClassifiedColors();
	virtual QImage getClassifiedImage(double* qualityPtr = nullptr,
	                                  ProgressObserver* progressObserver = nullptr);
	virtual QImage getBWImage(std::vector<bool> selectedColors,
	                          ProgressObserver* progressObserver = nullptr);
	virtual QImage getTransformedImage(MorphologicalOperation mo,
	                                   ProgressObserver* progressObserver = nullptr);
	static QImage getTransformedImage(const QImage& bwImage, MorphologicalOperation mo,
	                                  ProgressObserver* progressObserver = nullptr);
};
} // cove

#endif
