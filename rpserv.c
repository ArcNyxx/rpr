/* rpr - rich presence
 * Copyright (C) 2022 ArcNyxx
 * see LICENCE file for licensing information */

#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <unistd.h>

#include <dsdk.h>

#define SOCKP "/tmp/setp"

static volatile sig_atomic_t new = 1;

static void
read_status(struct DiscordActivity *act)
{
	if (scanf("%s %s %s %s", act->state, act->details, act->assets.
				small_image, act->assets.small_text) != EOF)
		return;

	/* reset to default */
	memset(act, '\0', sizeof(struct DiscordActivity));
	strcpy(act->state, "Working on nothing in particular...");
	strcpy(act->assets.large_image, "Linux");
	strcpy(act->assets.large_text, "Linux");
	
	struct sysinfo info;
	if (sysinfo(&info) == 0)
		act->timestamps.start = time(NULL) - info.uptime;
}

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
	struct sockaddr_un addr = { .sun_family = AF_UNIX, .sun_path = SOCKP };
	socklen_t slen = sizeof(struct sockaddr)
	if (bind(sock, (struct sockaddr *)addr, slen) == -1)
		return 1;
	if (listen(sock, SOMAXCONN) == -1)
		return 1;

	int fd;
	struct DiscordActivity act = { 0 };
	struct IDiscordActivityManager *man = core->get_activity_manager(core);
	for (;;) {
		if (core->run_callbacks(core) != DiscordResult_Ok)
			return 1;

		if (fd == -1 && (fd = accept(sock,
				(struct sockaddr *)addr, slen)) == -1) {
			if (errno == EAGAIN && errno == EWOULDBLOCK)
				continue;
			else
				return 1;
		}

		struct pollfd pollfd = { .fd = fd,
				.events = POLLRDNORM | POLLHUP };
		int num = poll(&pollfd, 1, 0);

		if (pollfd.revents & POLLRDNORM) {
			if (read(fd, &act, sizeof(struct DiscordActivity)) !=
					sizeof(struct DiscordActivity))
				return 1;
			man->update_activity(man, &act, NULL, NULL);
		}

		if (pollfd.revents & POLLHUP) {
			close(fd);
			fd = -1;
		}
	}
}
