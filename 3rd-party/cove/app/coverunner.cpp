/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2005-2019 Libor Pecháček.
 *
 * This file is part of CoVe.
 */

#include "coverunner.h"

#include <gui/main_window.h>

#include "mainform.h"

namespace LibreMapper {
class Map;
} // namespace LibreMapper

namespace cove {
void CoveRunner::run(LibreMapper::MainWindow* parent,
					 LibreMapper::Map* map,
					 LibreMapper::TemplateImage* templ)
{
	mainForm f(parent, map, templ);
	f.exec();
}
} // cove
