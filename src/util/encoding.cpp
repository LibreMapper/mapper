/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2016 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "encoding.h"

#include <QByteArray>
#include <QLocale>
#include <QString>
#include <QStringView>
#include <QTextCodec>


namespace {

struct LanguageMapping
{
	const char* languages;
	const char* codepage;
};

// Languages are listed as two letters, separated by single char
const LanguageMapping mappings[] = {
	{ "cs hu pl", "Windows-1250" }, // Central European
	{ "bg ru uk", "Windows-1251" }, // Cyrillic
	{ "et lt lv", "Windows-1257" }, // Baltic
	{ "el", "Windows-1253" }, // Greek
	{ "he", "Windows-1255" }, // Hebrew
#ifdef ENABLE_ALL_LANGUAGE_MAPPINGS
	{ "da de en es fi fr id it nb nl pt sv", "Windows-1252" }, // Western European (default)
	{ "tr", "Windows-1254" }, // Turkish
	{ "ar", "Windows-1256" }, // Arabic
	{ "vi", "Windows-1258" }, // Vietnamese
#endif
};

}  // namespace



namespace LibreMapper {

const char* Util::codepageForLanguage(const QString& language_name)
{
	const auto language = QStringView{language_name}.left(2).toLatin1();
	for (const auto& mapping : mappings)
	{
		auto len = qstrlen(mapping.languages);
		for (decltype(len) i = 0; i < len; i += 3)
		{
			if (language == QByteArray::fromRawData(mapping.languages+i, 2))
				return mapping.codepage;
		}
	}
	return "Windows-1252";
}


QTextCodec* Util::codecForName(const char* name)
{
	if  (qstrcmp(name, "Default") == 0)
		return QTextCodec::codecForName(Util::codepageForLanguage(QLocale().name()));
	else
		return QTextCodec::codecForName(name);
}


}  // namespace LibreMapper
