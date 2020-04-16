/*
 * Ethernet.c
 *
 *  Created on: Apr 2, 2020
 *      Author: wsadzik
 */

#include "stm32f4xx.h"
#include "Ethernet.h"
#include "bsp.h"
#include "misc.h"

/* Macros */
#define DISABLED ((uint32_t)0U)

/* Global variables */
uint8_t PHYAddress = 0x00;

/* Create aligned space for descriptors and buffers */
#if defined (__GNUC__) /* GNU Compiler */
ETH_TxDescriptor DMATxDesc[TX_DESCRIPTORS] __attribute__ ((aligned (4)));
ETH_RxDescriptor DMARxDesc[RX_DESCRIPTORS] __attribute__ ((aligned (4)));
uint8_t Txbuff[TX_DESCRIPTORS*2][TX_BUF_SIZE] __attribute__ ((aligned (4)));
uint8_t Rxbuff[RX_DESCRIPTORS*2][RX_BUF_SIZE] __attribute__ ((aligned (4)));
#endif


void Ethernet_Init(void)
{
	uint16_t PHYReg = 0U;

	ETH_PHYInit PHYInit;
	ETH_MACInit MACInit;
	ETH_DMAInit DMAInit;

	/* Initialization of Clock, GPIO and NVIC */
	Ethernet_LowLevelInit();

	/* Enable SYSCFG peripheral */
	RCC_SYSCFGEnable();

	/* Select RMII interface */
	SYSCFG_SelectRMII();

	/* Ethernet MAC software reset */
	ETH_MACReset();

	/* Wait for Software Reset bit to be cleared */
	while ((ETH->DMABMR & ETH_RESET) != 0);

	/* Clock range selection for MDC clock */
	ETH_PHYSetMDCClock();

	/* Reset the PHY device */
	ETH_PHYWrite(PHY_BCR, PHY_SOFTRESET);

	/* 500ms delay after reset */
	/* TODO use Systick or timer delay */
	BSP_Delay_ms(PHY_SOFTRESET_DELAY);

	/* Wait for SOFTRESET bit to clear */
	do
	{
		ETH_PHYRead(PHY_BCR, &PHYReg);
	} while((PHYReg & PHY_SOFTRESET) == PHY_SOFTRESET);

	/* Configure the PHY device */
	PHYInit.LoopbackMode = (uint16_t)DISABLED;
	PHYInit.Autonegotiation = PHY_AUTONEGOTIATION_ENABLED;
	PHYInit.DuplexMode = (uint16_t)DISABLED;
	PHYInit.Speed = (uint16_t)DISABLED;

	PHYReg = (PHYInit.LoopbackMode |
			  PHYInit.Autonegotiation |
			  PHYInit.DuplexMode |
			  PHYInit.Speed);

	ETH_PHYWrite(PHY_BCR, PHYReg);

	/* Wait for autonegotiation process to finish */
	do
	{
		ETH_PHYRead(PHY_BSR, &PHYReg);
	} while((PHYReg & PHY_AUTONEGOTIATION_COMPLETE) != PHY_AUTONEGOTIATION_COMPLETE);

	/* Initialize configuration structures */
	ETH_MACStructInit(&MACInit, &PHYInit);
	ETH_DMAStructInit(&DMAInit);

	/* Configure MAC and DMA registers with values set in configuration structures */
	ETH_MACConfig(&MACInit);
	ETH_DMAConfig(&DMAInit);
}


void Ethernet_LowLevelInit(void)
{
	/* Enable clock for GPIOs */
	RCC->AHB1ENR   |= (GPIOA_ENABLE | GPIOB_ENABLE | GPIOC_ENABLE | GPIOG_ENABLE);

	/* Ethernet pins configuration */
	GPIOA->MODER   |= (PIN1_AF | PIN2_AF | PIN7_AF);
	GPIOA->OSPEEDR |= (PIN1_HS | PIN2_HS | PIN7_HS);
	GPIOA->PUPDR   |= (PIN1_NOPULL | PIN2_NOPULL | PIN7_NOPULL);
	GPIOA->AFR[0]  |= (PIN1_ETH | PIN2_ETH | PIN7_ETH);

	GPIOB->MODER   |= PIN13_AF;
	GPIOB->OSPEEDR |= PIN13_HS;
	GPIOB->PUPDR   |= PIN13_NOPULL;
	GPIOB->AFR[1]  |= PIN13_ETH;

	GPIOC->MODER   |= (PIN1_AF | PIN4_AF | PIN5_AF);
	GPIOC->OSPEEDR |= (PIN1_HS | PIN4_HS | PIN5_HS);
	GPIOC->PUPDR   |= (PIN1_NOPULL | PIN4_NOPULL | PIN5_NOPULL);
	GPIOC->AFR[0]  |= (PIN1_ETH | PIN4_ETH | PIN5_ETH);

	GPIOG->MODER   |= (PIN2_AF | PIN11_AF | PIN13_AF);
	GPIOG->OSPEEDR |= (PIN2_HS | PIN11_HS | PIN13_HS);
	GPIOG->PUPDR   |= (PIN2_NOPULL | PIN11_NOPULL | PIN13_NOPULL);
	GPIOG->AFR[0]  |= PIN2_ETH;
	GPIOG->AFR[1]  |= (PIN11_ETH | PIN13_ETH);

	/* Enable Ethernet Interrupts and set priority */
	NVIC_SetPriority(ETH_IRQn, 0);
	NVIC_EnableIRQ(ETH_IRQn);

	/* Enable clock for Ethernet peripheral */
	RCC->AHB1ENR   |= (ETHMAC_ENABLE | ETHMACTX_ENABLE | ETHMACRX_ENABLE);
}


void ETH_HeaderStructInit(struct ETH_Header *ethhdr, uint8_t *destaddr, uint8_t *srcaddr)
{
	/* Set destination MAC address */
	ethhdr->DestinationAddress[0] = destaddr[0];
	ethhdr->DestinationAddress[1] = destaddr[1];
	ethhdr->DestinationAddress[2] = destaddr[2];
	ethhdr->DestinationAddress[3] = destaddr[3];
	ethhdr->DestinationAddress[4] = destaddr[4];
	ethhdr->DestinationAddress[5] = destaddr[5];

	/* Set source MAC address */
	ethhdr->SourceAddress[0] = srcaddr[0];
	ethhdr->SourceAddress[1] = srcaddr[1];
	ethhdr->SourceAddress[2] = srcaddr[2];
	ethhdr->SourceAddress[3] = srcaddr[3];
	ethhdr->SourceAddress[4] = srcaddr[4];
	ethhdr->SourceAddress[5] = srcaddr[5];

	/* Set type of header to IPv4 */
	ethhdr->Type = swap_uint16(IPV4_TYPE);
}

/* Sets the default configuration for MAC registers */
void ETH_MACStructInit(ETH_MACInit *macinit, ETH_PHYInit *phyinit)
{
	uint16_t PHYReg = 0U;

	/* Setup for MACCR register */
	ETH_PHYRead(PHY_BSR, &PHYReg); // Check for configuration from autonegotiation

	if((PHYReg & PHY_AUTONEGOTIATION_COMPLETE) == PHY_AUTONEGOTIATION_COMPLETE)
	{
		ETH_PHYRead(PHY_SR, &PHYReg);

		if((PHYReg & PHY_100M) == PHY_100M)
		{
			phyinit->Speed = PHY_SPEED_100M;
			macinit->Speed = ETHERNET_SPEED_100M;
		}
		else
		{
			phyinit->Speed = PHY_SPEED_10M;
			macinit->Speed = ETHERNET_SPEED_10M;
		}

		if((PHYReg & PHY_FULLDUPLEX) == PHY_FULLDUPLEX)
		{
			phyinit->DuplexMode = PHY_FULLDUPLEX_MODE;
			macinit->DuplexMode = FULLDUPLEX_MODE;
		}
		else
		{
			phyinit->DuplexMode = PHY_HALFDUPLEX_MODE;
			macinit->DuplexMode = HALFDUPLEX_MODE;
		}
	}
	else
	{
		macinit->Speed = ETHERNET_SPEED_100M;
		macinit->DuplexMode = HALFDUPLEX_MODE;
	}

	macinit->CRCstrip = DISABLED;
	macinit->Watchdog = WATCHDOG_ENABLED;
	macinit->Jabber = JABBER_ENABLED;
	macinit->InterFrameGap = INTERFRAME_GAP_96BIT;
	macinit->CarrierSense = CARRIER_SENSE_ENABLED;
	macinit->ReceiveOwn = RECEIVE_OWN_ENABLED;
	macinit->LoopbackMode = DISABLED;
	macinit->IPv4Checksum = IPV4_CHECKSUM_ENABLED;
	macinit->RetryTransmission = RETRY_TRANSMISSION_ENABLED;
	macinit->AutomaticPADCRCStrip = DISABLED;
	macinit->BackoffLimit = BACKOFF_LIMIT_10;
	macinit->DeferralCheck = DEFERRAL_CHECK_ENABLED;

	/* Setup for MACFFR register */
	macinit->ReceiveAll = RECEIVEALL_ENABLED;
	macinit->SourceAddressFilter = DISABLED;
	macinit->PassControlFrames = PASS_CONTROL_FRAMES_BLOCKALL;
	macinit->PromiscuousMode = DISABLED;

	/* Setup for MACFCR register */
	macinit->PauseTime = PAUSE_TIME(0);
	macinit->ZeroquantaPause = ZEROQUANTA_PAUSE_DISABLED;
	macinit->PauseLowThreshold = PAUSE_TIME_THRESHOLD_MINUS4;
	macinit->ReceiveFlowControl = DISABLED;
	macinit->TransmitFlowControl = DISABLED;

	/* Setup for MACVLANTR register */
	macinit->VLANTagComparison = VLAN_TAG_16BIT;
	macinit->VLANTagIdentifier = 0x0U;

	ETH_MACUserStructInit(macinit);
}

/* In this function User can set every MAC structure element individually */
__attribute__ ((weak)) void ETH_MACUserStructInit(ETH_MACInit *macinit)
{
}


void ETH_MACConfig(ETH_MACInit *macinit)
{
	uint32_t tempreg = 0U;

	/* MACCR register Init */
	tempreg = ETH->MACCR;

	tempreg = (macinit->CRCstrip |
			   macinit->Watchdog |
			   macinit->Jabber |
			   macinit->InterFrameGap |
			   macinit->CarrierSense |
			   macinit->Speed |
			   macinit->DuplexMode |
			   macinit->ReceiveOwn |
			   macinit->LoopbackMode |
			   macinit->IPv4Checksum |
			   macinit->RetryTransmission |
			   macinit->AutomaticPADCRCStrip |
			   macinit->BackoffLimit |
			   macinit->DeferralCheck);

	ETH->MACCR = tempreg;

	/* MACFFR register Init */
	tempreg = ETH->MACFFR;

	tempreg = (macinit->ReceiveAll |
			   macinit->SourceAddressFilter |
			   macinit->PassControlFrames |
			   macinit->PromiscuousMode);

	ETH->MACFFR |= tempreg;

	/* MACFCR register Init */
	tempreg = ETH->MACFCR;

	tempreg = (macinit->PauseTime |
			   macinit->ZeroquantaPause |
			   macinit->PauseLowThreshold |
			   macinit->ReceiveFlowControl |
			   macinit->TransmitFlowControl);

	ETH->MACFCR |= tempreg;

	/* MACVLANTR register Init */
	tempreg = ETH->MACVLANTR;

	tempreg = (macinit->VLANTagComparison |
			   macinit->VLANTagIdentifier);

	ETH->MACVLANTR = tempreg;
}


void ETH_DMAStructInit(ETH_DMAInit *dmainit)
{
	/* Setup for DMABMR register */
	dmainit->MixedBurst = DISABLED;
	dmainit->AddressAlignedBeats = ADDRESS_ALIGNED_BEATS_ENABLED;
	dmainit->PBLx4Mode = DISABLED;
	dmainit->UseSeparatePBL = DISABLED;
	dmainit->FixedBurst = FIXED_BURST_ENABLED;
	dmainit->RxTxPriorityRatio = RX_TX_PRIORITY_1_1;
	dmainit->PBL = PBL_32;
#ifdef USE_ENCHANCED_DESCRIPTORS
	dmainit->EnchancedDescriptorFormat = ENCHANCED_DESC_FORMAT_ENABLED;
#else
	dmainit->EnchancedDescriptorFormat = DISABLED;
#endif
	dmainit->DMAArbitration = ROUND_ROBIN_PRIORITY;

	/* Setup for DMAOMR register */
	dmainit->TCPIPChecksumErrorFramesDrop = TCPIP_CHECKSUM_ERROR_FRAMES_DROPPING_ENABLED;
	dmainit->ReceiveStoreAndForward = RECEIVE_STORE_AND_FORWARD_ENABLED;
	dmainit->ReceivedFramesFlushing = RECEIVED_FRAMES_FLUSHING_DISABLED;
	dmainit->TransmitStoreAndForward = TRANSMIT_STORE_AND_FORWARD_ENABLED;
	dmainit->TransmitThresholdControl = TRANSMIT_THRESHOLD_64;
	dmainit->ForwardErrorFrames = DISABLED;
	dmainit->ForwardUndersizedFrames = DISABLED;
	dmainit->ReceiveThresholdControl = RECEIVE_THRESHOLD_64;
	dmainit->OperateOnSecondFrame = DISABLED;

	ETH_DMAUserStructInit(dmainit);
}

/* In this function User can set every DMA structure element individually */
__attribute__ ((weak)) void ETH_DMAUserStructInit(ETH_DMAInit *dmainit)
{
}


void ETH_DMAConfig(ETH_DMAInit *dmainit)
{
	uint32_t tempreg = 0U;

	/* DMABMR register Init */
	tempreg = ETH->DMABMR;

	tempreg = (dmainit->MixedBurst |
			   dmainit->AddressAlignedBeats |
			   dmainit->PBLx4Mode |
			   dmainit->UseSeparatePBL |
			   dmainit->FixedBurst |
			   dmainit->RxTxPriorityRatio |
			   dmainit->PBL |
			   dmainit->EnchancedDescriptorFormat |
			   dmainit->DMAArbitration);

	ETH->DMABMR = tempreg;

	/* DMAOMR register Init */
	tempreg = ETH->DMAOMR;

	tempreg = (dmainit->TCPIPChecksumErrorFramesDrop |
			   dmainit->ReceiveStoreAndForward |
			   dmainit->ReceivedFramesFlushing |
			   dmainit->TransmitStoreAndForward |
			   dmainit->TransmitThresholdControl |
			   dmainit->ForwardErrorFrames |
			   dmainit->ForwardUndersizedFrames |
			   dmainit->ReceiveThresholdControl |
			   dmainit->OperateOnSecondFrame);

	ETH->DMAOMR = tempreg;
}


void ETH_SetHWMACAddress(uint8_t *addr)
{
	ETH->MACA0HR = (MAC_ADDR0HR_MO | (addr[5] << 8) | addr[4]);
	ETH->MACA0LR = ((addr[3] << 24) | (addr[2] << 16) | (addr[1] << 8) | addr[0]);
}

/* This function enables MAC transmitter */
void ETH_MACTxEnable(void)
{
	ETH->MACCR |= TRANSMITTER_ENABLED;
}


void ETH_MACTxDisable(void)
{
	ETH->MACCR &= ~TRANSMITTER_ENABLED;
}


void ETH_MACRxEnable(void)
{
	ETH->MACCR |= RECEIVER_ENABLED;
}


void ETH_MACRxDisable(void)
{
	ETH->MACCR &= ~RECEIVER_ENABLED;
}


void ETH_DMATxEnable(void)
{
	ETH->DMAOMR |= START_DMA_TRANSMISSION;
}


void ETH_DMATxDisable(void)
{
	ETH->DMAOMR &= ~START_DMA_TRANSMISSION;
}


void ETH_DMARxEnable(void)
{
	ETH->DMAOMR |= START_DMA_RECEIVE;
}


void ETH_DMARxDisable(void)
{
	ETH->DMAOMR &= ~START_DMA_RECEIVE;
}


void ETH_DMAFlushTxFIFO(void)
{
	ETH->DMAOMR = FLUSH_TRANSMIT_FIFO;
}


void ETH_Start(void)
{
	ETH_MACTxEnable();
	ETH_MACRxEnable();
	ETH_DMAFlushTxFIFO();
	ETH_DMATxEnable();
	ETH_DMARxEnable();
}


void ETH_Stop(void)
{
	ETH_DMATxDisable();
	ETH_DMARxDisable();
	ETH_MACRxDisable();
	ETH_DMAFlushTxFIFO();
	ETH_MACTxDisable();
}


void ETH_PHYSetMDCClock(void)
{
	/* Update SystemCoreClock variable */
	SystemCoreClockUpdate();

	if((SystemCoreClock >= 20000000U) && (SystemCoreClock < 35000000U))
	{
		/* Clock range between 20-35 MHz */
		ETH->MACMIIAR |= ETH_CR_DIV16;
	}

	else if((SystemCoreClock >= 35000000U) && (SystemCoreClock < 60000000U))
	{
		ETH->MACMIIAR |= ETH_CR_DIV26;
	}

	else if((SystemCoreClock >= 60000000U) && (SystemCoreClock < 100000000U))
	{
		ETH->MACMIIAR |= ETH_CR_DIV42;
	}

	else if((SystemCoreClock >= 100000000U) && (SystemCoreClock < 150000000U))
	{
		ETH->MACMIIAR |= ETH_CR_DIV62;
	}

	else /* if((SystemCoreClock >= 150000000U) && (SystemCoreClock <= 180000000U)) */
	{
		ETH->MACMIIAR |= ETH_CR_DIV102;
	}
}


void ETH_MACReset(void)
{
	ETH->DMABMR |= ETH_RESET;
}


void ETH_PHYWrite(uint16_t PHYReg, uint16_t PHYRegValue)
{
	uint32_t tempreg = 0U;

	/* Copy the Ethernet MACMIIAR register value */
	tempreg = ETH->MACMIIAR;

	/* Keep only the value of Clock Range CR bits */
	tempreg &= ~ETH_CR_MASK;

	/* Prepare the tempreg value to be written to MACMIIAR */
	tempreg |= (PHYAddress << 11);  // Set the PHY address
	tempreg |= (PHYReg << 6);       // Set the PHY register address
	tempreg |= ETH_MW_WRITE;        // Select the write operation
	tempreg |= ETH_MB_BUSY;         // Set the busy bit

	/* Write the data to be transmitted to MACMIIDR data register */
	ETH->MACMIIDR = PHYRegValue;

	/* Write the value of tempreg to MACMIIAR register to begin transmission */
	ETH->MACMIIAR = tempreg;

	/* Wait for busy bit to clear */
	while((tempreg & ETH_MB_BUSY) == ETH_MB_BUSY)
	{
		tempreg = ETH->MACMIIAR;
	}
}


void ETH_PHYRead(uint16_t PHYReg, uint16_t *PHYRegValue)
{
	uint32_t tempreg = 0U;

	/* Copy the Ethernet MACMIIAR register value */
	tempreg = ETH->MACMIIAR;

	/* Keep only the value of Clock Range CR bits */
	tempreg &= ~ETH_CR_MASK;

	/* Prepare the tempreg value to be written to MACMIIAR */
	tempreg |= (PHYAddress << 11);  // Set the PHY address
	tempreg |= (PHYReg << 6);       // Set the PHY register address
	tempreg |= ETH_MW_READ;         // Select the read operation
	tempreg |= ETH_MB_BUSY;         // Set the busy bit

	/* Write the value of tempreg to MACMIIAR register to begin transmission */
	ETH->MACMIIAR = tempreg;

	/* Wait for busy bit to clear */
	while((tempreg & ETH_MB_BUSY) == ETH_MB_BUSY)
	{
		tempreg = ETH->MACMIIAR;
	}

	/* Write received value to PHYRegValue */
	*PHYRegValue = (uint16_t)ETH->MACMIIDR;
}


void ETH_DMATxDescInit(ETH_TxDescriptor *DMATxDesc)
{
	uint32_t tempreg = 0U;

	/* ControlAndStatus */
	/* I will be sending one simple message so I initialize this descriptor as
	 * first and last segment and in ring configuration
	 */
	tempreg = (TX_DESC_INT_ON_COMPLETION_DISABLED |
			   TX_DESC_LAST_SEGMENT |
			   TX_DESC_FIRST_SEGMENT |
			   TX_DESC_ENABLE_CRC |
			   TX_DESC_ENABLE_PAD |
			   TX_DESC_TX_TIMESTAMP_DISABLED |
			   TX_DESC_CHECKSUM_INSERTION_DISABLED |
			   TX_DESC_TX_END_OF_RING |
			   TX_DESC_SECOND_ADDR_NOT_CHAINED);

	DMATxDesc->ControlAndStatus = tempreg;

	DMATxDesc->BufSize = 0x0;

	/* Set buffers addresses to addr of globally initialized and aligned buffers */
	DMATxDesc->Buf1Addr = (uint32_t)&Txbuff[0];
	DMATxDesc->Buf2NextDescAddr = (uint32_t)&Txbuff[1];

	/* Set the address of Tx descriptor in DMA register */
	ETH->DMATDLAR = (uint32_t)DMATxDesc;
}


void ETH_DMAPrepareTxDesc(ETH_TxDescriptor *DMATxDesc, uint16_t Framelength)
{
	/* Check if the descriptor is owned by DMA or CPU, if by CPU then continue */
	if((DMATxDesc->ControlAndStatus & TX_DESC_OWN) != 0)
	{
		/* TODO add delay on Systick or return some status */
		BSP_Delay_ms(1);
	}

	/* Set the exact size of frame to be transferred */
	DMATxDesc->BufSize = (Framelength & TX_DESC_BUF1SIZE_MAX);

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


void ETH_DMARxDescInit(ETH_RxDescriptor *DMARxDesc)
{
	uint32_t tempreg = 0U;

	/* Status */
	tempreg = (RX_DESC_FIRST_DESCRIPTOR | RX_DESC_LAST_DESCRIPTOR | RX_DESC_OWN);

	DMARxDesc->Status |= tempreg;

	/* BufSizeAndControl */
	tempreg = (RX_DESC_INT_ON_COMPLETION_ENABLED |
			   RX_DESC_BUF2SIZE(RX_BUF_SIZE) |
			   RX_DESC_RX_END_OF_RING |
			   RX_DESC_SECOND_ADDR_NOT_CHAINED |
			   RX_DESC_BUF1SIZE(RX_BUF_SIZE));

	DMARxDesc->BufSizeAndControl = tempreg;

	DMARxDesc->Buf1Addr = (uint32_t)&Rxbuff[0];
	DMARxDesc->Buf2NextDescAddr = (uint32_t)&Rxbuff[1];

	ETH->DMARDLAR = (uint32_t)DMARxDesc;
}


__attribute__ ((weak)) void ETH_RxCallback(void)
{
}


void ETH_IRQHandler(void)
{
	if ((ETH->DMASR & RECEIVE_FINISHED) == RECEIVE_FINISHED)
	{
		ETH_RxCallback();
		/* Clear receive pending interrupt bit */
		ETH->DMASR |= RECEIVE_FINISHED;
	}
	/* Clear normal interrupt summary bit */
	ETH->DMASR |= NORMAL_INTERRUPT_SUMMARY;
}


void SYSCFG_SelectRMII(void)
{
	SYSCFG->PMC |= ETH_RMII_SELECT;
}
