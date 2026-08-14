#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>
#include <unistd.h>

#include "ping.h"

static volatile sig_atomic_t keepRunning = 1;

static void sigHandler(int _)
{
	(void)_;
	keepRunning = 0;
}

void	ping(char *host, t_config config)
{
	struct sigaction act;
	act.sa_handler = sigHandler;
	sigaction(SIGINT, &act, NULL);

	uint8_t	*buffer = malloc(sizeof(struct icmphdr) + config.payload_size);
	if (!buffer)
	{
		fprintf(stderr, "ping: malloc error: Malloc failed\n");
		exit(EXIT_FAILURE);
	}
	while (keepRunning)
	{
		if (config.quiet == 0)
		{
			printf("We should keep running this thing: %s\n", host);
		}
		sleep(1);
	}
	free(buffer);

	// PRINT RESULTS
}
