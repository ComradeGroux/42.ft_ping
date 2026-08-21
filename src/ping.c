#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <netinet/ip_icmp.h>

#include "ping.h"
#include "socket.h"
#include "print.h"

static volatile sig_atomic_t	keepRunning = 1;
static volatile sig_atomic_t	sendPacket = 1;

static void sigHandler(int signum)
{
	if (signum == SIGALRM)
		sendPacket = 1;
	else if (signum == SIGINT)
		keepRunning = 0;
}

static void	init_signal(void)
{
	struct sigaction act = { 0 };
	act.sa_handler = sigHandler;
	sigaction(SIGINT, &act, NULL);
	sigaction(SIGALRM, &act, NULL);
}

void	ping(char *host, t_config config)
{
	init_signal();
	t_sock	s = create_socket(host, config);

	uint8_t	*request = malloc(sizeof(struct icmphdr) + config.payload_size);
	if (!request)
	{
		close(s.socket);
		fprintf(stderr, "ft_ping: malloc error: Malloc failed\n");
		exit(EXIT_FAILURE);
	}
	memset(request, 0, sizeof(struct icmphdr) + config.payload_size);

	uint8_t	*response = malloc(sizeof(struct iphdr) + sizeof(struct icmphdr) + config.payload_size);
	if (response == NULL)
	{
		free(request);
		close(s.socket);
		fprintf(stderr, "ft_ping: malloc error: Malloc failed\n");
		exit(EXIT_FAILURE);
	}

	size_t	count = config.count == 0 ? -1 : config.count;
	size_t	interval = config.interval == 0 ? 1 : config.interval;
	t_stats	stats;
	memset(&stats, 0, sizeof(stats));
	print_header(host, s.ip_str, config);
	while (keepRunning && count != 0)
	{
		if (sendPacket)
		{
			sendPacket = 0;
			alarm(interval);
			send_request(request, s, config, &stats);
		}

		if (receive_response(response, s, config, &stats) > 0)
			count--;
	}
	free(request);
	free(response);
	close(s.socket);

	print_stats(host, stats);
}
