/*
 * tcp.h
 *
 *  Created on: 14 May 2020
 *      Author: wsadzik
 */

#ifndef TCP_H_
#define TCP_H_

#include "stdint.h"

#define SIZEOF_TCP_HEADER 20U
#define SIZEOF_ETH_IP_TCP_HDR 54U

typedef enum {
	LISTEN 		 = 0,
	SYN_SENT 	 = 1,
	SYN_RECEIVED = 2,
	ESTABLISHED  = 3,
	FIN_WAIT_1	 = 4,
	FIN_WAIT_2 	 = 5,
	CLOSE_WAIT 	 = 6,
	CLOSING      = 7,
	LAST_ACK 	 = 8,
	TIME_WAIT 	 = 9,
	CLOSED 		 = 10
} TCP_State;

/* Transmission control block */
typedef struct {
	TCP_State state;

	uint16_t destport;

	uint32_t lastack;

	uint32_t lastseq;

	uint32_t nextseq;

} TCP_tcb;

/* tcb related macros */
#define TCP_MAX_CONNECTIONS 3U // Number of tcb's

struct TCP_Header {
	uint16_t SrcPort;

	uint16_t DestPort;

	uint32_t Seq;

	uint32_t Ack;

	uint8_t Reserved:3;

	uint8_t NS:1;

	uint8_t DataOffset:4;

	uint8_t Flags;

	uint16_t WindowSize;

	uint16_t Checksum;

	uint16_t UrgentPointer;

	uint32_t MSS;

} __attribute__ ((packed));

/* Ports */
#define HTTP_PORT (uint16_t)80U
#define NO_PORT 0U

#define TCP_DYNAMIC_PORT_RANGE_BEGIN 0xC000U
#define TCP_DYNAMIC_PORT_RANGE_END   0xFFFFU
#define TCP_IS_PORT_IN_DYN_PORT_RANGE(x) ((x) >= (TCP_DYNAMIC_PORT_RANGE_BEGIN) ? (1) : (0))

/* Flags */
#define CWR (uint8_t)(1U << 7)
#define ECE (uint8_t)(1U << 6)
#define URG (uint8_t)(1U << 5)
#define ACK (uint8_t)(1U << 4)
#define PSH (uint8_t)(1U << 3)
#define RST (uint8_t)(1U << 2)
#define SYN (uint8_t)(1U << 1)
#define FIN (uint8_t)(1U << 0)

/* Options */
#define MAXIMUM_SEGMENT_SIZE 	(uint32_t)0x02U
#define MSS_LENGTH				(uint32_t)0x04U
#define MSS_MAX_VALUE 			(uint32_t)1460U
#define MSS_DEFAULT 			(uint32_t)(MAXIMUM_SEGMENT_SIZE << 24 | MSS_LENGTH << 16 | MSS_MAX_VALUE)

/* Default values for checksum function */
#define NO_OPT  0U
#define NO_DATA 0U

/* ===================== */
/*  Function prototypes  */
/* ===================== */

void TCP_Respond(uint8_t *buffer, uint16_t framelength);
void TCP_SendData(TCP_tcb *tcb, uint8_t *msg, uint16_t framelength);
void TCP_AbortConnection(TCP_tcb *tcb, struct TCP_Header *rtcphdr);
void TCP_SendACK(TCP_tcb *tcb);
void TCP_LoadPage(TCP_tcb *tcb, uint16_t recvframelength);
void TCP_CreateConnection(TCP_tcb *tcb, struct TCP_Header *rtcphdr);
void TCP_PrepareHeader(TCP_tcb *tcb, uint16_t recvframelength, uint8_t flags, uint16_t datalength);
void TCP_HeaderInit(struct TCP_Header *rtcphdr);
uint16_t TCP_CalculateChecksum(uint16_t datalength, uint16_t datasum, uint8_t optsize);
void TCP_Updatetcb(TCP_tcb *tcb, struct TCP_Header *rtcphdr);
TCP_tcb *TCP_FindFreetcb(void);
TCP_tcb *TCP_FindtcbByPort(uint16_t port);
void TCP_Freetcb(TCP_tcb *tcb);

#endif /* TCP_H_ */

