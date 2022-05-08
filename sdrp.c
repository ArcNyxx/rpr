#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/sysinfo.h>
#include <unistd.h>

#include "discord_game_sdk.h"

static void
die(const char *fmt, ...)
{
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);

        /* perror if last char not '\n' */
        if (fmt[strlen(fmt) - 1] != '\n')
                perror(NULL);
        exit(1);
}

int
main(int argc, char **argv)
{
	char *client;
	if ((client = getenv("DISCLIENT")) == NULL)
		die("sdrp: unable to get client\n");

	struct DiscordCreateParams params = {
		.client_id = strtoul(client, NULL, 10),
		.flags = DiscordCreateFlags_NoRequireDiscord
	};

	struct IDiscordCore *core;
	if (DiscordCreate(DISCORD_VERSION, &params, &core) != DiscordResult_Ok)
		die("sdrp: unable to open sdk instance\n");

	struct IDiscordActivityManager *man = core->get_activity_manager(core);

	struct DiscordActivity chg = { 0 };

	for (;;) {
		if (core->run_callbacks(core) != DiscordResult_Ok)
			die("sdrp: unable to run sdk callbacks\n");

		FILE *pipe;
		if ((pipe = popen("./status.sh", "r")) == NULL)
			die("sdrp: unable to check activity: ");

		char array[128] = { 0 };
		size_t total = 127;
		errno = 0;
		while ((total -= fread(array, 1, 127, pipe)) == 127)
			if (ferror(pipe))
				die("sdrp: unable to read activity pipe: ");
			else if (feof(pipe))
				break;
		while ((total -= fread(array, 1, total, pipe)) != 0 &&
				!feof(pipe))
			if (ferror(pipe))
				die("sdrp: unable to read activity pipe: ");
		pclose(pipe);

		if (!strcmp(array, chg.state) && array[0] != '\0')
			goto cont;

		if (array[0] == '\0') {
			strcpy(chg.state, "Working on nothing in "
					"particular...");

			struct sysinfo info;
			if (sysinfo(&info) == -1)
				die("sdrp: unable to retrieve sysinfo: ");
			chg.timestamps.start = time(NULL) - info.uptime;
		} else {
			memcpy(chg.state, array, total + 1);

			chg.timestamps.start = time(NULL);
		}

		snprintf(chg.assets.large_image, 128, "sdrp");
		snprintf(chg.assets.large_text, 128, "Linux");

		man->update_activity(man, &chg, NULL, NULL);

cont:
		struct timespec timep = { .tv_sec = 5 };
		while (nanosleep(&timep, &timep) != 0)
			if (errno != EINTR)
				die("sdrp: unable to sleep: ");
	}
}
