/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2019-2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include <QtTest>
#include <QDir>           // IWYU pragma: keep
#include <QFileInfo>      // IWYU pragma: keep
#include <QObject>
#include <QSignalSpy>     // IWYU pragma: keep
#include <QStandardPaths> // IWYU pragma: keep
#include <QStaticPlugin>  // IWYU pragma: keep
#include <QTemporaryFile> // IWYU pragma: keep

#include "test_config.h"  // IWYU pragma: keep

namespace LibreMapper {}
using namespace LibreMapper;


#ifdef QT_POSITIONING_LIB
#include <QGeoPositionInfoSource>
#endif

#ifdef MAPPER_USE_FAKE_POSITION_PLUGIN
#include <QGeoCoordinate>           // IWYU pragma: keep
#include <QNmeaPositionInfoSource>  // IWYU pragma: keep
#include "sensors/fake_position_plugin.h"
#include "sensors/fake_position_source.h"
Q_IMPORT_PLUGIN(FakePositionPlugin)
#endif

#ifdef MAPPER_USE_POWERSHELL_POSITION_PLUGIN
#include "sensors/powershell_position_source.h"
Q_IMPORT_PLUGIN(PowershellPositionPlugin)
#endif


class SensorsTest : public QObject
{
	Q_OBJECT
	
private slots:
	void initTestCase()
	{
		QDir::addSearchPath(QStringLiteral("testdata"), QDir(QString::fromUtf8(MAPPER_TEST_SOURCE_DIR)).absoluteFilePath(QStringLiteral("data")));
	}
	
	
#if defined(MAPPER_USE_FAKE_POSITION_PLUGIN)
	void fakePositionSourcePluginTest()
	{
		auto sources = QGeoPositionInfoSource::availableSources();
		QVERIFY(sources.contains(QStringLiteral("Fake position")));
	}
	
	void fakePositionSourceSimulatedTest()
	{
		auto const reference = QGeoCoordinate{10, 20, 100};
		FakePositionSource::setReferencePoint(reference);
		
		auto* source = QGeoPositionInfoSource::createSource(QStringLiteral("Fake position"), this);
		QVERIFY(source);
		QCOMPARE(int(source->error()), int(QGeoPositionInfoSource::NoError));
		
		QSignalSpy source_spy(source, &QGeoPositionInfoSource::positionUpdated);
		QVERIFY(source_spy.isValid());
		
		source->startUpdates();
		QCOMPARE(int(source->error()), int(QGeoPositionInfoSource::NoError));
		QVERIFY(source_spy.wait());
		
		auto last = source->lastKnownPosition(true);
		QVERIFY(last.isValid());
		QVERIFY(qAbs(last.coordinate().latitude() - reference.latitude()) < 0.002);
		QVERIFY(qAbs(last.coordinate().longitude() - reference.longitude()) < 0.002);
		QVERIFY(qAbs(last.coordinate().altitude() - reference.altitude()) < 20);
		
		source->stopUpdates();
		delete source;
	}
#endif  // MAPPER_USE_FAKE_POSITION_PLUGIN
	
#if defined(MAPPER_USE_POWERSHELL_POSITION_PLUGIN)
	void powershellPositionSourcePluginTest()
	{
		auto powershell_position_source = QStringLiteral("Windows");
		auto sources = QGeoPositionInfoSource::availableSources();
		QVERIFY(sources.contains(powershell_position_source));  // JSON file properties
	}
		
	void powershellPositionSourceLiveTest()
	{
		auto powershell_path = QStandardPaths::findExecutable(QStringLiteral("powershell.exe"));
#if !defined(Q_OS_WIN)
		if (powershell_path.isEmpty())
			QSKIP("Powershell not available");
#endif
		QVERIFY(!powershell_path.isEmpty());
		
		auto powershell_position_source = QStringLiteral("Windows");
		auto* source = QGeoPositionInfoSource::createSource(powershell_position_source, this);
		QCOMPARE(source->error(), QGeoPositionInfoSource::NoError);
		
		QSignalSpy source_spy(source, &QGeoPositionInfoSource::positionUpdated);
		source->startUpdates();
		if (source_spy.wait(2000))
			return;
		
		switch(source->error())
		{
		case QGeoPositionInfoSource::NoError:
			QWARN("startUpdates(): QGeoPositionInfoSource::NoError");  // and no position yet!
			break;
		case QGeoPositionInfoSource::AccessError:
			QWARN("startUpdates(): QGeoPositionInfoSource::AccessError");
			break;
		case QGeoPositionInfoSource::ClosedError:
			QFAIL("startUpdates(): QGeoPositionInfoSource::ClosedError");
			break;
		default:
			QFAIL("startUpdates(): unknown error");
			break;
		}
	}
	
	void powershellPositionSourceSimulatedTest()
	{
		auto setup_function = [](QProcess& process, QByteArray&, QByteArray&) -> QGeoPositionInfoSource::Error {
			auto test_file = QFileInfo(QStringLiteral("testdata:sensors/powershell_position.txt"));
			if (!test_file.exists())
				return QGeoPositionInfoSource::UnknownSourceError;
			
#if defined(Q_OS_WIN)
			process.setProgram(QStandardPaths::findExecutable(QStringLiteral("cmd")));
			process.setArguments({QStringLiteral("/C"), QStringLiteral("type"), QDir::toNativeSeparators(test_file.absoluteFilePath())});
#else
			process.setProgram(QStringLiteral("/bin/cat"));
			process.setArguments({test_file.absoluteFilePath()});
#endif
			return QGeoPositionInfoSource::NoError;
		};
		PowershellPositionSource source(*setup_function);
		QCOMPARE(int(source.error()), int(QGeoPositionInfoSource::NoError));
		
		QSignalSpy source_spy(&source, &QGeoPositionInfoSource::positionUpdated);
		QVERIFY(source_spy.isValid());
		source.startUpdates();
		QCOMPARE(int(source.error()), int(QGeoPositionInfoSource::NoError));
		QVERIFY(source_spy.wait());
		
		auto last = source.lastKnownPosition(true);
		QVERIFY(last.isValid());
		QCOMPARE(int(last.coordinate().latitude()), 50);
		QCOMPARE(int(last.coordinate().longitude()), 8);
		
		source.stopUpdates();
	}
#endif  // MAPPER_USE_POWERSHELL_POSITION_PLUGIN
	
};


/*
 * We don't need a real GUI window.
 * 
 * But we discovered QTBUG-58768 macOS: Crash when using QPrinter
 * while running with "minimal" platform plugin.
 */
#ifndef Q_OS_MACOS
namespace  {
	auto Q_DECL_UNUSED qpa_selected = qputenv("QT_QPA_PLATFORM", "minimal");  // clazy:exclude=non-pod-global-static
}
#endif


QTEST_MAIN(SensorsTest)
#include "sensors_t.moc"  // IWYU pragma: keep
