/* rpr - rich presence daemon
 * Copyright (C) 2022 ArcNyxx
 * see LICENCE file for licensing information */

#include <sys/socket.h>
#include <sys/sysinfo.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#include <dsdk.h>

#include "util.h"

union sockr {
	struct sockaddr sa;
	struct sockaddr_un un;
};

int
main(int argc, char **argv)
{
	struct DiscordActivity act = {
		.details = "Working on nothing in particular...",
		.assets.large_image = "linux",
		.assets.large_text = "Linux"
	};

	char *ptrs[] = { act.details, act.state,
			act.assets.small_image, act.assets.small_text };
	for (int i = 1; i < argc; ++i)
		if (argv[i][0] != '\0')
			strncpy(ptrs[i - 1], argv[i], 127);

	if (argv[1] == NULL || argv[1][0] == '\0') {
		struct sysinfo info;
		if (sysinfo(&info) == -1)
			die("rpclnt: unable to get sysinfo: ");
		act.timestamps.start = time(NULL) - info.uptime;
	} else {
		act.timestamps.start = time(NULL);
	}

	int sock;
	if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
		die("rpclnt: unable to open socket: ");

	union sockr addr = { .un.sun_family = AF_UNIX, .un.sun_path = SOCKP };
	if (connect(sock, &addr.sa, sizeof(short) + sizeof(SOCKP)) == -1)
		die("rpclnt: unable to connect socket to address: ");
	if (write(sock, &act, sizeof(struct DiscordActivity)) !=
			sizeof(struct DiscordActivity))
		die("rpclnt: unable to write activity: ");
}
