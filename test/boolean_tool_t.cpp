/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2026 Libor Pecháček
 *
 * This file is part of LibreMapper.
 */

#include <algorithm>
#include <cstddef>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

#include <QCoreApplication>
#include <QByteArrayView>
#include <QFile>
#include <QIODevice>
#include <QLatin1StringView>
#include <QObject>
#include <QString>
#include <QtEnvironmentVariables>
#include <QtGlobal>
#include <QtResource>
#include <QtTest>

#include "core/map_coord.h"
#include "core/map.h"
#include "core/map_part.h"
#include "core/objects/boolean_tool.h"
#include "core/objects/object.h"
#include "global.h"
#include "undo/object_undo.h"

using namespace LibreMapper;
using namespace Qt::StringLiterals;

/**
 * @test Tests the boolean tool and the underlying Clipper2
 */

namespace literal {
	// This code does not yet use Qt6 literals, hence the explicit use of
	// QLatinStringView and QStringLiteral
	QLatin1StringView constexpr operations {"operations"};
	QLatin1StringView constexpr case_id {"case_id"};
	QLatin1StringView constexpr description {"description"};
	QLatin1StringView constexpr vertical_displacements {"vertical_displacements"};
	QLatin1StringView constexpr Union {"Union"};
	QLatin1StringView constexpr Intersection {"Intersection"};
	QLatin1StringView constexpr Difference {"Difference"};
	QLatin1StringView constexpr XOr {"XOr"};
	QLatin1StringView constexpr MergeHoles {"MergeHoles"};
}  // namespace literal


namespace {

// Empty operation name means a wildcard to enable selection of source objects
void perform_ordered_selection(Map* map, MapPart* source_part,
                               QString operation_name, int case_number)
{
	// Search the source part for test objects and reorder them.
	auto const case_id = QString::number(case_number);
	auto src_obj_sel = [operation_name, case_id](auto* object)
	{
		auto const obj_operation = object->getTag(literal::operations);
		auto const obj_case_id = object->getTag(literal::case_id);
		return (operation_name.isEmpty() || obj_operation == operation_name)
		        && case_id == obj_case_id;
	};

	source_part->applyOnMatchingObjects([map](Object* object) {
		map->addObjectToSelection(object, false);
	}, src_obj_sel);

	auto source_selection = map->selectedObjects();
	std::vector<std::tuple<Object*, int>> source_with_ordering;
	for (auto* obj : source_selection)
	{
		bool ok = false;
		auto order = obj->getTag(QStringLiteral("order")).toInt(&ok);
		QVERIFY2(ok, "Failed to interpret map object's \"order\" tag. Objects must be assigned a numeric value.");
		source_with_ordering.emplace_back(std::make_tuple(obj, order));
	}
	std::sort(source_with_ordering.begin(), source_with_ordering.end(),
	          [](auto& a, auto& b) { return std::get<int>(a) > std::get<int>(b); });
	map->clearObjectSelection(false);

	// Re-add objects into the selection in the right order, so that we are
	// sure which one is the subject of the operation.
	for (auto& obj_rec : source_with_ordering)
		map->addObjectToSelection(std::get<Object*>(obj_rec), false);
}

// Find pointers to source and results parts
std::tuple<MapPart*, MapPart*> find_map_parts(const Map& map)
{
	MapPart* source_part = {}; // NOLINT(misc-const-correctness)
	MapPart* results_part = {}; // NOLINT(misc-const-correctness)

	for (auto idx = 0; idx < map.getNumParts(); ++idx)
	{
		auto const part = map.getPart(idx);
		auto const part_name = part->getName();
		if (part_name == QStringLiteral("source"))
			source_part = part;
		else if (part_name == QStringLiteral("results"))
			results_part = part;
	}

	return {source_part, results_part};
}

}  // anonymous namespace


class BooleanToolTest : public QObject
{
Q_OBJECT

private:
	std::unique_ptr<Map> map;

	std::map<QString, BooleanTool::Operation> op_map = {
	    {literal::Union, BooleanTool::Union},
	    {literal::Intersection, BooleanTool::Intersection},
	    {literal::Difference, BooleanTool::Difference},
	    {literal::XOr, BooleanTool::XOr},
	    {literal::MergeHoles, BooleanTool::MergeHoles},
	};

private slots: // NOLINT(readability-redundant-access-specifiers)
	void initTestCase()
	{
		Q_INIT_RESOURCE(resources);
		doStaticInitializations();

		map = std::make_unique<Map>();
		QVERIFY(bool(map));
		auto const test_map_file = QFINDTESTDATA("data/boolean_tool.xmap");
		QVERIFY(map->loadFrom(test_map_file));
	}


	void testBooleanTool_data()
	{
		QTest::addColumn<BooleanTool::Operation>("tool_operation");
		QTest::addColumn<QString>("operation_name");
		QTest::addColumn<int>("case_id");
		QTest::addColumn<QString>("case_description");
		QTest::addColumn<int>("vertical_displacement");

		auto [source_part, results_part] = find_map_parts(*map);
		QVERIFY2(source_part && results_part, "The test map must have both \"source\" and \"results\" parts defined.");

		// Collect case descriptions from the source objects
		source_part->applyOnAllObjects([this](Object* object) {
			auto const cur_ops = object->getTag(literal::operations);
			if (cur_ops.isEmpty())
				return;
			auto const cur_case_id_str = object->getTag(literal::case_id);
			auto const cur_descr = object->getTag(literal::description);
			auto const cur_displacement = object->getTag(literal::vertical_displacements);

			bool ok = false;
			auto const cur_case_id = cur_case_id_str.toInt(&ok);
			if (!ok)
			{
				qWarning("Failed to interpret map object's \"case_id\" tag (%s). Case id must be a numeric value. Skipping this one.",
				         qPrintable(cur_case_id_str));
				return;
			}

			auto const ops = cur_ops.split(QLatin1Char(','));
			auto const displacements = cur_displacement.split(QLatin1Char(','));

			if (ops.size() > displacements.size())
				qWarning("Displacements list should be at least as long as the operations list.");

			for (auto i = 0; i < ops.size(); ++i)
			{
				auto const row_descr = QLatin1StringView {"%1 %2, %3"}
				                       .arg(ops[i], QString::number(cur_case_id), cur_descr);

				try
				{
					auto const tool_op = op_map.at(ops[i]);
					QTest::newRow(qPrintable(row_descr)) << tool_op << ops[i] << cur_case_id << cur_descr << displacements[i].toInt();
				}
				catch(std::out_of_range& e)
				{
					qWarning("Invalid operation name \"%s\", skipping test.", qPrintable(ops[i]));
				}
			}
		});
	}


	void testBooleanTool() // NOLINT(readability-function-cognitive-complexity)
	{
		QFETCH(BooleanTool::Operation, tool_operation); // NOLINT(misc-const-correctness)
		QFETCH(QString, operation_name); // NOLINT(misc-const-correctness)
		QFETCH(int, case_id); // NOLINT(misc-const-correctness)
		QFETCH(QString, case_description); // NOLINT(misc-const-correctness)
		QFETCH(int, vertical_displacement); // NOLINT(misc-const-correctness)

		// Make a copy of the map
		Map copy_map;
		copy_map.setScaleDenominator(map->getScaleDenominator());
		copy_map.importMap(*map, Map::ObjectImport);

		auto [source_part, results_part] = find_map_parts(copy_map);

		copy_map.setCurrentPart(source_part);
		perform_ordered_selection(&copy_map, source_part, {}, case_id);

		if (tool_operation == BooleanTool::Union)
			QVERIFY(BooleanTool(tool_operation, &copy_map).executePerSymbol());
		else
			QVERIFY(BooleanTool(tool_operation, &copy_map).execute());

		auto result_selection = copy_map.selectedObjects();

		// Select example result objects for comparison
		copy_map.clearObjectSelection(false);
		perform_ordered_selection(&copy_map, results_part, operation_name, case_id);
		auto sample_selection = copy_map.selectedObjects();
		copy_map.clearObjectSelection(false);

		// Boolean tool creates an undo step, so we follow with the object
		// tagging and move
		if (result_selection.size())
		{
			auto* undo_step = new ReplaceObjectsUndoStep(&copy_map);
			Map::ObjectSelection reworked_selection;
			for (Object* modified_obj : result_selection)
			{
				auto* orig_obj = modified_obj->duplicate();
				orig_obj->setMap(&copy_map);
				modified_obj->setTag(literal::operations, operation_name);
				modified_obj->setTag(literal::case_id, QString::number(case_id));
				if (vertical_displacement)
					modified_obj->move({0, vertical_displacement});
				undo_step->addObject(modified_obj, orig_obj);
				reworked_selection.insert(modified_obj);
			}

			copy_map.push(undo_step);
			copy_map.setObjectsDirty();
			result_selection = reworked_selection;
		}

#ifdef MAPPER_DEVELOPMENT_BUILD
		auto const out_dir = QCoreApplication::applicationDirPath();
		auto const out_filename = QLatin1StringView {"%1boolean_tool_t-%2%3.xmap"}
		                          .arg(out_dir, operation_name, QString::number(case_id));
		QFile file(out_filename);
		QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Truncate));
		QVERIFY(copy_map.exportToIODevice(file));
		auto const error_message = QLatin1StringView {"There is %1 for further analysis."}
		                           .arg(out_filename);
#else
		auto const error_message = QLatin1StringView {"Build Mapper in debug mode to get a file for further analysis."};
#endif

		QVERIFY2(result_selection.size() == sample_selection.size(), qPrintable(error_message));

		// We are using custom comparator checking only the coordinates, rather
		// than Object:equals() that tests other properties like tags
		auto obj_comparator = [](auto const* a_ptr, auto const* b_ptr) {
			if (a_ptr->getType() != Object::Path || b_ptr->getType() != Object::Path)
				return false;
			auto const* a = a_ptr->asPath();
			auto const* b = b_ptr->asPath();
			auto const count = a->getCoordinateCount();
			if (count != b->getCoordinateCount())
				return false;
			for (std::size_t i = 0; i < count; ++i)
				if (a->getCoordinate(i) != b->getCoordinate(i))
					return false;
			return true;
		};

		QVERIFY2(std::is_permutation(result_selection.cbegin(), result_selection.cend(),
		                            sample_selection.cbegin(), sample_selection.cend(),
		                            obj_comparator),
		         qPrintable(error_message));

#ifdef MAPPER_DEVELOPMENT_BUILD
		QVERIFY(QFile::remove(out_filename));
#endif
	}
};



/*
 * We don't need a real GUI window.
 *
 * But we discovered QTBUG-58768 macOS: Crash when using QPrinter
 * while running with "minimal" platform plugin.
 */
#ifndef Q_OS_MACOS
namespace  {
	Q_DECL_UNUSED auto const qpa_selected = qputenv("QT_QPA_PLATFORM", "minimal");  // clazy:exclude=non-pod-global-static
}
#endif


QTEST_MAIN(BooleanToolTest)

#include "boolean_tool_t.moc"  // IWYU pragma: keep
