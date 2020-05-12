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
#define MAX_FRAME_SIZE  83U

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
	const uint8_t Responseheader[2] = {0x06, 0x1C};
	const uint8_t Errorheader[2]    = {0x03, 0xC4};

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
	DMATxDesc->ControlAndStatus |= TX_DESC_DISABLE_PAD;

	/* Initialize receive descriptors in a list structure */
	ETH_DMARxDescListInit(DMARxDesc, RX_DESCRIPTORS);

	/* Enable Ethernet peripheral and DMA */
	ETH_Start();

	/* Initialize Ethernet header structure */
	ETH_HeaderStructInit(&ethhdr, DestAddr, SrcAddr);

	/* Prepare response in the first buffer */
	IP_PrepareStaticMessage(SrcIP, DestIP,
							DMATxDesc, DMATxDesc->Buf1Addr,
							&ethhdr, &iphdr,
							Responseheader, sizeof(Responseheader));

	/* Prepare error message in the second buffer */
	IP_PrepareStaticMessage(SrcIP, DestIP,
							DMATxDesc, DMATxDesc->Buf2NextDescAddr,
							&ethhdr, &iphdr,
							Errorheader, sizeof(Errorheader));

	while(1)
	{
		/* Check if we have received any message */
		if(RxFrames)
		{
			ETH_ReceiveFrame(&RxFrame);

			/* Added small delay for better stability on script side */
			SysTick_Delay(2);

			if(RxFrame.Framelength < MAX_FRAME_SIZE)
			{
				/* Calculate Total Length as well as checksum for outgoing frame */
				IP_StructInit(&iphdr, SrcIP, DestIP, RxFrame.Framelength + sizeof(Responseheader));

				/* Place IP header as well as response message in Tx buffer 1 */
				memcpy((uint8_t *)(DMATxDesc->Buf1Addr + sizeof(struct ETH_Header)), (uint8_t *)&iphdr, sizeof(struct IP_Header));
				memcpy((uint8_t *)DMATxDesc->Buf1Addr + SIZE_OF_HEADERS + sizeof(Responseheader),
					   (uint8_t *)RxFrame.Buffer, swap_uint16(iphdr.TotalLength) - sizeof(struct IP_Header));

				/* Send response */
				ETH_DMAPrepareTxDesc(DMATxDesc, sizeof(struct ETH_Header) + swap_uint16(iphdr.TotalLength), DMATxDesc->Buf1Addr);
			}

			else
			{
				ETH_DMAPrepareTxDesc(DMATxDesc, SIZE_OF_HEADERS + sizeof(Errorheader), DMATxDesc->Buf2NextDescAddr);
			}

			/* Decrement number of received frames that we need to respond to */
			RxFrames--;

			/* Give back control of Rx descriptor to the DMA */
			RxFrame.Desc->Status |= RX_DESC_OWN;
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
