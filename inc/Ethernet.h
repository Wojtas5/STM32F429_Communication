/*
 * Ethernet.h
 *
 *  Created on: Apr 2, 2020
 *      Author: wsadzik
 */

#ifndef ETHERNET_H_
#define ETHERNET_H_

#include "stdint.h"

typedef enum {
	MAC0,
	MAC1,
	MAC2,
	MAC3
} ETH_MACAddr;


typedef struct {
	uint16_t LoopbackMode;

	uint16_t Speed;

	uint16_t Autonegotiation;

	uint16_t DuplexMode;

} ETH_PHYInit;


typedef struct {
	uint32_t CRCstrip;

	uint32_t Watchdog;

	uint32_t Jabber;

	uint32_t InterFrameGap;

	uint32_t CarrierSense;

	uint32_t Speed;

	uint32_t ReceiveOwn;

	uint32_t LoopbackMode;

	uint32_t DuplexMode;

	uint32_t IPv4Checksum;

	uint32_t RetryTransmission;

	uint32_t AutomaticPADCRCStrip;

	uint32_t BackoffLimit;

	uint32_t DeferralCheck;

	uint32_t ReceiveAll;

	uint32_t BroadcastFramesFilter;

	uint32_t SourceAddressFilter;

	uint32_t PassControlFrames;

	uint32_t PromiscuousMode;

	uint32_t PauseTime;

	uint32_t ZeroquantaPause;

	uint32_t PauseLowThreshold;

	uint32_t ReceiveFlowControl;

	uint32_t TransmitFlowControl;

	uint32_t ActivateFlowControl;

	uint32_t VLANTagComparison;

	uint32_t VLANTagIdentifier;

} ETH_MACInit;


typedef struct {
	uint32_t MixedBurst;

	uint32_t AddressAlignedBeats;

	uint32_t PBLx4Mode;

	uint32_t UseSeparatePBL;

	uint32_t RxDMAPBL;

	uint32_t FixedBurst;

	uint32_t RxTxPriorityRatio;

	uint32_t PBL;

	uint32_t EnchancedDescriptorFormat;

	uint32_t DescriptorSkipLenght;

	uint32_t DMAArbitration;

	uint32_t TCPIPChecksumErrorFramesDrop;

	uint32_t ReceiveStoreAndForward;

	uint32_t ReceivedFramesFlushing;

	uint32_t TransmitStoreAndForward;

	uint32_t TransmitThresholdControl;

	uint32_t ForwardErrorFrames;

	uint32_t ForwardUndersizedFrames;

	uint32_t ReceiveThresholdControl;

	uint32_t OperateOnSecondFrame;

} ETH_DMAInit;


typedef struct {
	uint32_t ControlAndStatus;

	uint32_t BufSize;

	uint32_t Buf1Addr;

	uint32_t Buf2NextDescAddr; // Address of second buffer or next descriptor
#ifdef USE_ENCHANCED_DESCRIPTORS
	uint32_t Reserved[2];

	uint32_t TimeStampLow;

	uint32_t TimeStampHigh;
#endif

} ETH_TxDescriptor;


typedef struct {
	uint32_t Status;

	uint32_t BufSizeAndControl;

	uint32_t Buf1Addr;

	uint32_t Buf2NextDescAddr;
#ifdef USE_ENCHANCED_DESCRIPTORS
	uint32_t ExtendedStatus;

	uint32_t Reserved;

	uint32_t TimeStampLow;

	uint32_t TimeStampHigh;
#endif

} ETH_RxDescriptor ;


struct ETH_Header {
	uint8_t DestinationAddress[6];

	uint8_t SourceAddress[6];

	uint16_t Type;

} __attribute__ ((packed));

#define IPV4_TYPE 0x0800U
#define ARP_TYPE  0x0806U

/* ============================ */
/*  Descriptors related macros  */
/* ============================ */
#define ETH_MAX_PACKET_SIZE 1524U

#define TX_BUF_SIZE ETH_MAX_PACKET_SIZE
#define RX_BUF_SIZE ETH_MAX_PACKET_SIZE

#define TX_DESCRIPTORS 3U
#define RX_DESCRIPTORS 3U

/* ETH_TxDescriptor */
#define TX_DESC_OWN 						((uint32_t)(1U << 31))
#define TX_DESC_INT_ON_COMPLETION_ENABLED 	((uint32_t)(1U << 30))
#define TX_DESC_INT_ON_COMPLETION_DISABLED 	((uint32_t)(0U << 30))
#define TX_DESC_LAST_SEGMENT			    ((uint32_t)(1U << 29))
#define TX_DESC_FIRST_SEGMENT			    ((uint32_t)(1U << 28))
#define TX_DESC_DISABLE_CRC 				((uint32_t)(1U << 27))
#define TX_DESC_ENABLE_CRC 					((uint32_t)(0U << 27))
#define TX_DESC_DISABLE_PAD 				((uint32_t)(1U << 26))
#define TX_DESC_ENABLE_PAD 					((uint32_t)(0U << 26))
#define TX_DESC_TX_TIMESTAMP_ENABLED	    ((uint32_t)(1U << 25))
#define TX_DESC_TX_TIMESTAMP_DISABLED	    ((uint32_t)(0U << 25))
#define TX_DESC_CHECKSUM_INSERTION_DISABLED ((uint32_t)(0b00U << 22))
#define TX_DESC_CHECKSUM_IPHEADER_ENABLED   ((uint32_t)(0b01U << 22))
#define TX_DESC_TX_END_OF_RING			    ((uint32_t)(1U << 21))
#define TX_DESC_SECOND_ADDR_CHAINED		    ((uint32_t)(1U << 20))
#define TX_DESC_SECOND_ADDR_NOT_CHAINED		((uint32_t)(0U << 20))
#define TX_DESC_IPHEADER_ERROR				((uint32_t)(1U << 16))
#define TX_DESC_ERROR_SUMMARY			    ((uint32_t)(1U << 15))
#define TX_DESC_BUF1SIZE(x)					((uint32_t)(x << 0))
#define TX_DESC_BUF2SIZE(x)					((uint32_t)(x << 16))

#define TX_DESC_BUF1SIZE_MAX                ((uint32_t)0x00001FFF)
#define TX_DESC_BUF2SIZE_MAX                ((uint32_t)0x1FFF0000)

/* ETH_RxDescriptor */
#define RX_DESC_OWN 					  ((uint32_t)(1U << 31))
#define RX_FRAMELENGTH_MAX 				  ((uint32_t)0x3FFF0000U)
#define RX_FRAMELENGTH_OFFSET			  16U
#define RX_DESC_FIRST_DESCRIPTOR		  ((uint32_t)(1U << 9))
#define RX_DESC_LAST_DESCRIPTOR		   	  ((uint32_t)(1U << 8))
#define RX_DESC_INT_ON_COMPLETION_ENABLED ((uint32_t)(0U << 31))
#define RX_DESC_BUF2SIZE(x)				  ((uint32_t)(x << 16))
#define RX_DESC_RX_END_OF_RING			  ((uint32_t)(1U << 15))
#define RX_DESC_SECOND_ADDR_CHAINED		  ((uint32_t)(1U << 14))
#define RX_DESC_SECOND_ADDR_NOT_CHAINED	  ((uint32_t)(0U << 14))
#define RX_DESC_BUF1SIZE(x)			      ((uint32_t)(x << 0))

/* ============================== */
/*  MAC registers related macros  */
/* ============================== */

/* MACCR */
#define CRC_STRIPPING_ENABLED         ((uint32_t)(1U << 25))
#define WATCHDOG_ENABLED              ((uint32_t)(0U << 23))
#define JABBER_ENABLED                ((uint32_t)(0U << 22))
#define INTERFRAME_GAP_96BIT          ((uint32_t)(0b000U << 17))
#define CARRIER_SENSE_ENABLED         ((uint32_t)(0U << 16))
#define ETHERNET_SPEED_10M            ((uint32_t)(0U << 14))
#define ETHERNET_SPEED_100M           ((uint32_t)(1U << 14))
#define RECEIVE_OWN_ENABLED           ((uint32_t)(0U << 13))
#define LOOPBACK_MODE_ENABLED         ((uint32_t)(1U << 12))
#define FULLDUPLEX_MODE               ((uint32_t)(1U << 11))
#define HALFDUPLEX_MODE               ((uint32_t)(0U << 11))
#define IPV4_CHECKSUM_ENABLED         ((uint32_t)(1U << 10))
#define RETRY_TRANSMISSION_ENABLED    ((uint32_t)(0U << 9))
#define AUTO_PADCRC_STRIPPING_ENABLED ((uint32_t)(1U << 7))
#define BACKOFF_LIMIT_10              ((uint32_t)(0b00U << 5))
#define DEFERRAL_CHECK_ENABLED        ((uint32_t)(1U << 4))
#define TRANSMITTER_ENABLED           ((uint32_t)(1U << 3))
#define RECEIVER_ENABLED              ((uint32_t)(1U << 2))

/* MACFFR */
#define RECEIVEALL_ENABLED                         ((uint32_t)(1U << 31))
#define HASH_OR_PERFECT_FILTER_ENABLED             ((uint32_t)(1U << 10))
#define HASHFILTER_ENABLED                         ((uint32_t)(0U << 10))
#define SOURCE_ADDRESS_FILTER_ENABLED              ((uint32_t)(1U << 9))
#define PASS_CONTROL_FRAMES_BLOCKALL               ((uint32_t)(0b00U << 6))
#define PASS_CONTROL_FRAMES_PASSALL                ((uint32_t)(0b10U << 6))
#define PASS_CONTROL_FRAMES_PASSEDADDRESSFILTER    ((uint32_t)(0b11U << 6))
#define BROADCAST_FRAMES_FILTER_ENABLED            ((uint32_t)(1U << 5))
#define PASS_ALL_MULTICAST                         ((uint32_t)(1U << 4))
#define HASH_MULTICAST_FRAMES_FILTERING_ENABLED    ((uint32_t)(1U << 2))
#define PERFECT_MULTICAST_FRAMES_FILTERING_ENABLED ((uint32_t)(0U << 2))
#define HASH_UNICAST_FRAMES_FILTERING_ENABLED      ((uint32_t)(1U << 1))
#define PERFECT_UNICAST_FRAMES_FILTERING_ENABLED   ((uint32_t)(0U << 1))
#define PROMISCUOUS_MODE_ENABLED                   ((uint32_t)(1U << 0))

/* MACMIIAR */
#define ETH_CR_DIV16    ((uint32_t)(0b010U << 2))
#define ETH_CR_DIV26	((uint32_t)(0b011U << 2))
#define ETH_CR_DIV42	((uint32_t)(0b000U << 2))
#define ETH_CR_DIV62	((uint32_t)(0b001U << 2))
#define ETH_CR_DIV102	((uint32_t)(0b100U << 2))
#define ETH_CR_MASK     ((uint32_t)0xFFFFFFE3U)
#define ETH_MW_READ     ((uint32_t)(0U << 1))
#define ETH_MW_WRITE    ((uint32_t)(1U << 1))
#define ETH_MB_BUSY     ((uint32_t)(1U << 0))

/* MACFCR */
#define PAUSE_TIME(x)                         ((uint32_t)(x << 16))
#define ZEROQUANTA_PAUSE_ENABLED              ((uint32_t)(0U << 7))
#define ZEROQUANTA_PAUSE_DISABLED             ((uint32_t)(1U << 7))
#define PAUSE_TIME_THRESHOLD_MINUS4           ((uint32_t)(0b00U << 4))
#define UNICAST_PAUSE_FRAME_DETECTION_ENABLED ((uint32_t)(1U << 3))
#define RECEIVE_FLOW_CONTROL_ENABLED          ((uint32_t)(1U << 2))
#define TRANSMIT_FLOW_CONTROL_ENABLED         ((uint32_t)(1U << 1))
#define FLOW_CONTROL_BUSY                     ((uint32_t)(1U << 0))

/* MACVLANTR */
#define VLAN_TAG_16BIT ((uint32_t)(0U << 16))
#define VLAN_TAG_12BIT ((uint32_t)(1U << 16))

/* MACA0HR - MACA3LR */
#define MAC_ADDR0HR_MO   ((uint32_t)(1U << 31))
#define MAC_ADDR1_ENABLE ((uint32_t)(1U << 31))
#define MAC_ADDR2_ENABLE ((uint32_t)(1U << 31))
#define MAC_ADDR3_ENABLE ((uint32_t)(1U << 31))

/* ============================== */
/*  DMA registers related macros  */
/* ============================== */

/* DMABMR */
#define MIXED_BURST_ENABLED           ((uint32_t)(1U << 26))
#define ADDRESS_ALIGNED_BEATS_ENABLED ((uint32_t)(1U << 25))
#define PBL_X4_MODE_ENABLED           ((uint32_t)(1U << 24))
#define SEPARATE_PBL_ENABLED          ((uint32_t)(1U << 23))
#define FIXED_BURST_ENABLED           ((uint32_t)(1U << 16))
#define RX_TX_PRIORITY_1_1            ((uint32_t)(0b00U << 14))
#define PBL_32                        ((uint32_t)(1U << 13))
#define ENCHANCED_DESC_FORMAT_ENABLED ((uint32_t)(1U << 7))
#define RX_PRIOTITY                   ((uint32_t)(1U << 1))
#define ROUND_ROBIN_PRIORITY          ((uint32_t)(0U << 1))
#define ETH_RESET                     ((uint32_t)(1U << 0))

/* DMAOMR */
#define TCPIP_CHECKSUM_ERROR_FRAMES_DROPPING_ENABLED ((uint32_t)(0U << 26))
#define RECEIVE_STORE_AND_FORWARD_ENABLED            ((uint32_t)(1U << 25))
#define RECEIVED_FRAMES_FLUSHING_ENABLED             ((uint32_t)(0U << 24))
#define RECEIVED_FRAMES_FLUSHING_DISABLED            ((uint32_t)(1U << 24))
#define TRANSMIT_STORE_AND_FORWARD_ENABLED           ((uint32_t)(1U << 21))
#define FLUSH_TRANSMIT_FIFO      				     ((uint32_t)(1U << 20))
#define TRANSMIT_THRESHOLD_64    				     ((uint32_t)(0b000U << 14))
#define START_DMA_TRANSMISSION     				     ((uint32_t)(1U << 13))
#define FORWARD_ERROR_FRAMES_ENABLED    		     ((uint32_t)(1U << 7))
#define FORWARD_UNDERSIZED_FRAMES_ENABLED  		     ((uint32_t)(1U << 6))
#define RECEIVE_THRESHOLD_64        				 ((uint32_t)(0b00U << 3))
#define OPERATE_ON_SECOND_FRAME_ENABLED  		     ((uint32_t)(1U << 2))
#define START_DMA_RECEIVE      						 ((uint32_t)(1U << 1))

/* DMASR */
#define NORMAL_INTERRUPT_SUMMARY 	((uint32_t)(1U << 16))
#define RECEIVE_BUFFER_UNAVAILABLE  ((uint32_t)(1U << 7))
#define RECEIVE_FINISHED 		 	((uint32_t)(1U << 6))
#define TRANSMIT_BUFFER_UNAVAILABLE ((uint32_t)(1U << 2))
#define TRANSMIT_FINISHED           ((uint32_t)(1U << 0))

/* DMAIER */
#define NORMAL_INTERRUPT_ENABLE 			   ((uint32_t)(1U << 16))
#define RECEIVE_INTERRUPT_ENABLE 			   ((uint32_t)(1U << 6))
#define TX_BUFFER_UNAVAILABLE_INTERRUPT_ENABLE ((uint32_t)(1U << 2))
#define TRANSMIT_INTERRUPT_ENABLE 			   ((uint32_t)(1U << 0))

/* ============================== */
/*  PHY registers related macros  */
/* ============================== */

/* PHY Registers Addresses */
#define PHY_BCR         ((uint16_t)0x0000U)
#define PHY_BSR			((uint16_t)0x0001U)
#define PHY_SR			((uint16_t)0x001FU)

/* PHY_BCR */
#define PHY_SOFTRESET_DELAY          ((uint16_t)500U)
#define PHY_SOFTRESET                ((uint16_t)(1U << 15))
#define PHY_LOOPBACKMODE_ENABLED     ((uint16_t)(1U << 14))
#define PHY_SPEED_10M                ((uint16_t)(0U << 13))
#define PHY_SPEED_100M 			     ((uint16_t)(1U << 13))
#define PHY_AUTONEGOTIATION_ENABLED  ((uint16_t)(1U << 12))
#define PHY_RESTART_AUTONEGOTIATION  ((uint16_t)(1U << 9))
#define PHY_HALFDUPLEX_MODE          ((uint16_t)(0U << 8))
#define PHY_FULLDUPLEX_MODE 	     ((uint16_t)(1U << 8))

/* PHY_BSR */
#define PHY_AUTONEGOTIATION_COMPLETE ((uint16_t)(1U << 5))
#define PHY_AUTONEGOTIATION_ABILITY  ((uint16_t)(1U << 3))
#define PHY_LINK_STATUS              ((uint16_t)(1U << 2))
#define PHY_JABBER_DETECTED          ((uint16_t)(1U << 1))

/* PHY_SR */
#define PHY_10M		   ((uint16_t)(0b01U << 2))
#define PHY_100M  	   ((uint16_t)(0b10U << 2))
#define PHY_HALFDUPLEX ((uint16_t)(0U << 4))
#define PHY_FULLDUPLEX ((uint16_t)(1U << 4))

/* ================================ */
/*  Other registers related macros  */
/* ================================ */

/* RCC_AHB1ENR */
#define ETHMAC_ENABLE   ((uint32_t)(1U << 25))
#define ETHMACTX_ENABLE ((uint32_t)(1U << 26))
#define ETHMACRX_ENABLE ((uint32_t)(1U << 27))

/* SYSCFG_PMC */
#define ETH_RMII_SELECT ((uint32_t)(1U << 23))


/* ===================== */
/*  Function prototypes  */
/* ===================== */
void Ethernet_Init(void);
void Ethernet_LowLevelInit(void);
void ETH_HeaderStructInit(struct ETH_Header *ethhdr, uint8_t *destaddr, uint8_t * srcaddr);
void ETH_MACStructInit(ETH_MACInit *macinit, ETH_PHYInit *phyinit);
void ETH_MACUserStructInit(ETH_MACInit *macinit);
void ETH_MACConfig(ETH_MACInit *macinit);
void ETH_DMAStructInit(ETH_DMAInit *dmainit);
void ETH_DMAUserStructInit(ETH_DMAInit *dmainit);
void ETH_DMAConfig(ETH_DMAInit *dmainit);
void ETH_SetMACAddress(ETH_MACAddr MAC ,uint8_t *addr);
void ETH_MACTxEnable(void);
void ETH_MACTxDisable(void);
void ETH_MACRxEnable(void);
void ETH_MACRxDisable(void);
void ETH_DMATxEnable(void);
void ETH_DMATxDisable(void);
void ETH_DMARxEnable(void);
void ETH_DMARxDisable(void);
void ETH_DMAFlushTxFIFO(void);
void ETH_Start(void);
void ETH_Stop(void);
void ETH_PHYSetMDCClock(void);
void ETH_MACReset(void);
void ETH_PHYWrite(uint16_t PHYReg, uint16_t PHYRegValue);
void ETH_PHYRead(uint16_t PHYReg, uint16_t *PHYRegValue);
void ETH_DMATxDescInit(ETH_TxDescriptor *DMATxDesc);
void ETH_DMAPrepareTxDesc(ETH_TxDescriptor *DMATxDesc, uint16_t Framelength);
void ETH_DMARxDescInit(ETH_RxDescriptor *DMARxDesc);
void ETH_RxCallback(void);


/* Other */
void SYSCFG_SelectRMII(void);

/* Exported variables and structures */
extern ETH_TxDescriptor DMATxDesc[TX_DESCRIPTORS];
extern ETH_RxDescriptor DMARxDesc[RX_DESCRIPTORS];
extern uint8_t Txbuff[TX_DESCRIPTORS*2][TX_BUF_SIZE];
extern uint8_t Rxbuff[RX_DESCRIPTORS*2][RX_BUF_SIZE];

extern ETH_MACInit MACInit;
extern ETH_DMAInit DMAInit;


#endif /* ETHERNET_H_ */
