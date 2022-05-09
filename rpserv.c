/* rpr - rich presence
 * Copyright (C) 2022 ArcNyxx
 * see LICENCE file for licensing information */

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#include <dsdk.h>

#define SOCKP "/tmp/rpr"

union sockr {
	struct sockaddr    sa;
	struct sockaddr_un un;
};

int
main(void)
{
	char *client;
	if ((client = getenv("DISCLIENT")) == NULL)
		return 1;

	struct IDiscordCore *core;
	struct DiscordCreateParams params = {
		.client_id = strtoul(client, NULL, 10),
		.flags = DiscordCreateFlags_NoRequireDiscord
	};
	if (DiscordCreate(DISCORD_VERSION, &params, &core) != DiscordResult_Ok)
		return 1;

	int sock, flags;
	if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
		return 1;
	if ((flags = fcntl(sock, F_GETFL, 0)) < 0)
		return 1;
	if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1)
		return 1;
	if (remove(SOCKP) == -1 && errno != ENOENT)
		return 1;

	union sockr addr = { .un.sun_family = AF_UNIX, .un.sun_path = SOCKP };
	socklen_t slen = sizeof(struct sockaddr);
	if (bind(sock, &addr.sa, slen) == -1)
		return 1;
	if (listen(sock, SOMAXCONN) == -1)
		return 1;

	int fd = -1;
	struct DiscordActivity act;
	struct IDiscordActivityManager *man = core->get_activity_manager(core);
	for (;;) {
		if (core->run_callbacks(core) != DiscordResult_Ok)
			return 1;

#define SCREAM(hey) { printf(hey); return 1; }

		if (fd == -1 && (fd = accept(sock, &addr.sa, &slen)) == -1) {
			if (errno == EAGAIN && errno == EWOULDBLOCK)
				continue;
			else
				SCREAM("joawei")
		}

		struct pollfd pollfd = { .fd = fd,
				.events = POLLRDNORM | POLLHUP };
		if (poll(&pollfd, 1, 0) != 1)
			continue;

		if (pollfd.revents & POLLRDNORM) {
			if (read(fd, &act, sizeof(struct DiscordActivity)) !=
					sizeof(struct DiscordActivity)) {
				if (errno == EAGAIN)
					goto out;
				else {
					perror("helo");
					return 1;
				}
			}
			man->update_activity(man, &act, NULL, NULL);
		}

out:
		if (pollfd.revents & POLLHUP) {
			close(fd);
			fd = -1;
		}
	}
}
