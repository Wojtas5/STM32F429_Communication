/*
 * IP.h
 *
 *  Created on: Apr 9, 2020
 *      Author: wsadzik
 */

#ifndef IP_H_
#define IP_H_

#include "stdint.h"
#include "Ethernet.h"
#include "interface.h"

#define SIZE_OF_IP_HDR 20U
#define SIZE_OF_ETH_IP_HDR 34U

struct IP_Header {
	uint8_t HeaderLength : 4;

	uint8_t Version : 4;

	uint8_t TypeOfService;

	uint16_t TotalLength;

	uint16_t ID;

	uint16_t FragmentOffset : 13;

	uint16_t flags : 3;

	uint8_t TTL;

	uint8_t Protocol;

	uint16_t Checksum;

	uint32_t SrcIP;

	uint32_t DestIP;

} __attribute__ ((packed));

/* IP_Header structure related macros */
#define IPV4_VERSION 		  0x4U
#define DEFAULT_HEADER_LENGTH 0x5U
#define DEFAULT_TOS 		  0x00U
#define DEFAULT_ID 			  0x0000U
#define NO_FLAGS 			  0b000U
#define DONT_FRAGMENT 		  0b010U
#define MORE_FRAGMENTS 		  0b001U
#define DEFAULT_FRAG_OFFSET   0x0000U
#define DEFAULT_TTL           64U
#define TCP_PROTOCOL          6U
#define UDP_PROTOCOL          17U

#define IPV4_ADDR(ipaddr) ((uint32_t)ipaddr[0] << 24 | \
						   (uint32_t)ipaddr[1] << 16 | \
						   (uint32_t)ipaddr[2] << 8 |  \
						   (uint32_t)ipaddr[3])        \


/* ===================== */
/*  Function prototypes  */
/* ===================== */

void IP_StructInit(struct IP_Header *iphdr, uint8_t *srcip, uint8_t *destip, uint16_t len);
void IP_PrepareHeader(struct Interface *interface, uint16_t len);
uint16_t IP_CalculateChecksum(struct IP_Header *iphdr);
void IP_Send(struct Interface *interface, uint8_t *data);
void IP_PrepareStaticMessage(uint8_t *srcip, uint8_t *destip,
							 ETH_TxDescriptor *DMATxDesc, uint32_t buffer,
		 	 	 	 	 	 struct ETH_Header *ethhdr, struct IP_Header *iphdr,
							 const uint8_t *data, uint16_t datasize);

#endif /* IP_H_ */
