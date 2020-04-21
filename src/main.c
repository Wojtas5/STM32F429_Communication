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
#include "UDS.h"

/* Macros */
#define SIZE_OF_HEADERS 34U
#define DISABLED 		((uint32_t)0U)

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

	/* Set MAC address of the microcontroller and PC for filtering */
	ETH_SetMACAddress(MAC0, SrcAddr);
	ETH_SetMACAddress(MAC1, DestAddr);

	/* Initialize transmit descriptor */
	ETH_DMATxDescInit(DMATxDesc);
	DMATxDesc->ControlAndStatus |= TX_DESC_DISABLE_PAD;

	/* Initialize receive descriptor */
	ETH_DMARxDescInit(DMARxDesc);

	/* Enable Ethernet peripheral and DMA */
	ETH_Start();

	/* Initialize Ethernet header structure */
	ETH_HeaderStructInit(&ethhdr, DestAddr, SrcAddr);

	/* Initialize IP header structure */
	IP_StructInit(&iphdr, SrcIP, DestIP, 0U);

	while(1)
	{
		/* Check if we have received any message */
		if((ETH->DMASR & RECEIVE_FINISHED) == RECEIVE_FINISHED)
		{
			/* Added small delay for better stability on script side */
			BSP_Delay_ms(1);

			/* Decode the message and respond to it */
			UDS_Respond((uint8_t *)DMARxDesc->Buf1Addr + SIZE_OF_HEADERS);

			/* Clear receive status and buffer unavailable status */
			ETH->DMASR = RECEIVE_FINISHED | RECEIVE_BUFFER_UNAVAILABLE;

			/* Give back control of Rx descriptor to the DMA */
			DMARxDesc->Status |= RX_DESC_OWN;
		}
	}
}

/* Enable MAC address filtering */
void ETH_MACUserStructInit(ETH_MACInit *macinit)
{
	macinit->ReceiveAll = DISABLED;
	macinit->SourceAddressFilter = SOURCE_ADDRESS_FILTER_ENABLED;
	macinit->BroadcastFramesFilter = BROADCAST_FRAMES_FILTER_ENABLED;
}

