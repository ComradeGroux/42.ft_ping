#pragma once

#include <unistd.h>

typedef struct s_stats {
	size_t	transmitted;
	size_t	received;
	double	min;
	double	max;
	double	sum;
	double	sum_squared;
} t_stats;
