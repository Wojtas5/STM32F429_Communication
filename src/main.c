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
#include "tcp.h"
#include "string.h"

/* Macros */
#define FILTERING_ENABLED

/* Global variables */
uint8_t DestAddr[6] = {0x50, 0x9A, 0x4C, 0x36, 0x00, 0x45}; // MAC address of PC
uint8_t SrcAddr[6]  = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00}; // MAC address of a microcontroller

uint8_t DestIP[4] = {192, 168, 0, 11}; // PC IP address
uint8_t SrcIP[4]  = {192, 168, 0, 10}; // Microcontroller IP address

volatile uint8_t RxFrames = 0U;

/* Global structures */
struct ETH_Header ethhdr;
struct IP_Header iphdr;
ETH_RxFrame RxFrame;

/* Function prototypes */

int main(void)
{
	/* Initialize onboard LEDs and a button */
	BSP_Init();

	/* Initialize Systick timer peripheral */
	/* For some reason variable SystemCoreClock divided by 1000 doesn't gives us 1 tick per 1 ms
	 * TODO Check why 60000 is the good value */
	Systick_Init(60000U);

	/* Initialize Ethernet peripheral */
	Ethernet_Init();

	/* Set MAC address of the microcontroller and PC for filtering */
#ifdef FILTERING_ENABLED
	ETH_SetMACAddress(MAC0, SrcAddr);
	ETH_SetMACAddress(MAC1, DestAddr);
#endif /* FILTERING_ENABLED */

	/* Initialize transmit descriptor */
	ETH_DMATxDescInit(DMATxDesc);
	//DMATxDesc->ControlAndStatus |= TX_DESC_DISABLE_PAD;

	/* Initialize receive descriptors in a list structure */
	ETH_DMARxDescListInit(DMARxDesc, RX_DESCRIPTORS);

	/* Enable Ethernet peripheral and DMA */
	ETH_Start();

	/* Initialize Ethernet header structure */
	ETH_HeaderStructInit(&ethhdr, DestAddr, SrcAddr);

	/* Initialize IP header structure */
	IP_StructInit(&iphdr, SrcIP, DestIP, 0U);

	while(1)
	{
		/* Check if we have received any message */
		if(RxFrames)
		{
			ETH_ReceiveFrame(&RxFrame);

			/* Added small delay for better stability on script side */
			//SysTick_Delay(2);

			/* Decode the message and respond to it */
			//UDS_Respond((uint8_t *)RxFrame.Buffer + SIZE_OF_HEADERS);
			TCP_Respond((uint8_t *)RxFrame.Buffer + SIZE_OF_ETH_IP_HDR, RxFrame.Framelength);

			/* Give back control of Rx descriptor to the DMA */
			RxFrame.Desc->Status |= RX_DESC_OWN;

			/* Decrement number of received frames that we need to respond to */
			RxFrames--;
		}
	}
}

/* Enable MAC address filtering */
void ETH_MACUserStructInit(ETH_MACInit *macinit)
{
#ifdef FILTERING_ENABLED
	macinit->ReceiveAll = DISABLED;
	macinit->SourceAddressFilter = SOURCE_ADDRESS_FILTER_ENABLED;
	macinit->BroadcastFramesFilter = BROADCAST_FRAMES_FILTER_ENABLED;
#endif /* FILTERING_ENABLED */
}

/* Enable DMA receive interrupt */
void ETH_DMAUserStructInit(ETH_DMAInit *dmainit)
{
	ETH->DMAIER |= NORMAL_INTERRUPT_ENABLE | RECEIVE_INTERRUPT_ENABLE;
}

/* Increment number of received frames */
void ETH_RxCallback(void)
{
	RxFrames++;
}

