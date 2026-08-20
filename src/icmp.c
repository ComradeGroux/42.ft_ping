#include <string.h>
#include <netinet/ip_icmp.h>
#include <time.h>

#include "config.h"

static uint16_t	extract_checksum(const void *data)
{
	const uint16_t *buff = data;
	return buff[1];
}

static uint16_t	compute_checksum(const void *data, size_t len) {
	const uint16_t *buff = data;
	uint32_t sum = 0;

	while (len > 1)
	{
		sum += *buff++;
		len -= 2;
	}

	if (len == 1)
		sum += *(const uint8_t *)buff;

	while (sum >> 16)
		sum = (sum & 0xFFFF) + (sum >> 16);

	return (uint16_t)(~sum);
}

int	verify_checksum(const void *data, size_t len)
{
	uint16_t to_compute[len];
	if (len != 0)
	{
		memcpy(to_compute, data, len);
		to_compute[1] = 0;
	}

	return (compute_checksum(to_compute, len) == extract_checksum(data));
}

void	build_icmp_request(uint8_t *data, t_config config)
{
	static size_t packet_num = 0;

	struct icmphdr *header = (struct icmphdr*)data;
	header->type = ICMP_ECHO;
	header->code = 0;
	header->checksum = 0;
	header->un.echo.id = htons((config.identifier & 0xFFFF));
	header->un.echo.sequence = htons(packet_num);

	uint8_t	*payload = data + sizeof(header);
	memset(payload, 0, config.payload_size);

	struct timespec now;
	if (config.payload_size >= sizeof(now))
	{
		clock_gettime(CLOCK_MONOTONIC, &now);
		memcpy(payload, &now, sizeof(now));
	}

	header->checksum = compute_checksum(data, sizeof(struct icmphdr) + config.payload_size);

	packet_num++;
}
