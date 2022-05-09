#!/bin/sh
# rpr - rich presence
# Copyright (C) 2022 ArcNyxx
# see LICENCE file for licensing information

setp & PROC=$!
trap 'kill "$PROC"; exit' INT

unset PSTATE
while true; do
	sleep 5

	# programming in git repository with github remote
	ps -C "$EDITOR" -o 'pid' | sed '1d' | while read -r LINE; do
		REMOTE="$(cat "/proc/${LINE##\ }/cwd/.git/FETCH_HEAD" \
				2>/dev/null)"
		[ $? -ne 0 ] && continue # file not exists

		echo 'awfjweoa'

		REMOTE="${REMOTE##* }"
		[ "${REMOTE#https://}" = "$REMOTE" ] && \
			REMOTE="https://$(echo "${REMOTE#*@}" | tr ':' '/')"
		[ "${REMOTE##https://github.com/}" = "$REMOTE" ] &&
			continue # remote not github.com

		echo 'awfjweoa'

		STATE="Working on \`${REMOTE##*/}\` at $REMOTE"
		DETAILS=""

		echo "$STATE"

		[ "$STATE" = "$PSTATE" ] && continue 2
		PSTATE="$STATE"

		echo "$STATE"

		echo "$STATE $DETAILS" >"/proc/$PROC/fd/0"
		kill -s USR1 "$PROC"
	done
done
