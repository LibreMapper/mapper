/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2005-2019 Libor Pecháček.
 *
 * This file is part of CoVe.
 */

#ifndef COVERUNNER_H
#define COVERUNNER_H

namespace LibreMapper {
class MainWindow;
class Map;
class TemplateImage;
} // namespace LibreMapper

namespace cove {
class CoveRunner
{
public:
	void run(LibreMapper::MainWindow* parent, LibreMapper::Map* map,
	         LibreMapper::TemplateImage* templ) const;
};
} // cove

#endif // COVERUNNER_H
