#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright 2016 Kai Pastor (OpenOrienteering)
#
# This file is part of LibreMapper.
 
set -e

for FILE
do
	echo -n "$FILE" | sed -e 's/.*OpenOrienteering_/Comment[/;s/\.ts$/]=/'
	cat ${FILE} | sed -n -e '
	  /<source>A free software for drawing orienteering maps</! d
	  n
	  /<translation>/! n
	  s/[^>]*>//
	  s/<.*//
	  p
	  q
	'
done | sed -e "
  s/^[^=]*\]=\(Comment\)/\1/
  /^[^=]*\]=$/ d
  s/&quot;/\"/
  s/&apos;/'/
" > desktop_file_comment.txt

sed -e '
  /^Comment=/ r desktop_file_comment.txt
  /^Comment\[/ d
' -i -- "../packaging/linux/Mapper.desktop" \
&& rm -f "../packaging/linux/Mapper.desktop--"

for FILE
do
	echo -n "$FILE" | sed -e 's/.*OpenOrienteering_/     <comment xml:lang="/;s/\.ts$/">/'
	cat ${FILE} | sed -n -e '
	  /<source>Orienteering map</! d
	  n
	  /<translation>/! n
	  s/[^>]*>//
	  s/<.*//
	  p
	  q
	'
done | sed -e "
  s/^[^>]*>\( *<comment\)/\1/
  /\">$/ d
  s/$/<\/comment>/
  s/&quot;/\"/
  s/&apos;/'/
" > mime_type_comment.txt

sed -e '
  /^ *<comment>/ r mime_type_comment.txt
  /^ *<comment [^>]*lang=/ d
' -i -- "../packaging/linux/openorienteering-mapper.xml" \
&& rm -f "../packaging/linux/openorienteering-mapper.xml--"

