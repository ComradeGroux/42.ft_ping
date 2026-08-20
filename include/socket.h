#pragma once

#include <arpa/inet.h>
#include <sys/socket.h>

#include "config.h"
#include "stats.h"

typedef struct s_sock {
	int					socket;
	struct sockaddr_in	addr;
	char				ip_str[INET_ADDRSTRLEN];
} t_sock;

t_sock	create_socket(char *hostname, t_config config);
void	send_request(uint8_t *buffer, t_sock s, t_config config, t_stats *stats);
int		receive_response(uint8_t *buffer, t_sock s, t_config config, t_stats *stats);
