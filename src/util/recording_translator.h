/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2013 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#ifndef LIBREMAPPER_UTIL_RECORDING_TRANSLATOR_H
#define LIBREMAPPER_UTIL_RECORDING_TRANSLATOR_H

#include <QObject>
#include <QString>
#include <QTranslator>

namespace OpenOrienteering {


/** A QTranslator variation that doesn't actually translate,
 *  but instead dumps (TODO: records) the strings to be translated.
 * 
 *  Note: This is for Qt5 only.
 */
class RecordingTranslator : public QTranslator
{
Q_OBJECT
public:
	/** Constructs a new translator. */
	explicit RecordingTranslator(QObject* parent = 0);
	
	/** Destructs the translator. */
	~RecordingTranslator() override;
	
	/** Returns false. */
	bool isEmpty() const override;
	
	/** Dumps the context, sourceText, disambiguation and n using qDebug().
	 *  Return a null string. 
	 *
	 *  Note: This is the Qt5 signature of translate(). */
	QString	translate(const char* context, const char* sourceText, const char* disambiguation = 0, int n = -1) const override;
};


}  // namespace OpenOrienteering

#endif
