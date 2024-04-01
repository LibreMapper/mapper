/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2012-2017 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_TRANSLATION_UTIL_H
#define LIBREMAPPER_TRANSLATION_UTIL_H

#include <vector>
#include <memory>

#include <QString>
#include <QTranslator>

class QSettings;
// IWYU pragma: no_forward_declare QTranslator

namespace OpenOrienteering {


/**
 * A simplified interface to searching and loading translations.
 */
class TranslationUtil
{
public:
	/**
	 * A struct for representing the language of a translation.
	 * 
	 * This struct is independent of QLocale limitations for languages
	 * like Esperanto.
	 */
	struct Language
	{
		/** 
		 * The code defining the language. \see QLocale::name()
		 */
		QString code;
		
		/**
		 * The display name of the language.
		 */
		QString displayName;
		
		/**
		 * Returns true when the object holds valid data.
		 */
		bool isValid() const { return !code.isEmpty(); }
	};
	
	
	/**
	 * A collection of languages.
	 */
	using LanguageList = std::vector<Language>;
	
	
	/**
	 * Creates a new translation utility for the file or language from the settings.
	 * 
	 * The base name of the translation files must be set in advance.
	 */
	TranslationUtil();
	
	/**
	 * Creates a new translation utility for the file or language from the given settings.
	 * 
	 * The base name of the translation files must be set in advance.
	 */
	TranslationUtil(const QSettings& settings);
	
	/**
	 * Creates a new translation utility for the given language.
	 * 
	 * The base name of the translation files must be set in advance.
	 */
	TranslationUtil(const QString& code, QString translation_file = {});
	
	/**
	 * Returns the code of the language.
	 */
	QString code() const { return language.code; }
	
	/**
	 * Returns the display name of the language.
	 */
	QString displayName() const { return language.displayName; }
	
	/**
	 * Returns a translator for Qt strings.
	 */
	QTranslator& getQtTranslator() { return qt_translator; }
	
	/**
	 * Returns a translator for application strings.
	 */
	QTranslator& getAppTranslator() { return app_translator; }
	
	
	/**
	 * Sets the common base name of the application's translation files.
	 */
	static void setBaseName(const QLatin1String& name);
	
	/**
	 * Returns a collection of available languages for this application.
	 */
	static LanguageList availableLanguages();
	
	/**
	 * Returns the language for a given translation file.
	 */
	static Language languageFromFilename(const QString& path);
	
	/**
	 * Returns the language for a language name.
	 */
	static Language languageFromCode(const QString& code);
	
	/**
	 * Returns the language for the given settings object.
	 */
	static Language languageFromSettings(const QSettings& settings);
	
	
	/**
	 * Tries to create a QTranslator for the named translation.
	 * 
	 * If the named translation cannot be loaded from the search path,
	 * returns an unset unique_prt.
	 */
	std::unique_ptr<QTranslator> load(const QString& translation_name) const;
	
protected:
	/**
	 * Finds a named translation from the search path and loads it
	 * to the translator.
	 */
	bool load(QTranslator& translator, const QString& translation_name) const;
	
	
	static QString base_name;
	
	Language language;
	
	QTranslator qt_translator;
	
	QTranslator app_translator;
};


inline
bool operator<(const TranslationUtil::Language& first, const TranslationUtil::Language& second) {
	return first.code < second.code;
}


}  // namespace OpenOrienteering

#endif
