#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright 2019 Kai Pastor (OpenOrienteering)
#
# This file is part of LibreMapper.

if [ -z "$1" -o -z "$2" -o -z "$3" ] ; then
	echo "Usage: $0 SOURCE_ID INTERMEDIATE_ID TARGET_ID"
    exit 1
fi

FIRST_FILE="$1-$2.crt"
if [ ! -f "$FIRST_FILE" ]; then
	echo "No such file: $FIRST_FILE"
    exit 2
fi

SECOND_FILE="$2-$3.crt"
if [ ! -f "$SECOND_FILE" ]; then
	echo "No such file: $SECOND_FILE"
    exit 3
fi

{
	cat "$SECOND_FILE" |
	  sed -e '/^[0-9]/!d;s,$,   ,;s,^\([0-9.].....\) *\([0-9].....\),s/^\2 /!\1 /,'
	echo '/^[0-9]/d'
	echo 's,^!,,'
} | tee tmp.sed | tail
echo "---"

OUTPUT_FILE="$1-$3.crt"
{
	cat "$FIRST_FILE" |
	  sed -f tmp.sed
} | tee "$OUTPUT_FILE" | tail
echo "---"

diff -uw "$FIRST_FILE" "$OUTPUT_FILE" | tail
rm tmp.sed

