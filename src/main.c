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
#include "misc.h"
#include "IP.h"
#include "CAN.h"
#include "UDS.h"

/* Macros */
#define MAC_FILTERING_ENABLED
#define CAN1_SPEED_500kbps 500000U
#define CAN1_DEFAULT_FRAME_ID 0x0101U

/* Global variables */
uint8_t DestAddr[6] = {0x50, 0x9A, 0x4C, 0x36, 0x00, 0x45}; // MAC address of PC
uint8_t SrcAddr[6]  = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00}; // MAC address of a microcontroller

uint8_t DestIP[4] = {192, 168, 0, 11}; // PC IP address
uint8_t SrcIP[4]  = {192, 168, 0, 10}; // Microcontroller IP address

volatile uint8_t ETHRxFrames = 0U;
volatile uint8_t CANRxFrames = 0U;

/* Global structures */
struct ETH_Header ethhdr;
struct IP_Header iphdr;
CAN_Mailbox cantx, canrx;
ETH_RxFrame RxFrame;

struct Interface CAN_interface = {CAN, &CAN1_PrepareMsg, &CAN1_SetDLC, (CAN_Mailbox *)&cantx};
struct Interface ETH_interface = {ETHERNET, &IP_Send, &IP_PrepareHeader, (struct IP_Header *)&iphdr};

/* Function prototypes */
void CAN1_DisableFiltering(void);

int main(void)
{
	/* Initialize onboard LEDs and a button */
	BSP_Init();

	/* Initialize Systick timer peripheral */
	/* For some reason variable SystemCoreClock divided by 1000 doesn't gives us 1 tick per 1 ms
	 * TODO Check why 60000 is the good value */
	SysTick_Init(60000U);

	/* Initialize Ethernet peripheral */
	Ethernet_Init();

	/* Set MAC address of the microcontroller and PC for filtering */
#ifdef MAC_FILTERING_ENABLED
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

	/* Initialize IP header structure */
	IP_StructInit(&iphdr, SrcIP, DestIP, 0U);

	/* Initialize CAN1 peripheral */
	CAN1_Init(CAN1_SPEED_500kbps);

	/* Disable filtering of CAN frames */
	CAN1_DisableFiltering();

	/* Set default ID of CAN frame */
	cantx.ID = CAN1_DEFAULT_FRAME_ID;

	while(1)
	{
		/* Check if we have received any message */
		if(ETHRxFrames)
		{
			ETH_ReceiveFrame(&RxFrame);

			/* Added small delay for better stability on script side */
			SysTick_Delay(2);

			/* Decode the message and respond to it */
			UDS_Respond(&ETH_interface, (uint8_t *)RxFrame.Buffer + SIZE_OF_ETH_IP_HDR);

			/* Give back control of Rx descriptor to the DMA */
			RxFrame.Desc->Status |= RX_DESC_OWN;

			/* Decrement number of received frames that we need to respond to */
			--ETHRxFrames;
		}

		if(CANRxFrames)
		{
			if(CAN_RECEIVED == CAN1_Receive(&canrx, CAN_FIFO_0))
			{
				UDS_Respond(&CAN_interface, canrx.Data);

				--CANRxFrames;

				/* Re-enable CAN interrupts */
				_CAN_FIFO0_MSG_PENDING_INTERRUPT_ENABLE;
			}
		}
	}
}


void CAN1_DisableFiltering(void)
{
	CAN_Filter canfilter;
	canfilter.FilterNum = 0U;
	canfilter.FilterIDH = 0U;
	canfilter.FilterIDL = 0U;
	canfilter.FilterMaskH = 0U;
	canfilter.FilterMaskL = 0U;

	CAN1_FilterConfig(&canfilter);
}

/* Enable CAN receive interrupt */
void CAN_UserStructInit(CAN_Init *caninit)
{
	_CAN_FIFO0_MSG_PENDING_INTERRUPT_ENABLE;
}

/* Enable MAC address filtering */
void ETH_MACUserStructInit(ETH_MACInit *macinit)
{
#ifdef MAC_FILTERING_ENABLED
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

/* Increment number of received Ethernet frames */
void ETH_RxCallback(void)
{
	++ETHRxFrames;
}

/* Increment number of received CAN frames */
void CAN1_RX0Callback(void)
{
	++CANRxFrames;
}
