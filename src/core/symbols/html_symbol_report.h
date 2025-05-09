/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2024 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_HTML_SYMBOL_REPORT_H
#define LIBREMAPPER_HTML_SYMBOL_REPORT_H

class QString;

namespace LibreMapper {

class Map;

/**
 * Generates a symbol set report in HTML format.
 */
QString makeHTMLSymbolReport(const Map& map);

}  // namespace LibreMapper

#endif  // LIBREMAPPER_HTML_SYMBOL_REPORT_H
