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

#include "util.h"

union sockr {
	struct sockaddr sa;
	struct sockaddr_un un;
};

static int read_act(int *fd, struct DiscordActivity *act, size_t *len);

static int
read_act(int *fd, struct DiscordActivity *act, size_t *len)
{
	struct pollfd pollfd = { .fd = *fd, .events = POLLRDNORM | POLLHUP };
	if (poll(&pollfd, 1, 500) != 1)
		return 0;

	ssize_t num = 0;
	if (pollfd.revents & POLLRDNORM) {
		if ((num = read(*fd, act, sizeof(struct DiscordActivity) -
				*len )) == sizeof(struct DiscordActivity))
			*len += num;
		else if (num == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
			die("rpserv: unable to read socket: ");
	}

	if (pollfd.revents & POLLHUP)
		close(*fd), *fd = -1;
	return num != 0;
}

int
main(void)
{
	char *client;
	if ((client = getenv("DISCLIENT")) == NULL)
		die("rpserv: unable to get client id\n");

	struct IDiscordCore *core;
	struct DiscordCreateParams params = {
		.client_id = strtoul(client, NULL, 10),
		.flags = DiscordCreateFlags_NoRequireDiscord
	};
	if (DiscordCreate(DISCORD_VERSION, &params, &core) != DiscordResult_Ok)
		die("rpserv: unable to create instance of dsdk\n");

	int sock, flags;
	if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
		die("rpserv: unable to open socket: ");
	if ((flags = fcntl(sock, F_GETFL, 0)) < 0)
		die("rpserv: unable to get socket flags: ");
	if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1)
		die("rpserv: unable to set socket flags: ");
	if (remove(SOCKP) == -1 && errno != ENOENT)
		die("rpserv: unable to clear socket file location: ");

	union sockr addr = { .un.sun_family = AF_UNIX, .un.sun_path = SOCKP };
	socklen_t slen = sizeof(struct sockaddr);
	if (bind(sock, &addr.sa, slen) == -1)
		die("rpserv: unable to bind socket address: ");
	if (listen(sock, SOMAXCONN) == -1)
		die("rpserv: unable to listen on socket: ");

	int fd = -1;
	size_t len = 0;
	struct DiscordActivity act;
	struct IDiscordActivityManager *man = core->get_activity_manager(core);
	for (;;) {
		if (core->run_callbacks(core) != DiscordResult_Ok)
			die("rpserv: unable to run dsdk callbacks\n");

		if (fd == -1 && (fd = accept(sock, &addr.sa, &slen)) == -1) {
			if (errno != EAGAIN && errno != EWOULDBLOCK)
				die("rpserv: unable to accept connection: ");
			continue;	
		}

		if (read_act(&fd, &act, &len) != 0)
			man->update_activity(man, &act, NULL, NULL), len = 0;
	}
}
