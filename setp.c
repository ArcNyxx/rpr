/* sdrp - rich presence
 * Copyright (C) 2022 ArcNyxx
 * see LICENCE file for licensing information */

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <fcntl.h>
#include <sys/sysinfo.h>
#include <unistd.h>

#include <dsdk.h>

static volatile sig_atomic_t new = 1;

static void
read_status(struct DiscordActivity *act)
{
	if (read(STDIN_FILENO, act, sizeof(struct DiscordActivity)) ==
			sizeof(struct DiscordActivity))
		return;

	/* reset to default */
	memset(act, '\0', sizeof(struct DiscordActivity));
	strcpy(act->state, "Working on nothing in particular...");
	strcpy(act->assets.large_image, "sdrp");
	strcpy(act->assets.large_text, "Linux");
	
	struct sysinfo info;
	if (sysinfo(&info) == 0)
		act->timestamps.start = time(NULL) - info.uptime;
}

static void
sighand(int signal)
{
	new = 1;
}

int
main(int argc, char **argv)
{
	char *client;
	if ((client = getenv("DISCLIENT")) == NULL)
		return 1;

	struct sigaction signal = { .sa_handler = sighand };
	sigemptyset(&signal.sa_mask);
	if (sigaction(SIGUSR1, &signal, NULL) != 0)
		return 1;

	struct IDiscordCore *core;
	struct DiscordCreateParams params = {
		.client_id = strtoul(client, NULL, 10),
		.flags = DiscordCreateFlags_NoRequireDiscord
	};
	if (DiscordCreate(DISCORD_VERSION, &params, &core) != DiscordResult_Ok)
		return 1;

	/* ensure STDIN_FILENO does not block on read */
	int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

	struct DiscordActivity act = { 0 };
	struct IDiscordActivityManager *man = core->get_activity_manager(core);
	for (;;) {
		if (core->run_callbacks(core) != DiscordResult_Ok)
			return 1;

		if (new == 1) {
			read_status(&act);
			man->update_activity(man, &act, NULL, NULL);
			new = 0;
		}

		struct timespec timep = { .tv_sec = 5 };
		while (nanosleep(&timep, &timep) != 0);
	}
}
