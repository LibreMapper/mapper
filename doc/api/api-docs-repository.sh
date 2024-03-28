#!/bin/sh -e
# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright 2015 Kai Pastor (OpenOrienteering)
#
# This file is part of LibreMapper.

# Usage: api-docs-repository.sh REPOSITORY BRANCH PATH

if [ ! -f repository.txt -o "$(cat repository.txt)" != "$1" ]; then
	if [ -d repository ]; then
		rm -Rf repository
	fi
	echo "$1" > repository.txt
fi

if [ ! -d repository ]; then
	git clone --depth 5 "$1" -b "$2" repository
	cd repository
else
	cd repository
	git checkout "$2" "${3:-.}"
	git pull --rebase
fi
