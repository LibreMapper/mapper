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
	static unsigned int masks[];
	static bool todelete[];
	static bool isDeletable[];
	static bool isInsertable[];
	static bool isPrunable[];
	QImage image, thinnedImage;
	bool runMorpholo(bool* table, bool insert,
	                 ProgressObserver* progressObserver = nullptr);
	int modifyImage(bool* table, bool insert,
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
