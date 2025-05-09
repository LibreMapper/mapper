/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2022 Matthias Kühlewein (OpenOrienteering)
 * Copyright 2022 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "ocd_parameter_stream_reader.h"

#include <algorithm>

#include <QtGlobal>
#include <QChar>
#include <QLatin1Char>


namespace LibreMapper {

OcdParameterStreamReader::OcdParameterStreamReader(const QString& param_string) noexcept
 : param_string(param_string)
 , pos(0)
 , next(param_string.indexOf(QLatin1Char('\t')))
 , current_key(noKey())
{}

bool OcdParameterStreamReader::readNext()
{
	while (!atEnd())
	{
		pos = next + 1;
		next = param_string.indexOf(QLatin1Char('\t'), pos);
		if (pos < param_string.length())
		{
			current_key = param_string.at(pos).toLatin1();
			if (Q_LIKELY(current_key != '\t'))
			{
				++pos;
				return true;
			}
		}
	}
	
	current_key = noKey();
	pos = param_string.length();
	return false;
}

QStringView OcdParameterStreamReader::value() const
{
	return QStringView{param_string}.mid(pos, std::max(-1, next - pos));
}


}  // namespace LibreMapper
