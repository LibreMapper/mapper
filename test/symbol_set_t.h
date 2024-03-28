/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2014-2019 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef OPENORIENTEERING_SYMBOL_SET_T_H
#define OPENORIENTEERING_SYMBOL_SET_T_H

#include <vector>

#include <QDir>
#include <QObject>
#include <QString>

class QXmlStreamWriter;

/**
 * @test Updates compressed and scaled symbol sets, examples and test files,
 *       and update map symbol translation files.
 * 
 * This tool overwrites the .omap symbol set files in the symbol set directory.
 * This is not only a test uncovering changes in the standard file format but
 * also helps to maintain the released compact symbol sets from a minimum set
 * of verbose source symbol sets.
 * 
 * This tool also creates compressed example files out of original .xmap files.
 * 
 * In addition it creates and updates map symbol translation files for all
 * symbol names and descriptions in the symbol set files. The symbol set file
 * name is used to determine language and translation "context". The translation
 * "comment" field is used to identify strings independent of symbol name and
 * description.
 * 
 * The target files remain untouched if there is no change.
 */
class SymbolSetTool : public QObject
{
Q_OBJECT

public:	
	struct TranslationEntry
	{
		QString context;
		QString source;
		QString comment;
		QString translation = {};
		QString type = {};
		bool obsolete = true;
		
		void write(QXmlStreamWriter& xml) const;
	};
	
private slots:
	void initTestCase();
	void processSymbolSet_data();
	void processSymbolSet();
	void processSymbolSetTranslations_data();
	void processSymbolSetTranslations() const;
	void processExamples_data();
	void processExamples();
	void processTestData_data();
	void processTestData();
	
private:
	std::vector<TranslationEntry> translation_entries;
	bool translations_complete;
	
	QDir symbol_set_dir;
	QDir examples_dir;
	QDir test_data_dir;
	QDir translations_dir;
	
};

#endif // OPENORIENTEERING_SYMBOL_SET_T_H
