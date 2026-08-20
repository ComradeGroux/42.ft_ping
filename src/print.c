#include <stdio.h>
#include <math.h>

#include "print.h"

void	print_header(char* hostname, char* ip_resolved, t_config config)
{
	printf("PING %s (%s): %ld data bytes\n", hostname, ip_resolved, config.payload_size);
}

void	print_packet(char* ip_resolved, int bytes_received, int id_sequence, int ttl_received, float rtt)
{
	if (rtt > 0.0)
		printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n", bytes_received, ip_resolved, id_sequence, ttl_received, rtt);
	else
		printf("%d bytes from %s: icmp_seq=%d ttl=%d\n", bytes_received, ip_resolved, id_sequence, ttl_received);
}

void	print_stats(char* hostname, t_stats stats)
{
	printf("--- %s ping statistics ---\n", hostname);
	printf("%ld packets transmitted, %ld packets received, %ld%% packet loss\n", stats.transmitted, stats.received, (stats.transmitted - stats.received) * 100 / stats.transmitted);

	if (stats.received != 0 && stats.min != 0.0)
	{
		double	avg = stats.sum / stats.transmitted;
		double	variance = (stats.sum_squared / stats.received) - (avg * avg);
		printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", stats.min, avg, stats.max, sqrt(variance));
	}
}
