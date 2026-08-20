#pragma once

#include <unistd.h>
#include <stdint.h>

typedef struct s_config {
	uint8_t		verbose;
	uint8_t		quiet;
	uint16_t	identifier;
	size_t		payload_size;
	uint8_t		count;
	uint8_t		interval;
} t_config;
