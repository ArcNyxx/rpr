/* rpr - rich presence
 * Copyright (C) 2022 ArcNyxx
 * see LICENCE file for licensing information */

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <dsdk.h>

#define SOCKP "/tmp/rpr"

union sockr {
	struct sockaddr    sa;
	struct sockaddr_un un;
};

int
main(int argc, char **argv)
{
	struct DiscordActivity act = { .assets.large_image = "linux",
			.assets.large_text = "Linux" };

	char *ptrs[] = { act.details, act.state,
			act.assets.small_image, act.assets.small_text };
	for (int i = 1; i < argc; ++i)
		strncpy(ptrs[i - 1], argv[i], 127);

	int sock;
	if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
		return 1;

	union sockr addr = { .un.sun_family = AF_UNIX, .un.sun_path = SOCKP };
	if (connect(sock, &addr.sa, sizeof(struct sockaddr)) == -1)
		return 1;
	if (write(sock, &act, sizeof(struct DiscordActivity)) !=
			sizeof(struct DiscordActivity))
		return 1;
}
