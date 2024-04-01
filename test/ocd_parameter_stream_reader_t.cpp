/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2022 Matthias Kühlewein (OpenOrienteering)
 * Copyright 2022 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include <QtGlobal>
#include <QtTest>
#include <QChar>
#include <QCharRef>
#include <QLatin1Char>
#include <QObject>
#include <QString>
#include <QStringRef>

#include "fileformats/ocd_parameter_stream_reader.h"


namespace LibreMapper
{

/**
 * @test Unit test for OCD file import.
 */
class OcdFileImportTest : public QObject
{
	Q_OBJECT

private slots:
	void firstParameter_data()
	{
		QTest::addColumn<QString>("param_string");
		QTest::addColumn<QString>("first_parameter");
		
		QTest::newRow("only first parameter") << "firstonly" << "firstonly";
		QTest::newRow("first parameter plus more") << "first\ta1\tb2.0" << "first";
	}
	
	void firstParameter()
	{
		QFETCH(QString, param_string);
		QFETCH(QString, first_parameter);
		
		auto reader = OcdParameterStreamReader(param_string);
		
		QCOMPARE(reader.value().toString(), first_parameter);
		QCOMPARE(reader.key(), OcdParameterStreamReader::noKey());
	}
	
	
	void fullCheck_data()
	{
		QTest::addColumn<QString>("param_string");
		
		QTest::newRow("Stringtype: 8 (BackgroundMap)") << "C:\\user\\OL\\Karten\\Hagelloch\\Ortho\10-02-_2020_13-58-12.png\ts0\tr1\tx-54.763125\ty-118.991563\ta-0.07881792\tb-0.07881792\tu0.0196680528\tv0.0196680528";
		QTest::newRow("Stringtype: 9 (Color)") << "All printing colours\tn1\tk100\tc100\tm100\ty100\tsProcess Black\tp100\tsPMS471_Brown\tp100\tsPMS136_Yellow\tp100\tsPMS299_Blue\tp100\tsPMS361_Green\tp100\tsPMS428_Grey\tp100\tsPurple\tp100\to0\tt100\tbNormal";
		QTest::newRow("Stringtype: 9 (Color)") << "Lower Purple for Course Overprint\tn52\tc30\tm100\ty0\tk0\to1\tt100\tbDarken\tsPurple\tp100";
		QTest::newRow("Stringtype: 10 (SpotColor)") << "PMS471_Brown\tn2\tv1\tc0.0\tm56.0\ty81.5\tk18.5\tf1500.0\ta45.0";
		QTest::newRow("Stringtype: 12 (Zoom)") << "\tx-27.403086\ty-42.832344\tz3.784824";
		QTest::newRow("Stringtype: 1024 (DisplayPar)") << "\tt1\tg-1\tp188\tv190\ts128\tzVERT";
		QTest::newRow("Stringtype: 1030 (ViewPar)") << "\tx11.120000\ty-2.010000\tz14.301646\tv0\tm100\tt100\tb50\tc50\th0\tk0\tp0\td0\ti0\tl0";
		QTest::newRow("Stringtype: 1039 (ScalePar)") << "\tm10000\tg50.0000\tr1\tx502000\ty5378000\ta0.00000000\td1000\ti2032\tb0.00\tc0.00";
		QTest::newRow("Empty String") << "";
		QTest::newRow("String with tab without value") << "\tx11.120000\t \ty-2.010000";
	}
	
	void fullCheck()
	{
		QFETCH(QString, param_string);
		
		auto reader = OcdParameterStreamReader(param_string);
		
		int i = param_string.indexOf(QLatin1Char('\t'), 0);
		if (i > 0)	// 'first' parameter
		{
			const QString name = param_string.left(qMax(-1, i)); // copied
			
			QCOMPARE(reader.value().toString(), name);
			QCOMPARE(reader.key(), OcdParameterStreamReader::noKey());
		}
		
		while (i >= 0)
		{
			int next_i = param_string.indexOf(QLatin1Char('\t'), i+1);
			int len = (next_i > 0 ? next_i : param_string.length()) - i - 2;
			auto param_value = param_string.midRef(i+2, len);
			char key = param_string[i+1].toLatin1();
			
			QVERIFY(reader.readNext());
			QCOMPARE(reader.key(), key);
			QCOMPARE(reader.value(), param_value);
			
			i = next_i;
		}
		QVERIFY(!reader.readNext());		// check that there are no more parameters available
		QVERIFY(reader.atEnd());		// test atEnd() function
	}
	
	
	void irregularPatterns_data()
	{
		QTest::addColumn<QString>("param_string");
		QTest::addColumn<QString>("result_pattern");
		
		QTest::newRow("Empty string") << "" << "";
		QTest::newRow("Only tabs")    << "\t\t" << "";
		QTest::newRow("Leading tabs") << "\t\t\ta73" << "a";
		QTest::newRow("Trailing tab") << "\ta73\t" << "a";
		QTest::newRow("Multiple separating tabs") << "\ta73\t\t\tb73" << "ab";
		QTest::newRow("Keys without value") << "\ta73\tB\t\tc73\tD" << "aBcD";	// uppercase: check for key without parameter
	}
	
	void irregularPatterns()
	{
		QFETCH(QString, param_string);
		QFETCH(QString, result_pattern);
		
		auto reader = OcdParameterStreamReader(param_string);
		
		for (auto expected_key : result_pattern)
		{
			QVERIFY(!reader.atEnd());
			QVERIFY(reader.readNext());
			QCOMPARE(reader.key(), expected_key.toLatin1());
			if (expected_key.isLower())
				QVERIFY(reader.value().toInt() == 73);
			else
				QVERIFY(reader.value().isEmpty());
		}
		QVERIFY(!reader.readNext());
		QVERIFY(reader.atEnd());
	}
	
};  // class OcdFileImportTest


}  // namespace LibreMapper



QTEST_APPLESS_MAIN(LibreMapper::OcdFileImportTest)

#include "ocd_parameter_stream_reader_t.moc"  // IWYU pragma: keep
