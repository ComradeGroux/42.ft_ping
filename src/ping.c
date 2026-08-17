#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <time.h>
#include <unistd.h>

#include "ping.h"
#include "socket.h"
#include "print.h"

static volatile sig_atomic_t keepRunning = 1;

static void sigHandler(int _)
{
	(void)_;
	keepRunning = 0;
}

static void	init_signal(void)
{
	struct sigaction act = { 0 };
	act.sa_handler = sigHandler;
	sigaction(SIGINT, &act, NULL);
}

static double	get_elapsed_time(struct timespec start)
{
	struct timespec	end;
	clock_gettime(CLOCK_MONOTONIC, &end);

	return (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1e6;
}

void	ping(char *host, t_config config)
{
	init_signal();
	t_sock	s = create_socket(host, config);

	printf("IP: %s\n", s.ip_str);

	uint8_t	*buffer = malloc(sizeof(struct icmphdr) + config.payload_size);
	if (!buffer)
	{
		fprintf(stderr, "ping: malloc error: Malloc failed\n");
		exit(EXIT_FAILURE);
	}
	memset(buffer, 0, sizeof(struct icmphdr) + config.payload_size);

	size_t	count = config.count == 0 ? -1 : config.count;
	struct timespec	start;
	while (keepRunning && count != 0)
	{
		if (config.interval != 0)
			clock_gettime(CLOCK_MONOTONIC, &start);
		count--;

		if (config.quiet == 0)
			printf("We should keep running this thing: %s\n", host);






		double	remaining_time_ms = (config.interval * 1000.0) - get_elapsed_time(start);
		if (config.interval != 0 && remaining_time_ms > 0)
		{
			start.tv_sec  = (long)(remaining_time_ms / 1000.0);
			start.tv_nsec = (long)((remaining_time_ms - start.tv_sec * 1000.0) * 1e6);
			nanosleep(&start, NULL);
		}
	}
	free(buffer);

	// PRINT RESULTS
}
