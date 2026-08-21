#include <string.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <time.h>
#include <stdio.h>

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

	uint8_t	*payload = data + sizeof(struct icmphdr);
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

static char*	get_icmp_type_str(int type, int code)
{
	switch (type)
	{
		case ICMP_DEST_UNREACH:
			switch (code)
			{
				case ICMP_NET_UNREACH:
					return "Destination Net Unreachable";
				case ICMP_HOST_UNREACH:
					return "Destination Host Unreachable";
				case ICMP_PROT_UNREACH:
					return "Destination Protocol Unreachable";
				case ICMP_PORT_UNREACH:
					return "Destination Port Unreachable";
				case ICMP_FRAG_NEEDED:
					return "Frag needed";
				case ICMP_SR_FAILED:
					return "Source Route Failed";
				case ICMP_NET_UNKNOWN:
					return "Destination Net Unknown";
				case ICMP_HOST_UNKNOWN:
					return "Destination Host Unknown";
				case ICMP_HOST_ISOLATED:
					return "Source Host Isolated";
				case ICMP_NET_ANO:
					return "Destination Net Prohibited";
				case ICMP_HOST_ANO:
					return "Destination Host Prohibited";
				case ICMP_NET_UNR_TOS:
					return "Destination Net Unreachable for Type of Service";
				case ICMP_HOST_UNR_TOS:
					return "Destination Host Unreachable for Type of Service";
				case ICMP_PKT_FILTERED:
					return "Packet filtered";
				case ICMP_PREC_VIOLATION:
					return "Precedence Violation";
				case ICMP_PREC_CUTOFF:
					return "Precedence Cutoff";
				default:
					return "Dest Unreachable";
			}
		case ICMP_SOURCE_QUENCH:
			return "Source Quench";
		case ICMP_REDIRECT:
			switch (code)
			{
				case ICMP_REDIR_NET:
					return "Redirect Network";
				case ICMP_REDIR_HOST:
					return "Redirect Host";
				case ICMP_REDIR_NETTOS:
					return "Redirect Type of Service and Network";
				case ICMP_REDIR_HOSTTOS:
					return "Redirect Type of Service and Host";
				default:
					return "Redirect";
			}
		default:
			return "Unknown error";
	}
}

void	print_icmp_error(struct icmphdr *icmp, ssize_t len, struct sockaddr_in *src, t_config config)
{
	char	senderIP[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &src->sin_addr, senderIP, sizeof(senderIP));
	printf("%ld bytes from %s: %s\n", len, senderIP, get_icmp_type_str(icmp->type, icmp->code));
	struct iphdr	*iphdr = (struct iphdr*)((uint8_t*)icmp + sizeof(struct icmphdr));

	if (config.verbose == 0)
		return;

	printf("IP Hdr Dump:\n");
	uint8_t	*bytes = (uint8_t*)iphdr;
	for (size_t i = 0; i < sizeof(struct iphdr); i += 2)
	{
		printf(" %02x%02x", *bytes, *(bytes + 1));
		bytes += 2;
	}

	printf("\nVr HL TOS  Len   ID Flg  off TTL Pro  cks      Src	Dst	Data\n");
	printf(" %x  %x  %02x %04x %04x   %x %04x  %02x  %02x %04x ",
		iphdr->version, iphdr->ihl, iphdr->tos, ntohs(iphdr->tot_len), ntohs(iphdr->id), ntohs(iphdr->frag_off) >> 13,
		ntohs(iphdr->frag_off) & 0x1FFF, iphdr->ttl, iphdr->protocol,
		ntohs(iphdr->check));
	inet_ntop(AF_INET, &iphdr->saddr, senderIP, sizeof(senderIP));
	printf("%s  ", senderIP);
	inet_ntop(AF_INET, &iphdr->daddr, senderIP, sizeof(senderIP));
	printf("%s\n", senderIP);

	icmp = (struct icmphdr*)((uint8_t*)iphdr + sizeof(struct iphdr));
	printf("ICMP: type %d, code %d, size %ld, id 0x%4x, seq 0x%04x\n", icmp->type, icmp->code,
		config.payload_size - sizeof(struct icmphdr), icmp->un.echo.id, icmp->un.echo.sequence);
}
