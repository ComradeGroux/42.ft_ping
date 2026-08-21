#pragma once

#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>

typedef struct s_config {
	uint8_t		verbose;
	uint8_t		quiet;
	uint16_t	identifier;
	size_t		payload_size;
	ssize_t		count;
	ssize_t		interval;
} t_config;
