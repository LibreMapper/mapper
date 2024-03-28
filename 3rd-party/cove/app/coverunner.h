/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2005-2019 Libor Pecháček.
 *
 * This file is part of CoVe.
 */

#ifndef COVERUNNER_H
#define COVERUNNER_H

namespace OpenOrienteering {
class MainWindow;
class Map;
class TemplateImage;
} // namespace OpenOrienteering

namespace cove {
class CoveRunner
{
public:
	void run(OpenOrienteering::MainWindow* parent, OpenOrienteering::Map* map,
	         OpenOrienteering::TemplateImage* templ);
};
} // cove

#endif // COVERUNNER_H
