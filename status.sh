#!/bin/sh
# status - determine user task
# Copyright (C) 2022 ArcNyxx
# see LICENCE file for licensing information

# check for programming in git repository
ps -C "$EDITOR" -o 'pid' | sed '1d' | while read -r LINE; do
	LINE="${LINE##\ }"
	[ ! -d "/proc/$LINE/cwd/.git/" ] && continue

	REMOTE="$(cat "/proc/$LINE/cwd/.git/FETCH_HEAD")"
	REMOTE="${REMOTE##* }"

	# convert ssh address to http
	[ "${REMOTE#http}" = "$REMOTE" ] && \
		REMOTE="http://$(echo "${REMOTE#*@}" | tr ':' '/')"

	echo "Working on \`${REMOTE##*/}\` at $REMOTE"

	exit
done
