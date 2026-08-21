#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <errno.h>
#include <time.h>

#include "socket.h"
#include "icmp.h"
#include "print.h"

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
		fprintf(stderr, "ft_ping error: getaddrinfo error: %s\n", gai_strerror(err));
		exit(EXIT_FAILURE);
	}

	if (res == NULL)
	{
		fprintf(stderr, "ft_ping error: server error: Host unreachable\n");
		exit(EXIT_FAILURE);
	}

	memcpy(&(s->addr), res->ai_addr, sizeof(struct sockaddr_in));
	freeaddrinfo(res);

	if (inet_ntop(AF_INET, &(s->addr.sin_addr), s->ip_str, INET_ADDRSTRLEN) == NULL)
	{
		perror("ft_ping: inet_ntop error");
		exit(EXIT_FAILURE);
	}
}

t_sock	create_socket(char *hostname, t_config config)
{
	t_sock s;
	memset(&(s.addr), 0, sizeof(s.addr));
	resolve_hostname(hostname, &s);

	s.socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (s.socket == -1)
	{
		perror("ft_ping: socket error");
		exit(EXIT_FAILURE);
	}

	struct timespec	tv_out =
	{
		.tv_sec = config.interval == 0 ? 1 : config.interval,
		.tv_nsec = 0
	};
	if (setsockopt(s.socket, SOL_SOCKET, SO_RCVTIMEO, &tv_out, sizeof(tv_out)) < 0)
	{
		close(s.socket);
		perror("ft_ping: socket error");
		exit(EXIT_FAILURE);
	}

	return s;
}

void	send_request(uint8_t *buffer, t_sock s, t_config config, t_stats *stats)
{
	build_icmp_request(buffer, config);
	if (sendto(s.socket, buffer, (sizeof(struct icmphdr) + config.payload_size), 0, (struct sockaddr *)&(s.addr), sizeof(s.addr)) == -1)
	{
		close(s.socket);
		perror("ft_ping: send error");
		exit(EXIT_FAILURE);
	}
	stats->transmitted++;
}

static inline size_t	extract_ttl(uint8_t *buffer)
{
	struct iphdr	*ip_header = (struct iphdr*)buffer;
	return (size_t)ip_header->ttl;
}

static inline struct icmphdr	*skip_ip_header(uint8_t *buffer)
{
	return (struct icmphdr*)(buffer + (((struct iphdr *)buffer)->ihl * 4));
}

static double	get_elapsed_time(struct timespec start)
{
	struct timespec	end;
	clock_gettime(CLOCK_MONOTONIC, &end);

	return (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1e6;
}

int		receive_response(uint8_t *buffer, t_sock s, t_config config, t_stats *stats)
{
	struct sockaddr_in	src;
	socklen_t			src_len = sizeof(src);
	ssize_t	n = recvfrom(s.socket, buffer, (sizeof(struct iphdr) + sizeof(struct icmphdr) + config.payload_size), 0, (struct sockaddr*)&src, &src_len);

	if (n == -1)
	{
		switch (errno)
		{
			case EINTR:
				return 0;
			case EWOULDBLOCK:
				return -1;
			default:
				;
		}
		close(s.socket);
		perror("ft_ping: recvfrom error");
		exit(EXIT_FAILURE);
	}
	else if (n <= (ssize_t)sizeof(struct icmphdr))
	{
		close(s.socket);
		perror("ft_ping: recvfrom error");
		exit(EXIT_FAILURE);
	}

	struct icmphdr	*icmp = skip_ip_header(buffer);
	if (icmp->type != ICMP_ECHOREPLY && icmp->type != ICMP_ECHO)
	{
		if (!config.quiet)
			print_icmp_error(icmp, n - sizeof(struct iphdr), &src, config);
		return 0;
	}

	if (icmp->un.echo.id != ntohs((config.identifier & 0xFFFF)) || icmp->type == ICMP_ECHO)
		return 0;

	if (!verify_checksum(icmp, n - sizeof(struct iphdr)))
	{
		if (config.verbose)
			fprintf(stderr, "ft_ping: warning: corrupted packet received (bad checksum)\n");
		return -1;
	}

	ssize_t	payload_len = n - sizeof(struct iphdr) - sizeof(struct icmphdr);
	double	rtt = -1.0;
	if (payload_len - (ssize_t)sizeof(struct timespec) >= 0.0)
	{
		struct timespec	start;
		memcpy(&start, (uint8_t*)icmp + sizeof(struct icmphdr), sizeof(struct timespec));
		rtt = get_elapsed_time(start);

		if (rtt < stats->min || stats->received == 0)
			stats->min = rtt;
		if (rtt > stats->max || stats->received == 0)
			stats->max = rtt;

		stats->sum += rtt;
		stats->sum_squared += rtt * rtt;
	}

	stats->received++;
	if (!config.quiet)
		print_packet(s.ip_str, n - sizeof(struct iphdr), ntohs(icmp->un.echo.sequence), extract_ttl(buffer), rtt);

	return 1;
}
