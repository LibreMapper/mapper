/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2014, 2018 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef OPENORIENTEERING_SYMBOL_ICON_DECORATOR_H
#define OPENORIENTEERING_SYMBOL_ICON_DECORATOR_H

#include <QPoint>

class QPainter;

namespace OpenOrienteering {


/**
 * An abstract interface for classes which draws icon decorations.
 * 
 * The icon is expected to be at (0, 0) in the painter's coordinates.
 */
class SymbolIconDecorator
{
public:
	SymbolIconDecorator() noexcept = default;
	SymbolIconDecorator(const SymbolIconDecorator&) = delete;
	SymbolIconDecorator(SymbolIconDecorator&&) = delete;
	SymbolIconDecorator& operator=(const SymbolIconDecorator&) = delete;
	SymbolIconDecorator& operator=(SymbolIconDecorator&&) = delete;
	virtual ~SymbolIconDecorator();
	virtual void draw(QPainter& p) const = 0;
};



/**
 * Draws the decoration for a hidden symbol.
 * 
 * A small red x is drawn in the top-left corner of the icon.
 */
class HiddenSymbolDecorator : public SymbolIconDecorator
{
public:
	explicit HiddenSymbolDecorator(int icon_size);
	~HiddenSymbolDecorator() override;
	void draw(QPainter& p) const override;
	
private:
	int icon_size;
	int pen_width;
	int x_width;
	QPoint offset;
};



/**
 * Draws the decoration for a protected symbol.
 * 
 * A small gray lock is drawn in the top-right corner of the icon.
 */
class ProtectedSymbolDecorator : public SymbolIconDecorator
{
public:
	explicit ProtectedSymbolDecorator(int icon_size);
	~ProtectedSymbolDecorator() override;
	void draw(QPainter& p) const override;
	
private:
	int arc_size;
	int pen_width;
	int box_width;
	int box_height;
	QPoint offset;
};


}  // namespace OpenOrienteering

#endif  // OPENORIENTEERING_SYMBOL_ICON_DECORATOR_H
