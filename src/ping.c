#include <stdio.h>
#include <signal.h>

#include "ping.h"
#include "config.h"

static volatile sig_atomic_t keepRunning = 1;

static void sigHandler(int _)
{
	(void)_;
	keepRunning = 0;
}

void	ping(char *host, t_config flags)
{
	struct sigaction act;
	act.sa_handler = sigHandler;
	sigaction(SIGINT, &act, NULL);

	if (flags.verbose)
		printf("Flag -v is here\n");
	else
		printf("There is no flags\n");

	while (keepRunning)
	{
		printf("We should keep running this thing: %s\n", host);
	}
	// PRINT RESULTS
}
