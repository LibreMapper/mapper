#!/bin/sh -e
# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright 2015 Kai Pastor (OpenOrienteering)
#
# This file is part of LibreMapper.

# Usage: api-docs-commit.sh PATH [PROJECT_BINARY_DIR]

for I in "repository/${1:-.}"/*; do
	if [ -e "$I" -a "${I##*/}" != "README.md" ]; then
		rm -R "$I"
	fi
done

for I in html/*; do
	if [ "${I##*.}" != "md5" -a "${I##*.}" != "map" ]; then
		cp -R "$I" "repository/${1:-.}/"
	fi
done

cd repository
git add -A "${1:-.}"
git commit -m "Update${1:+ in $1}"

echo "Now call 'cd \"${2:-[CURRENT_BINARY_DIR]}/repository\" && git commit' to publish the updated API docs."
