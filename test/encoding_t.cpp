/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2016 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "encoding_t.h"

#include <QtTest>
#include <QLatin1String>
#include <QLocale>
#include <QString>
#include <QTextCodec>

#include "util/encoding.h"

using namespace LibreMapper;


EncodingTest::EncodingTest(QObject* parent)
: QObject(parent)
{
	// nothing
}


void EncodingTest::testCodepageForLanguage_data()
{
	// We use the row data tag (const char*) for the language.
	QTest::addColumn<QString>("encoding");
	QTest::newRow("cs")    << QString::fromLatin1("Windows-1250");
	QTest::newRow("cs_x")  << QString::fromLatin1("Windows-1250");
	QTest::newRow("ru")    << QString::fromLatin1("Windows-1251");
	QTest::newRow("ru_RU") << QString::fromLatin1("Windows-1251");
	QTest::newRow("de")    << QString::fromLatin1("Windows-1252");
	QTest::newRow("??")    << QString::fromLatin1("Windows-1252");
}

void EncodingTest::testCodepageForLanguage()
{
	auto language = QTest::currentDataTag();
	QFETCH(QString, encoding);
	QCOMPARE(QLatin1String(Util::codepageForLanguage(QString::fromLatin1(language))), encoding);
}


void EncodingTest::testCodecForName()
{
	QVERIFY(Util::codecForName("Windows-1250") == Util::codecForName("Windows-1250"));
	QVERIFY(Util::codecForName("Windows-1250") != Util::codecForName("Windows-1251"));
	QVERIFY(Util::codecForName("Windows-1250") == QTextCodec::codecForName("Windows-1250"));
	QVERIFY(Util::codecForName("Windows-1250") != QTextCodec::codecForName("Windows-1251"));
	QVERIFY(Util::codecForName("Default") == QTextCodec::codecForName(Util::codepageForLanguage(QLocale().name())));
}


QTEST_APPLESS_MAIN(EncodingTest)
