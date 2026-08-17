#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <time.h>

#include "socket.h"

static void	resolve_hostname(char* hostname, t_sock* s)
{
	struct addrinfo		hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_flags = 0;
	hints.ai_protocol = IPPROTO_ICMP;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	struct addrinfo*	res = NULL;

	int err = getaddrinfo(hostname, NULL, &hints, &res);
	if (err != 0)
	{
		fprintf(stderr, "ping error: getaddrinfo error: %s\n", gai_strerror(err));
		exit(EXIT_FAILURE);
	}

	if (res == NULL)
	{
		fprintf(stderr, "ping error: server error: Host unreachable\n");
		exit(EXIT_FAILURE);
	}

	s->addr = (struct sockaddr_in*)(res->ai_addr);
	inet_ntop(AF_INET, &(s->addr->sin_addr), s->ip_str, INET_ADDRSTRLEN);
	freeaddrinfo(res);
}

t_sock	create_socket(char *hostname, t_config config)
{
	t_sock s;
	s.socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (s.socket == -1)
	{
		perror("ping: socket error: ");
		exit(EXIT_FAILURE);
	}

	memset(&(s.addr), 0, sizeof(s.addr));
	resolve_hostname(hostname, &s);

	struct timespec	tv_out =
	{
		.tv_sec = config.interval,
		.tv_nsec = 0
	};
	if (setsockopt(s.socket, SOL_SOCKET, SO_RCVTIMEO, &tv_out, sizeof(tv_out)) < 0)
	{
		perror("ping: socket error: ");
		exit(EXIT_FAILURE);
	}

	return s;
}


