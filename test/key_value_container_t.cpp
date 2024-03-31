/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <vector>

#include <Qt>
#include <QtTest>
#include <QObject>
#include <QString>

#include "util/key_value_container.h"

using namespace LibreMapper;

#define QSL(str) QStringLiteral(str)

/**
 * @test Tests printing and export features
 */
class KeyValueContainterTest : public QObject
{
Q_OBJECT
	
private slots:
	void keyValueTest()
	{
		QVERIFY((KeyValue{QSL("ak"), QSL("aa")} == KeyValue{QSL("ak"), QSL("aa")}));
		QVERIFY((KeyValue{QSL("BK"), QSL("aa")} != KeyValue{QSL("ak"), QSL("aa")}));
		QVERIFY((KeyValue{QSL("ak"), QSL("BB")} != KeyValue{QSL("ak"), QSL("aa")}));
		QVERIFY((KeyValue{QSL("ak"), QSL("aa")} != KeyValue{QSL("BK"), QSL("aa")}));
		QVERIFY((KeyValue{QSL("ak"), QSL("aa")} != KeyValue{QSL("ak"), QSL("BB")}));
	}
	
	void emptyContainerTest()
	{
		auto container = KeyValueContainer();
		QVERIFY(container.empty());
		QCOMPARE(int(container.size()), 0);
	}
	
	void crudTest_data()
	{
		QTest::addColumn<QString>("key");
		QTest::addColumn<QString>("value");
		QTest::newRow("null key")  << QString{} << QSL("null");
		QTest::newRow("empty key") << QSL("")   << QSL("empty");  // clazy:exclude=empty-qstringliteral
		QTest::newRow("bk")        << QSL("bk") << QSL("bb");
	}
	
	void crudTest()
	{
		QFETCH(QString, key);
		QFETCH(QString, value);
		
		auto container = KeyValueContainer{};
		QVERIFY(container.find(key) == end(container));
		QVERIFY(!container.contains(key));
		QVERIFY_EXCEPTION_THROWN(container.at(key), std::out_of_range);
		
		// operator[]
		QVERIFY(container[key].isEmpty());
		QVERIFY(container.find(key) != container.end());
		QVERIFY(container.contains(key));
		QVERIFY(container.at(key).isEmpty());
		
		container[key] = value;
		QVERIFY(container.find(key) != container.end());
		QCOMPARE(*container.find(key), (KeyValue{key, value}));
		QVERIFY(container.contains(key));
		QCOMPARE(container.at(key), value);
		QCOMPARE(container[key], value);
		
		// contains()
		QVERIFY(container.contains(key));
		auto const key_uc = key.toUpper();
		if (key.compare(key_uc, Qt::CaseSensitive) != 0)
			QVERIFY(!container.contains(key_uc));
		
		// erase()
		container.erase(container.find(key));
		QVERIFY(container.find(key) == container.end());
		QVERIFY(!container.contains(key));
		QVERIFY_EXCEPTION_THROWN(container.at(key), std::out_of_range);
		
		// Order-preserving
		auto kv_a = KeyValue{QSL("ak"), QSL("aa")};
		auto kv_c = KeyValue{QSL("ck"), QSL("cc")};
		
		QVERIFY(container.empty());
		auto const result_a = container.insert(kv_a);
		QCOMPARE(*result_a.first, kv_a);
		QCOMPARE(result_a.second, true);
		auto const result_key = container.insert_or_assign(key, value);
		QCOMPARE(result_key.first->key, key);
		QCOMPARE(result_key.first->value, value);
		QCOMPARE(result_key.second, true);
		container.insert(kv_c);
		QCOMPARE(container.front(), kv_a);
		QCOMPARE(container.at(key), value);
		QCOMPARE(container.back(), kv_c);
		
		auto container2 = KeyValueContainer{};
		QVERIFY(container2.empty());
		container2.insert(kv_c);
		container2.insert(kv_a);
		container2.insert_or_assign(container2.begin(), key, value);
		QCOMPARE(container2.front(), (KeyValue{key, value}));
		QCOMPARE(container2.at(kv_c.key), kv_c.value);
		QCOMPARE(container2.back(), kv_a);
		
		// Comparing containers
		QVERIFY(container2 != container);
		QVERIFY(std::is_permutation(begin(container2), end(container2), begin(container), end(container)));
		
		// insert() and insert_or_assign(), existing vs. new key
		auto const old_size = container.size();
		
		auto const result_insert = container.insert({key, QSL("new value")});
		QCOMPARE(result_insert.second, false);
		QCOMPARE(container[key], value);
		QCOMPARE(container.size(), old_size);
		
		auto const result_insert_or_assign = container.insert_or_assign(key, QSL("new value"));
		QCOMPARE(result_insert_or_assign.second, false);
		QCOMPARE(container[key], QSL("new value"));
		QCOMPARE(container.size(), old_size);
		
		container[key] = value;
		QCOMPARE(container.at(key), value);
		QCOMPARE(container.size(), old_size);
		
		auto const result_insert_2 = container.insert({QSL("insert_2"), QSL("new value")});
		QCOMPARE(result_insert_2.second, true);
		QCOMPARE(container[result_insert_2.first->key], QSL("new value"));
		QCOMPARE(container.size(), old_size + 1);
		
		auto const result_insert_or_assign_2 = container.insert_or_assign(QSL("insert_or_assign_2"), QSL("new value"));
		QCOMPARE(result_insert_or_assign_2.second, true);
		QCOMPARE(container[result_insert_or_assign_2.first->key], QSL("new value"));
		QCOMPARE(container.size(), old_size + 2);
	}
	
};



QTEST_GUILESS_MAIN(KeyValueContainterTest)
#include "key_value_container_t.moc"  // IWYU pragma: keep
