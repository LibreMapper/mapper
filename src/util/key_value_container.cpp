/* SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright 2020 Kai Pastor (OpenOrienteering)
 *
 * This file is part of LibreMapper.
 */

#include "key_value_container.h"

#include <algorithm>
#include <stdexcept>


namespace LibreMapper {

bool operator==(const KeyValue& lhs, const KeyValue& rhs)
{
	return lhs.key == rhs.key && lhs.value == rhs.value;
}


const KeyValueContainer::mapped_type& KeyValueContainer::at(const key_type& key) const
{
	auto hint = find(key);
	if (hint == end())
		throw std::out_of_range("Invalid key");
	return hint->value;
}

KeyValueContainer::mapped_type& KeyValueContainer::at(const key_type& key)
{
	return const_cast<mapped_type&>(static_cast<const KeyValueContainer*>(this)->at(key));
}

KeyValueContainer::mapped_type& KeyValueContainer::operator[](const key_type& key)
{
	auto hint = find(key);
	if (hint == end())
		hint = std::vector<KeyValue>::insert(hint, {key, {}});
	return hint->value;
}


KeyValueContainer::const_iterator KeyValueContainer::find(const key_type& key) const
{
	return std::find_if(begin(), end(), [key](auto const& tag) { return tag.key == key; });
}

KeyValueContainer::iterator KeyValueContainer::find(const key_type& key)
{
	return std::find_if(begin(), end(), [key](auto const& tag) { return tag.key == key; });
}

bool KeyValueContainer::contains(const key_type& key) const
{
	return find(key) != cend();
}


std::pair<KeyValueContainer::iterator, bool> KeyValueContainer::insert(const value_type& value)
{
	auto hint = find(value.key);
	auto const at_end = hint == end();
	if (at_end)
		hint = std::vector<KeyValue>::insert(hint, value);
	return {hint, at_end};
}

std::pair<KeyValueContainer::iterator, bool> KeyValueContainer::insert_or_assign(const key_type& key, const mapped_type& object)
{
	auto hint = find(key);
	auto const at_end = hint == end();
	if (at_end)
		hint = std::vector<KeyValue>::insert(hint, {key, object});
	else
		hint->value = object;
	return {hint, at_end};
}

KeyValueContainer::iterator KeyValueContainer::insert_or_assign(iterator hint, const key_type& key, const mapped_type& object)
{
	auto const at_end = hint == end();
	if (at_end || hint->key != key)
		hint = std::vector<KeyValue>::insert(hint, {key, object});
	else
		hint->value = object;
	return hint;
}


}  // namespace LibreMapper
