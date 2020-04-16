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

/* Global variables */
uint8_t DestAddr[6] = {0x50, 0x9A, 0x4C, 0x36, 0x00, 0x45}; // MAC address of PC
uint8_t SrcAddr[6] =  {0x02, 0x00, 0x00, 0x00, 0x00, 0x00}; // MAC address of a microcontroller

uint8_t DestIP[4] = {192, 168, 0, 11}; // PC IP address
uint8_t SrcIP[4] =  {192, 168, 0, 10}; // Microcontroller IP address

uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF,
				  0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF,
				  0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF,
				  0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF,
				  0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF,
				  0xDE, 0xAD, 0xBE, 0xEF};

/* Global structures */
struct ETH_Header ethhdr;
struct IP_Header iphdr;


int main(void)
{
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

	/* Initialize IP header structure */
	IP_StructInit(&iphdr, SrcIP, DestIP, sizeof(data));

	/* Copy headers and data to be sent to transmit buffer and prepare them for sending */
	IP_Send(&iphdr, &ethhdr, data);

	/* Resend the data every 500 miliseconds */
	while(1)
	{
		BSP_Delay_ms(500);
		ETH_DMAPrepareTxDesc(DMATxDesc, sizeof(struct ETH_Header) + swap_uint16(iphdr.TotalLength));
	}
}
