#!/bin/sh
# sdrp - rich presence
# Copyright (C) 2022 ArcNyxx
# see LICENCE file for licensing information

rm -rf /tmp/dsdk.zip /tmp/dsdk

curl -s 'https://dl-game-sdk.discordapp.net/2.5.6/discord_game_sdk.zip' \
	--output /tmp/dsdk.zip && unzip -q /tmp/dsdk.zip -d /tmp/dsdk
[ $? -ne 0 ] && exit 1

cp /tmp/dsdk/lib/x86_64/discord_game_sdk.so /usr/local/lib/libdsdk.so
cp /tmp/dsdk/c/discord_game_sdk.h /usr/local/include/dsdk.h
