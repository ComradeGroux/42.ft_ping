#pragma once

#include <sys/socket.h>

#include "config.h"

void	print_header(char* hostname, char* ip_resolved, t_config config);
void	print_packet(char* ip_resolved, int bytes_received, int id_sequence, int ttl_received, float rtt);
void	print_stats(char* hostname, int packet_transmitted, int packet_received, float min, float max);
