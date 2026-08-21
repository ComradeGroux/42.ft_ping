#pragma once

#include "config.h"
#include "stats.h"

void	print_header(char* hostname, char* ip_resolved, t_config config);
void	print_packet(char* ip_resolved, int bytes_received, int id_sequence, int ttl_received, float rtt);
void	print_stats(char* hostname, t_stats);
