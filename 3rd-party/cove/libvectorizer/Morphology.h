/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2005-2019 Libor Pecháček.
 *
 * This file is part of CoVe.
 */

#ifndef COVE_THINNER_H
#define COVE_THINNER_H

#include <QImage>

namespace cove {
class ProgressObserver;

class Morphology
{
protected:
	static const unsigned int masks[];
	static const bool todelete[];
	static const bool isDeletable[];
	static const bool isInsertable[];
	static const bool isPrunable[];
	QImage image, thinnedImage;
	bool runMorpholo(const bool* table, bool insert,
	                 ProgressObserver* progressObserver = nullptr);
	int modifyImage(const bool* table, bool insert,
	                ProgressObserver* progressObserver = nullptr);

public:
	Morphology(const QImage& img);
	bool rosenfeld(ProgressObserver* progressObserver = nullptr);
	bool erosion(ProgressObserver* progressObserver = nullptr);
	bool dilation(ProgressObserver* progressObserver = nullptr);
	bool pruning(ProgressObserver* progressObserver = nullptr);
	QImage getImage() const;
};
} // cove

#endif
