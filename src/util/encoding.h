/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2016 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_UTIL_ENCODING_H
#define LIBREMAPPER_UTIL_ENCODING_H

class QString;
class QTextCodec;

namespace LibreMapper {

namespace Util {

/**
 * Determines the name of the 8-bit legacy codepage for a language.
 * 
 * This function accepts language names as returned by QLocale::name().
 * Characters after the two letter language code are ignored.
 * 
 * If the language is unknown, it returns "Windows-1252".
 * 
 * @param language_name A lowercase, two-letter ISO 639 language code
 * @return A Windows codepage name.
 */
const char* codepageForLanguage(const QString& language_name);

/**
 * Determines the codec for a given name.
 * 
 * This function wraps QTextCodec::codecForName. Other than that function,
 * it will try to lookup the codepage name for the current locale if the
 * name is "Default" (case sensitive). It may return nullptr.
 */
QTextCodec* codecForName(const char* name);


}  // namespace Util

}  // namespace LibreMapper

#endif
