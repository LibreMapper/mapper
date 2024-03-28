/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2005-2019 Libor Pecháček.
 *
 * This file is part of CoVe.
 */

#include "coverunner.h"

#include <gui/main_window.h>

#include "mainform.h"

namespace OpenOrienteering {
class Map;
} // namespace OpenOrienteering

namespace cove {
void CoveRunner::run(OpenOrienteering::MainWindow* parent,
					 OpenOrienteering::Map* map,
					 OpenOrienteering::TemplateImage* templ)
{
	mainForm f(parent, map, templ);
	f.exec();
}
} // cove
