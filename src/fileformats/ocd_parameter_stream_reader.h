/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2022 Matthias Kühlewein (OpenOrienteering)
 * Copyright 2022 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#ifndef LIBREMAPPER_OCD_PARAMETER_STREAM_READER_H
#define LIBREMAPPER_OCD_PARAMETER_STREAM_READER_H

#include <QString>
#include <QStringRef>

namespace OpenOrienteering {


/**
 * A class for processing OCD parameter strings,
 * loosely modeled after QXmlStreamReader.
 */
class OcdParameterStreamReader
{
public:
	/**
	 * Constructs a new reader object.
	 */
	explicit OcdParameterStreamReader(const QString& param_string) noexcept;
	
	/**
	 * Reads the input until the next key-value pair.
	 * 
	 * @return True if another key-value pair was reached. False for end of input.
	 */
	bool readNext();
	
	/**
	 * Returns the current key.
	 */
	char key() const { return current_key; }
	
	/**
	 * Returns the current value.
	 */
	QStringRef value() const;
	
	/**
	 * Returns true if there is no more data.
	 */
	bool atEnd() const { return next == -1; }
	
	/**
	 * The value returned by `key()` for the first value, or at the end of input.
	 */
	static constexpr char noKey() { return 0; }
	
private:
	QString param_string;
	int pos;  ///< The begin of the current value substring, or param_string.length().
	int next; ///< The position after the end of the current value substring, or -1 if reaching the end of input.
	char current_key;
};

}  // namespace Openorienteering

#endif // LIBREMAPPER_OCD_PARAMETER_STREAM_READER
