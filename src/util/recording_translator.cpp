/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2013 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */


#include "recording_translator.h"

#include <QtGlobal>
#include <QDebug>


namespace LibreMapper {

RecordingTranslator::RecordingTranslator(QObject* parent)
 : QTranslator(parent)
{
	; // nothing
}

RecordingTranslator::~RecordingTranslator()
{
	; // nothing
}

bool RecordingTranslator::isEmpty() const
{
	return false;
}

QString RecordingTranslator::translate(const char* context, const char* sourceText, const char* disambiguation, int n) const
{
	qDebug() << context << " " << sourceText << " " << disambiguation << " " << n;
	static const QString null_string;
	return null_string;
}


}  // namespace LibreMapper
