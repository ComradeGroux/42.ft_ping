#pragma once

#include <stdint.h>

typedef struct s_config {
	uint8_t		verbose;
	uint8_t		quiet;
	uint16_t	identifier;
	size_t		payload_size;
} t_config;
