/*
 * IP.c
 *
 *  Created on: Apr 9, 2020
 *      Author: wsadzik
 */

#include "IP.h"
#include "string.h"
#include "misc.h"

/* Initialize IP_Header structure with default values and calculate total length and Checksum */
void IP_StructInit(struct IP_Header *iphdr, uint8_t *srcip, uint8_t *destip, uint16_t len)
{
	iphdr->Version = IPV4_VERSION;
	iphdr->HeaderLength = DEFAULT_HEADER_LENGTH;
	iphdr->TypeOfService = DEFAULT_TOS;
	iphdr->TotalLength = swap_uint16(sizeof(struct IP_Header) + len);
	iphdr->ID = DEFAULT_ID;
	iphdr->flags = NO_FLAGS;
	iphdr->FragmentOffset = DEFAULT_FRAG_OFFSET;
	iphdr->TTL = DEFAULT_TTL;
	iphdr->Protocol = TCP_PROTOCOL;
	iphdr->Checksum = 0U;
	iphdr->SrcIP = swap_uint32(IPV4_ADDR(srcip));
	iphdr->DestIP = swap_uint32(IPV4_ADDR(destip));
	iphdr->Checksum = IP_CalculateChecksum(iphdr);
}

/* Calculate checksum for IP header */
uint16_t IP_CalculateChecksum(struct IP_Header *iphdr)
{
	uint32_t sum = 0U;
	uint16_t *ip = (uint16_t *)iphdr;
	uint8_t count = sizeof(struct IP_Header)/2;

	while (count > 0)
	{
		sum += *(ip++);
		count--;
	}

	sum = (sum & 0xFFFF) + (sum >> 16);

	return (uint16_t)~sum;
}

/* Copy IP and Ethernet headers as well as data to transmit buffer */
void IP_Send(struct IP_Header *iphdr, struct ETH_Header *ethhdr, uint8_t *data)
{
	memcpy((uint8_t *)DMATxDesc->Buf1Addr, (uint8_t *)ethhdr, sizeof(struct ETH_Header));
	memcpy((uint8_t *)(DMATxDesc->Buf1Addr + sizeof(struct ETH_Header)), (uint8_t *)iphdr, sizeof(struct IP_Header));
	memcpy((uint8_t *)(DMATxDesc->Buf1Addr + sizeof(struct ETH_Header) + sizeof(struct IP_Header)),
		   data, swap_uint16(iphdr->TotalLength) - sizeof(struct IP_Header));

	ETH_DMAPrepareTxDesc(DMATxDesc, sizeof(struct ETH_Header) + swap_uint16(iphdr->TotalLength));
}


