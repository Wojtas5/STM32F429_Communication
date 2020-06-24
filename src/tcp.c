/*
 * tcp.c
 *
 *  Created on: 14 May 2020
 *      Author: wsadzik
 */
#if 0 /* TODO don't build if not configured for use */
#include "tcp.h"
#include "bsp.h"
#include "IP.h"
#include "misc.h"
#include "html-page.h"
#include <string.h>

struct TCP_Header tcphdr;
extern struct IP_Header iphdr;

TCP_tcb tcptcb[TCP_MAX_CONNECTIONS];
static uint8_t buf[1480];

void TCP_Respond(uint8_t *msg, uint16_t framelength)
{
	struct TCP_Header *rtcphdr = (struct TCP_Header *)msg;
	TCP_tcb *tcb = TCP_FindtcbByPort(rtcphdr->SrcPort);

	switch(rtcphdr->Flags)
	{
		case SYN:
			tcb = TCP_FindFreetcb();

			if(LISTEN == tcb->state)
			{
				TCP_CreateConnection(tcb, rtcphdr);
			}

			else
			{
				/* No more free tcb's */
				/* TODO use timer and queue actual connection, after timeout check for free tcb */
				TCP_AbortConnection(tcb, rtcphdr);
			}
			break;

		case ACK:
			if(SYN_RECEIVED != tcb->state && FIN_WAIT_1 != tcb->state)
			{
				TCP_AbortConnection(tcb, rtcphdr); //+1 do Seq dla syn_received i fin_wait_1
			}
			break;

		case (PSH | ACK):
			if(ESTABLISHED == tcb->state)
			{
				TCP_SendData(tcb, msg, framelength);
			}

			else
			{
				TCP_AbortConnection(tcb, rtcphdr);
			}
			break;

		case (FIN | ACK):
			if(FIN_WAIT_2 == tcb->state)
			{
				/* Acknowledge the end of connection */
				TCP_SendACK(tcb);
			}

			else
			{
				TCP_AbortConnection(tcb, rtcphdr);
			}
			break;

		case (RST | ACK):
			tcb->state = CLOSED;
			break;

		default:
			TCP_AbortConnection(tcb, rtcphdr);
			break;
	}

	/* Update actual tcb values */
	TCP_Updatetcb(tcb, rtcphdr);
}

/* Respond with a data according to received request */
void TCP_SendData(TCP_tcb *tcb, uint8_t *msg, uint16_t framelength)
{
	uint8_t *data = msg + SIZEOF_TCP_HEADER;

	if(strncmp((const char *)data, "GET /", 5) == 0)
	{
		TCP_LoadPage(tcb, framelength);
	}

	else
	{
		/* Wrong request abort connection */
		TCP_AbortConnection(tcb, (struct TCP_Header *)msg);
	}
}


void TCP_AbortConnection(TCP_tcb *tcb, struct TCP_Header *rtcphdr)
{
	IP_PrepareHeader(&iphdr, SIZEOF_TCP_HEADER);

	/* Prepare TCP header based on received frame, because this function can be called anytime */
	tcphdr.Seq = rtcphdr->Ack;
	tcphdr.Ack = rtcphdr->Seq;
	tcphdr.SrcPort = rtcphdr->DestPort;
	tcphdr.DestPort = rtcphdr->SrcPort;
	tcphdr.DataOffset = SIZEOF_TCP_HEADER/sizeof(uint32_t);
	tcphdr.Flags = RST | ACK;
	tcphdr.WindowSize = swap_uint16(2920U);
	tcphdr.UrgentPointer = 0U;
	tcphdr.Checksum = TCP_CalculateChecksum(NO_DATA, NO_DATA, NO_OPT);

	IP_Send((uint8_t *)&tcphdr);

	/* Mark tcb ready to be freed */
	tcb->state = CLOSED;
}


void TCP_SendACK(TCP_tcb *tcb)
{
	IP_PrepareHeader(&iphdr, SIZEOF_TCP_HEADER);
	TCP_PrepareHeader(tcb, NO_DATA, ACK, NO_DATA);

	IP_Send((uint8_t *)&tcphdr);
}

/* Load a html page via http */
void TCP_LoadPage(TCP_tcb *tcb, uint16_t recvframelength)
{
	IP_PrepareHeader(&iphdr, SIZEOF_TCP_HEADER + TEST_PAGE_LEN);
	TCP_PrepareHeader(tcb, recvframelength, FIN | ACK, TEST_PAGE_LEN);

	memcpy(buf, (uint8_t *)&tcphdr, SIZEOF_TCP_HEADER);
	memcpy(buf + SIZEOF_TCP_HEADER, TEST_PAGE, TEST_PAGE_LEN);
	IP_Send(buf);
}

/* Respond with SYN and ACK, create a tcb */
void TCP_CreateConnection(TCP_tcb *tcb, struct TCP_Header *rtcphdr)
{
	if(TCP_IS_PORT_IN_DYN_PORT_RANGE(swap_uint16(rtcphdr->SrcPort)))
	{
		IP_PrepareHeader(&iphdr, SIZEOF_TCP_HEADER + sizeof(tcphdr.MSS));
		TCP_HeaderInit(rtcphdr);

		IP_Send((uint8_t *)&tcphdr);
	}

	else
	{
		/* Invalid port number */
		TCP_AbortConnection(tcb, rtcphdr);
	}
}


void TCP_PrepareHeader(TCP_tcb *tcb, uint16_t recvframelength, uint8_t flags, uint16_t datalength)
{
	tcphdr.SrcPort = swap_uint16(HTTP_PORT);//
	tcphdr.DestPort = tcb->destport;
	tcphdr.Seq = tcb->nextseq;
	tcphdr.DataOffset = SIZEOF_TCP_HEADER/sizeof(uint32_t);
	tcphdr.Flags = flags;

	/* Set Ack value of outgoing frame */
	if(NO_DATA != recvframelength)
	{
		tcphdr.Ack = swap_uint32(swap_uint32(tcb->lastack) + (recvframelength - SIZEOF_ETH_IP_TCP_HDR));
	}

	else
	{
		tcphdr.Ack = inc_swapped_uint32(tcb->lastack);
	}

	/* Calculate checksum for outgoing frame */
	if(TEST_PAGE_LEN == datalength)
	{
		tcphdr.Checksum = TCP_CalculateChecksum(TEST_PAGE_LEN, TEST_PAGE_SUM(), NO_OPT);
	}

	else
	{
		tcphdr.Checksum = TCP_CalculateChecksum(NO_DATA, NO_DATA, NO_OPT);
	}
}


void TCP_HeaderInit(struct TCP_Header *rtcphdr)
{
	tcphdr.SrcPort = swap_uint16(HTTP_PORT);
	tcphdr.DestPort = rtcphdr->SrcPort;
	tcphdr.Seq = swap_uint32(6510U); /* TODO randomize Initial Sequence number */
	tcphdr.Ack = inc_swapped_uint32(rtcphdr->Seq);
	tcphdr.DataOffset = sizeof(struct TCP_Header)/sizeof(uint32_t);
	tcphdr.NS = 0U;
	tcphdr.Flags = (SYN | ACK);
	tcphdr.WindowSize = swap_uint16(2920U);
	tcphdr.UrgentPointer = 0U;
	tcphdr.MSS = swap_uint32(MSS_DEFAULT);
	tcphdr.Checksum = TCP_CalculateChecksum(NO_DATA, NO_DATA, sizeof(tcphdr.MSS));
}

/* Calculate checksum for TCP header */
uint16_t TCP_CalculateChecksum(uint16_t datalength, uint16_t datasum, uint8_t optsize)
{
	tcphdr.Checksum = 0U;

	uint32_t sum = 0U;
	uint16_t *tcp = (uint16_t *)&tcphdr;
	uint8_t count = (SIZEOF_TCP_HEADER + optsize)/2;

	/* Sum values from IP pseudo-header */
	sum = (uint16_t)(iphdr.SrcIP >> 16) + (uint16_t)iphdr.SrcIP +
		  (uint16_t)(iphdr.DestIP >> 16) + (uint16_t)iphdr.DestIP +
		  swap_uint16(TCP_PROTOCOL + SIZEOF_TCP_HEADER + optsize + datalength + datasum);

	while (count > 0)
	{
		sum += *(tcp++);
		count--;
	}

	sum = (sum & 0xFFFF) + (sum >> 16);

	return (uint16_t)~sum;
}


void TCP_Updatetcb(TCP_tcb *tcb, struct TCP_Header *rtcphdr)
{
	switch(tcb->state)
	{
		case LISTEN:
			tcb->state = SYN_RECEIVED;
			tcb->destport = rtcphdr->SrcPort;
			tcb->lastack = rtcphdr->Seq;
			tcb->lastseq = tcphdr.Seq;
			tcb->nextseq = tcphdr.Seq;
			break;

		case SYN_RECEIVED:
			tcb->state = ESTABLISHED;
			tcb->nextseq = inc_swapped_uint32(tcb->nextseq);
			break;

		case ESTABLISHED:
			tcb->state = FIN_WAIT_1;
			tcb->lastack = tcphdr.Ack;
			tcb->lastseq = tcphdr.Seq;
			tcb->nextseq = swap_uint32(swap_uint32(tcphdr.Seq) + TEST_PAGE_LEN);
			break;

		case FIN_WAIT_1:
			tcb->state = FIN_WAIT_2;
			tcb->lastack = rtcphdr->Seq;
			tcb->nextseq = inc_swapped_uint32(tcb->nextseq);
			break;

		case FIN_WAIT_2:
			/* TODO set a timeout after which the state will be CLOSED/LISTEN and free tcb */
			tcb->state = TIME_WAIT;
			TCP_Freetcb(tcb);
			break;

		case TIME_WAIT:
		case CLOSED:
			TCP_Freetcb(tcb);
			break;

		default:
			break;
	}
}

/* Returns address of first free tcb */
TCP_tcb *TCP_FindFreetcb(void)
{
	return TCP_FindtcbByPort(NO_PORT);
}


TCP_tcb *TCP_FindtcbByPort(uint16_t port)
{
	for(int i = 0; i < TCP_MAX_CONNECTIONS; ++i)
	{
		if(port == tcptcb[i].destport)
		{
			return &tcptcb[i];
		}
	}

	return NULL;
}


void TCP_Freetcb(TCP_tcb *tcb)
{
	tcb->state = LISTEN;
	tcb->destport = NO_PORT;
}

#endif
