/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include "stm32f4xx.h"
#include "bsp.h"
#include "Ethernet.h"
#include "IP.h"
#include "misc.h"
#include "string.h"

/* Macros */
#define SIZE_OF_HEADERS 34U
#define SIZE_OF_CRC     4U
#define MAX_FRAME_SIZE  83U

/* Global variables */
uint8_t DestAddr[6] = {0x50, 0x9A, 0x4C, 0x36, 0x00, 0x45}; // MAC address of PC
uint8_t SrcAddr[6]  = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00}; // MAC address of a microcontroller

uint8_t DestIP[4] = {192, 168, 0, 11}; // PC IP address
uint8_t SrcIP[4]  = {192, 168, 0, 10}; // Microcontroller IP address

/* Global structures */
struct ETH_Header ethhdr;
struct IP_Header iphdr;

/* Function prototypes */
void ETH_SendErrorFrame(ETH_TxDescriptor *DMATxDesc, uint16_t Framelength);


int main(void)
{
	volatile uint16_t Framelength;

	const uint8_t Responseheader[2] = {0x06, 0x1C};
	const uint8_t Errorheader[2]    = {0x03, 0xC4};

	/* Initialize onboard LEDs and a button */
	BSP_Init();

	/* Initialize Ethernet peripheral */
	Ethernet_Init();

	/* Set MAC address of the microcontroller for filtering */
	ETH_SetHWMACAddress(SrcAddr);

	/* Initialize transmit descriptor */
	ETH_DMATxDescInit(DMATxDesc);

	/* Initialize receive descriptor */
	ETH_DMARxDescInit(DMARxDesc);

	/* Enable Ethernet peripheral and DMA */
	ETH_Start();

	/* Initialize Ethernet header structure */
	ETH_HeaderStructInit(&ethhdr, DestAddr, SrcAddr);

	/* Prepare response in the first buffer */
	memcpy((uint8_t *)DMATxDesc->Buf1Addr, (uint8_t *)&ethhdr, sizeof(struct ETH_Header));
	memcpy((uint8_t *)DMATxDesc->Buf1Addr + SIZE_OF_HEADERS, Responseheader, sizeof(Responseheader));

	/* Prepare error message in the second buffer */
	IP_StructInit(&iphdr, SrcIP, DestIP, sizeof(Errorheader));
	memcpy((uint8_t *)DMATxDesc->Buf2NextDescAddr, (uint8_t *)&ethhdr, sizeof(struct ETH_Header));
	memcpy((uint8_t *)(DMATxDesc->Buf2NextDescAddr + sizeof(struct ETH_Header)), (uint8_t *)&iphdr, sizeof(struct IP_Header));
	memcpy((uint8_t *)DMATxDesc->Buf2NextDescAddr + SIZE_OF_HEADERS, Errorheader, sizeof(Errorheader));

	while(1)
	{
		/* Check if we have received any message */
		if((ETH->DMASR & RECEIVE_FINISHED) == RECEIVE_FINISHED)
		{
			/* Calculate length of received frame */
			Framelength = ((DMARxDesc->Status & RX_FRAMELENGTH_MAX) >> RX_FRAMELENGTH_OFFSET) - SIZE_OF_CRC;

			if (Framelength < MAX_FRAME_SIZE)
			{
				/* Calculate Total Length as well as checksum for outgoing frame */
				IP_StructInit(&iphdr, SrcIP, DestIP, Framelength + sizeof(Responseheader));

				/* Place IP header as well as response message in Tx buffer 1 */
				memcpy((uint8_t *)(DMATxDesc->Buf1Addr + sizeof(struct ETH_Header)), (uint8_t *)&iphdr, sizeof(struct IP_Header));
				memcpy((uint8_t *)DMATxDesc->Buf1Addr + SIZE_OF_HEADERS + sizeof(Responseheader),
						Rxbuff[0], swap_uint16(iphdr.TotalLength) - sizeof(struct IP_Header));

				/* Send response */
				ETH_DMAPrepareTxDesc(DMATxDesc, sizeof(struct ETH_Header) + swap_uint16(iphdr.TotalLength));
			}

			else
			{
				/* Send error frame */
				ETH_SendErrorFrame(DMATxDesc, SIZE_OF_HEADERS + sizeof(Errorheader));
			}

			/* Clear receive status and buffer unavailable status */
			ETH->DMASR = RECEIVE_FINISHED | RECEIVE_BUFFER_UNAVAILABLE;

			/* Give back control of Rx descriptor to the DMA */
			DMARxDesc->Status |= RX_DESC_OWN;
		}
	}
}


void ETH_SendErrorFrame(ETH_TxDescriptor *DMATxDesc, uint16_t Framelength)
{
	/* Check if the descriptor is owned by DMA or CPU, if by CPU then continue */
	if((DMATxDesc->ControlAndStatus & TX_DESC_OWN) != 0)
	{
		/* TODO add delay on Systick or return some status */
		BSP_Delay_ms(1);
	}

	/* Set the exact size of frame to be transferred */
	DMATxDesc->BufSize = 0U;
	DMATxDesc->BufSize = ((Framelength << TX_DESC_BUFFER2_OFFSET) & TX_DESC_BUF2SIZE_MAX);

	/* Frame is ready to be sent, give control do DMA */
	DMATxDesc->ControlAndStatus |= TX_DESC_OWN;

	/* Check for Transmit buffer unavailable flag, clear if set */
	if((ETH->DMASR & TRANSMIT_BUFFER_UNAVAILABLE) != 0)
	{
	    /* Clear Transmit buffer unavailable flag */
	    ETH->DMASR = TRANSMIT_BUFFER_UNAVAILABLE;

	    /* Resume DMA transmission*/
	    ETH->DMATPDR = 0;
	}
}

