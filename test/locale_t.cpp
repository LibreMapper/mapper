/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2016 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "locale_t.h"

#include <QtTest>
#include <QLatin1String>
#include <QLocale>
#include <QString>

#include "util/translation_util.h"

using namespace LibreMapper;


namespace LibreMapper {

inline
bool operator==(const TranslationUtil::Language& first, const TranslationUtil::Language& second) {
	return first.code == second.code && first.displayName == second.displayName;
}


}  // namespace LibreMapper



LocaleTest::LocaleTest(QObject* parent)
: QObject(parent)
{
	// nothing
}


void LocaleTest::testEsperantoQLocale()
{
	QCOMPARE(QLocale::languageToString(QLocale::Esperanto), QString::fromLatin1("Esperanto"));
	
#if QT_VERSION < 0x050c00
	QEXPECT_FAIL("", "Cannot construct Esperanto QLocale from \"eo\" (issue #792).", Continue);
#endif
	QCOMPARE(QLocale(QString::fromLatin1("eo")).language(), QLocale::Esperanto);
	
#if QT_VERSION < 0x050c00
	QEXPECT_FAIL("", "Cannot construct Esperanto QLocale from \"eo_C\".", Continue);
#endif
	QCOMPARE(QLocale(QString::fromLatin1("eo_C")).language(), QLocale::Esperanto);
	
#if QT_VERSION < 0x050c00
	QEXPECT_FAIL("", "Cannot construct Esperanto QLocale for AnyScript, AnyCountry.", Continue);
#endif
	QCOMPARE(QLocale(QLocale::Esperanto, QLocale::AnyScript, QLocale::AnyCountry).language(), QLocale::Esperanto);
}

void LocaleTest::testEsperantoTranslationUtil()
{
	auto eo = QString::fromLatin1("eo");
	QCOMPARE(TranslationUtil::languageFromCode(eo).code, eo);
	QCOMPARE(TranslationUtil::languageFromCode(eo).displayName, QString::fromLatin1("Esperanto"));
	
	TranslationUtil translation { eo };
	QCOMPARE(translation.code(), eo);
	QCOMPARE(translation.displayName(), QString::fromLatin1("Esperanto"));
	
	auto test_basename = QLatin1String("LocaleTest");
	TranslationUtil::setBaseName(test_basename);
	
	auto test_filename = QString { QLatin1String("some_dir/") + test_basename + QLatin1String("_eo.qm") };
	QCOMPARE(TranslationUtil::languageFromFilename(test_filename), TranslationUtil::languageFromCode(eo));
}


QTEST_GUILESS_MAIN(LocaleTest)
