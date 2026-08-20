#include <stdio.h>

#include "print.h"

void	print_header(char* hostname, char* ip_resolved, t_config config)
{
	printf("PING %s (%s): %ld data bytes\n", hostname, ip_resolved, config.payload_size);
}

void	print_packet(char* ip_resolved, int bytes_received, int id_sequence, int ttl_received, float rtt)
{
	printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n", bytes_received, ip_resolved, id_sequence, ttl_received, rtt);
}

void	print_stats(char* hostname, int packet_transmitted, int packet_received, float min, float max)
{
	printf("--- %s ping statistics ---\n", hostname);
	printf("%d packets transmitted, %d packets received, %d%% packet loss", packet_transmitted, packet_received, (packet_received * 100 / packet_transmitted));
	printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", min, (min + max) / 2.0f, max, 0.0f);
}
