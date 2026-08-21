#pragma once

#include "config.h"

int		verify_checksum(const void *data, size_t len);
void	build_icmp_request(uint8_t *data, t_config config);
void	print_icmp_error(struct icmphdr *icmp, ssize_t len, struct sockaddr_in *src, t_config config);
