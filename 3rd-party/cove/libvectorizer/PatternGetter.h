/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2005-2019 Libor Pecháček.
 *
 * This file is part of CoVe.
 */

#ifndef COVE_PATTERNGETTER_H
#define COVE_PATTERNGETTER_H

#include <QImage>

#include "KohonenMap.h"

namespace cove {
class MapColor;
class ProgressObserver;

class PatternGetterDataMembers
{
protected:
	const QImage& image;
	MapColor* retval;
	int width, height;
	PatternGetterDataMembers(const QImage& i, MapColor* mc);
	~PatternGetterDataMembers();
};

class RandomPatternGetter : public KohonenPatternGetter,
                            protected PatternGetterDataMembers
{
private:
	RandomPatternGetter();

public:
	RandomPatternGetter(const QImage& im, MapColor* mc);
	~RandomPatternGetter() override;
	const OrganizableElement* getPattern() override;
};

class SequentialPatternGetter : public BatchPatternGetter,
                                protected PatternGetterDataMembers
{
protected:
	QImage classifiedImage;
	ProgressObserver* progressObserver;
	int x, y, nChanges;

private:
	SequentialPatternGetter();

public:
	SequentialPatternGetter(const QImage& im, MapColor* mc,
	                        ProgressObserver* progressObserver = nullptr);
	~SequentialPatternGetter() override;
	const OrganizableElement* getPattern() override;
	int getLastElementClass() const override;
	void setLastElementClass(int classNumber) override;
	void reset() final;
	int numberOfChanges() override;
	virtual QImage* getClassifiedImage();
};
} // cove

#endif
