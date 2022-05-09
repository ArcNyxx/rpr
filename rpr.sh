#!/bin/sh
# rpr - rich presence daemon
# Copyright (C) 2022 ArcNyxx
# see LICENCE file for licensing information

rpserv & PROC=$!
trap 'kill "$PROC"' EXIT; sleep 1 # wait for server ready

PAST='new'
unset STATE DETAILS IMAGE ITEXT
while true; do
	[ "$STATE" != "$PAST" ] && rpclnt "$STATE" "$DETAILS" "$IMAGE" "$ITEXT"
	PAST="$STATE"
	sleep 4 # rate limiting

	# programming in git repository with github remote
	STATE="$(ps -C "$EDITOR" -o 'pid' | sed '1d' | while read -r LINE; do
		REMOTE="$(cat "/proc/${LINE##\ }/cwd/.git/FETCH_HEAD" \
				2>/dev/null)"
		[ $? -ne 0 ] && continue # file not exists

		REMOTE="${REMOTE##* }"
		[ "${REMOTE#https://}" = "$REMOTE" ] && \
			REMOTE="https://$(echo "${REMOTE#*@}" | tr ':' '/')"
		[ "${REMOTE##https://github.com/}" = "$REMOTE" ] &&
			continue # remote not github.com

		echo "Working on \`${REMOTE##*/}\` at $REMOTE"
		break
	done)"
	[ "$STATE" ] && continue
done
