#!/bin/bash
set -e

if [[ $CIRCLE_TAG =~ ^v[0-9].* ]]; then
	RVERSION="${CIRCLE_TAG:1}"
else
	RVERSION="0.0.${CIRCLE_BUILD_NUM:=1}"
fi

#RVERSION="$(${CIRCLE_TAG:=0.0.${CIRCLE_BUILD_NUM:-1}} | sed 's/[^0-9\.]*//g')"
RDATE="$(date -R)"

sed -e 's/$RELEASE/'"$RVERSION"'/' -e 's/$DATE/'"$RDATE"'/' debian/changelog.template > debian/changelog
