/*
 * CAN.h
 *
 *  Created on: 22 Apr 2020
 *      Author: wsadzik
 */

#ifndef CAN_H_
#define CAN_H_

#include "stm32f4xx.h"
#include "interface.h"

#define CAN_TX_PIN 9
#define CAN_RX_PIN 8

typedef enum {
	CAN_RECEIVED = 0x00U,
	CAN_NOMESSAGE = 0x01U
} CAN_RXState;

typedef struct {
	uint32_t DebugFreeze;

	uint32_t TimeTriggeredCom;

	uint32_t AutoBusOff;

	uint32_t AutoWakeup;

	uint32_t NoAutoRetransmission;

	uint32_t RXFifoLocked;

	uint32_t TXFifoPrio;

	uint32_t SilentMode;

	uint32_t LoopBackMode;

	uint32_t SJW;

	uint32_t TimeSeg2;

	uint32_t TimeSeg1;

	uint32_t BaudratePrescaler;

	double SamplePoint;

} CAN_Init;

typedef struct {
	uint32_t FilterNum;

	uint32_t FilterIDH;

	uint32_t FilterIDL;

	uint32_t FilterMaskH;

	uint32_t FilterMaskL;

} CAN_Filter;

typedef struct {
	uint32_t ID;

	uint32_t IDE;

	uint8_t DLC;

	uint8_t Data[8];

} CAN_Mailbox;

/* ================================== */
/*  CAN control and status registers  */
/* ================================== */

/* MCR */
#define CAN_DEBUG_DONT_FREEZE 			 ((uint32_t)(0U << 16))
#define CAN_RESET 						 ((uint32_t)(1U << 15))
#define CAN_TIME_TRIGGERED_MODE_DISABLED ((uint32_t)(0U << 7))
#define CAN_AUTO_BUSOFF_MANAGEMENT 		 ((uint32_t)(1U << 6))
#define CAN_AUTO_WAKEUP_ENABLED 		 ((uint32_t)(1U << 5))
#define CAN_AUTO_RETRANSMISSION_ENABLED  ((uint32_t)(0U << 4))
#define CAN_RX_FIFO_LOCKED 				 ((uint32_t)(1U << 3))
#define CAN_TX_FIFO_PRIO_BY_ID 			 ((uint32_t)(0U << 2))
#define CAN_TX_FIFO_PRIO_BY_REQUEST 	 ((uint32_t)(1U << 2))
#define CAN_SLEEP_MODE					 ((uint32_t)(1U << 1))
#define CAN_NORMAL_MODE_REQUEST 		 ((uint32_t)(0U << 0))
#define CAN_INIT_MODE_REQUEST 			 ((uint32_t)(1U << 0))

/* MSR */
#define CAN_NORMAL_MODE_ENABLED 		 ((uint32_t)(0U << 0))
#define CAN_INIT_MODE_ENABLED 			 ((uint32_t)(1U << 0))

/* TSR */
#define CAN_TX_MAILBOX2_EMPTY 			 ((uint32_t)(1U << 28))
#define CAN_TX_MAILBOX1_EMPTY 			 ((uint32_t)(1U << 27))
#define CAN_TX_MAILBOX0_EMPTY 			 ((uint32_t)(1U << 26))
#define IS_ANY_TX_MAILBOX_EMPTY 		 (CAN1->TSR & (CAN_TX_MAILBOX0_EMPTY | \
													   CAN_TX_MAILBOX1_EMPTY | \
													   CAN_TX_MAILBOX2_EMPTY))
#define CAN_TX_MAILBOX_RQ_COMPLETED(x) 	 ((uint32_t)(1U << (x*8)))

/* RF0R */
#define _CAN_RELEASE_FIFO0				 (CAN1->RF0R |= (uint32_t)(1U << 5))
#define _CAN_RELEASE_FIFO1				 (CAN1->RF1R |= (uint32_t)(1U << 5))
#define IS_ANY_MSG_PENDING_FIFO0		 (CAN1->RF0R & (uint32_t)0b11)

/* IER */
#define CAN_FIFO0_MSG_PENDING_INTERRUPT_ENABLE 	 ((uint32_t)(1U << 1))
#define _CAN_FIFO0_MSG_PENDING_INTERRUPT_ENABLE  (CAN1->IER |= CAN_FIFO0_MSG_PENDING_INTERRUPT_ENABLE)
#define _CAN_FIFO0_MSG_PENDING_INTERRUPT_DISABLE (CAN1->IER &= ~CAN_FIFO0_MSG_PENDING_INTERRUPT_ENABLE)

/* BTR */
#define USE_STRUCT_VALUES				 ((uint32_t)0U)
#define SAMPLE_POINT_DEFAULT 			 ((double)0.875)
#define SEG1_MAX 						 ((double)16U)
#define TQ_QUANTITY_MAX 				 ((double)(1U + 16U + 8U))
#define TQ_QUANTITY_MIN 				 ((double)(1U + 1U + 1U))
#define APB_CLOCK_SPEED					 ((uint32_t)(SystemCoreClock/4))

#define CAN_SILENT_MODE_ENABLED 		 ((uint32_t)(1U << 31))
#define CAN_SILENT_MODE_DISABLED 		 ((uint32_t)(0U << 31))
#define CAN_LOOPBACK_MODE_ENABLED 		 ((uint32_t)(1U << 30))
#define CAN_LOOPBACK_MODE_DISABLED 		 ((uint32_t)(0U << 30))
#define CAN_PRESCALER(x)				 ((uint32_t)((uint32_t)(x-1) << 0))
#define CAN_SJW(x) 					 	 ((uint32_t)((uint32_t)(x-1) << 24))
#define CAN_TSEG2(x) 					 ((uint32_t)((uint32_t)(x-1) << 20))
#define CAN_TSEG1(x)				 	 ((uint32_t)((uint32_t)(x-1) << 16))

/* ======================= */
/*  CAN mailbox registers  */
/* ======================= */
#define CAN_FIFO_0					((uint8_t)0U)
#define CAN_FIFO_1 					((uint8_t)1U)
#define CAN_MAX_DLC					((uint8_t)8U)

/* TIR */
#define CAN_TX_MAILBOX_IDE 			((uint32_t)(1U << 2))
#define CAN_TX_MAILBOX_RTR			((uint32_t)(1U << 1))
#define CAN_TX_MAILBOX_TXRQ 		((uint32_t)(1U << 0))

#define _CAN_TX_MAILBOX_SET_IDE(x) 	(CAN1->sTxMailBox[x].TIR |= CAN_TX_MAILBOX_IDE)
#define IS_TX_MAILBOX_EXTID(x) 		((CAN1->sTxMailBox[x].TIR & CAN_TX_MAILBOX_IDE) == CAN_TX_MAILBOX_IDE)
#define _CAN_TX_MAILBOX_SEND(x)		(CAN1->sTxMailBox[x].TIR |= CAN_TX_MAILBOX_TXRQ)

/* RIR */
#define CAN_RX_MAILBOX_IDE 			((uint32_t)(1U << 2))
#define IS_RX_MAILBOX_EXTID(x) 		((CAN1->sFIFOMailBox[x].RIR & CAN_RX_MAILBOX_IDE) == CAN_RX_MAILBOX_IDE)

/* ====================== */
/*  CAN filter registers  */
/* ====================== */

/* FMR */
#define CAN_FILTER_INIT_MODE 	((uint32_t)(1U << 0))
#define CAN_START_BANK_MASK 	((uint32_t)0x00003F00U)
#define CAN_START_BANK_DEFAULT 	((uint32_t)0x0FU)

#define _CAN1_FILTER_INIT  		(CAN1->FMR |= CAN_FILTER_INIT_MODE)
#define _CAN1_FILTER_LEAVE_INIT (CAN1->FMR &= ~CAN_FILTER_INIT_MODE)

/* ======= */
/*  Other  */
/* ======= */
#define _RCC_CAN1_ENABLE (RCC->APB1ENR |= (uint32_t)(1U << 25))
#define _RCC_CAN2_ENABLE (RCC->APB1ENR |= (uint32_t)(1U << 26))

/* ===================== */
/*  Function prototypes  */
/* ===================== */
void CAN1_Init(uint32_t baudrate);
void CAN1_LowLevelInit(void);
void CAN_StructInit(CAN_Init *caninit);
void CAN_UserStructInit(CAN_Init *caninit);
void CAN1_Config(CAN_Init *caninit);
void CAN1_SetBitTime(uint32_t baudrate, CAN_Init *caninit);
void CAN_CalculateBitTime(uint32_t baudrate, CAN_Init *caninit);
void CAN1_Reset(void);
void CAN1_FilterConfig(CAN_Filter *filter);
void CAN1_SetDLC(struct Interface *interface, uint16_t len);
void CAN1_PrepareMsg(struct Interface *interface, uint8_t *data);
void CAN1_Send(CAN_Mailbox *txmsg);
CAN_RXState CAN1_Receive(CAN_Mailbox *rxmsg, uint8_t fifonum);
void CAN1_RX0Callback(void);

#endif /* CAN_H_ */
