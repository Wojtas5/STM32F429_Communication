/*
 * CAN.c
 *
 *  Created on: 22 Apr 2020
 *      Author: wsadzik
 */

#include "stm32f4xx.h"
#include "bsp.h"
#include "CAN.h"
#include "interface.h"
#include "string.h"

void CAN1_Init(uint32_t baudrate)
{
	CAN_Init CAN1Init;

	/* Initialization of Clock, GPIO and NVIC */
	CAN1_LowLevelInit();

	/* Reset CAN1 peripheral */
	CAN1_Reset();

	/* Wait for reset bit to be cleared */
	while((CAN1->MCR & CAN_RESET) == CAN_RESET);

	/* Exit sleep mode */
	CAN1->MCR &= ~CAN_SLEEP_MODE;

	/* Enter initialization mode */
	CAN1->MCR |= CAN_INIT_MODE_REQUEST;

	/* Wait for INAK flag to be set */
	while((CAN1->MCR & CAN_INIT_MODE_ENABLED) !=  CAN_INIT_MODE_ENABLED);

	/* Initialize configuration structure */
	CAN_StructInit(&CAN1Init);

	/* Configure CAN1 registers with values set in configuration structure */
	CAN1_Config(&CAN1Init);

	/* Set CAN1 bit time */
	CAN1_SetBitTime(baudrate, &CAN1Init);

	/* Enter normal mode */
	CAN1->MCR &= ~CAN_INIT_MODE_REQUEST;

	/* Wait for INAK flag to be cleared */
	while((CAN1->MCR & CAN_INIT_MODE_ENABLED) ==  CAN_INIT_MODE_ENABLED);
}

void CAN1_LowLevelInit(void)
{
	/* Enable clock for GPIOs */
	RCC->AHB1ENR |= GPIOB_ENABLE;

	/* CAN pins configuration */
	GPIOB->MODER   |= (PIN8_AF | PIN9_AF);
	GPIOB->OSPEEDR |= (PIN8_HS | PIN9_HS);
	GPIOB->PUPDR   |= (PIN8_PULLUP | PIN9_PULLUP);
	GPIOB->AFR[1]  |= (PIN8_CAN | PIN9_CAN);

	/* Enable CAN interrupts and set priority */
	NVIC_SetPriority(CAN1_RX0_IRQn, 3U);
	NVIC_EnableIRQ(CAN1_RX0_IRQn);

	/* Enable clock for CAN1 peripheral */
	_RCC_CAN1_ENABLE;
}

void CAN_StructInit(CAN_Init *caninit)
{
	caninit->DebugFreeze = CAN_DEBUG_DONT_FREEZE;
	caninit->TimeTriggeredCom = CAN_TIME_TRIGGERED_MODE_DISABLED;
	caninit->AutoBusOff = CAN_AUTO_BUSOFF_MANAGEMENT;
	caninit->AutoWakeup = CAN_AUTO_WAKEUP_ENABLED;
	caninit->NoAutoRetransmission = CAN_AUTO_RETRANSMISSION_ENABLED;
	caninit->RXFifoLocked = CAN_RX_FIFO_LOCKED;
	caninit->TXFifoPrio = CAN_TX_FIFO_PRIO_BY_REQUEST;
	caninit->SilentMode = CAN_SILENT_MODE_DISABLED;
	caninit->LoopBackMode = CAN_LOOPBACK_MODE_DISABLED;
	caninit->SJW = CAN_SJW(1U);
	caninit->SamplePoint = SAMPLE_POINT_DEFAULT;

	CAN_UserStructInit(caninit);
}

__attribute__ ((weak)) void CAN_UserStructInit(CAN_Init *caninit)
{
}

void CAN1_Config(CAN_Init *caninit)
{
	uint32_t tempreg = 0U;

	tempreg = (caninit->DebugFreeze |
			   caninit->TimeTriggeredCom |
			   caninit->AutoBusOff |
			   caninit->AutoWakeup |
			   caninit->NoAutoRetransmission |
			   caninit->RXFifoLocked |
			   caninit->TXFifoPrio |
			   CAN_INIT_MODE_REQUEST);

	CAN1->MCR = tempreg;
}

void CAN1_SetBitTime(uint32_t baudrate, CAN_Init *caninit)
{
	if(USE_STRUCT_VALUES != baudrate)
	{
		CAN_CalculateBitTime(baudrate, caninit);
	}

	CAN1->BTR = (caninit->SilentMode |
				 caninit->LoopBackMode |
				 caninit->SJW |
				 caninit->TimeSeg1 |
				 caninit->TimeSeg2 |
				 caninit->BaudratePrescaler);
}

/* Function for calculating time quantas quantity, length of Seg1 and Seg2 based on Sample Point */
void CAN_CalculateBitTime(uint32_t baudrate, CAN_Init *caninit)
{
	double closediff = 1.0;
	double calculatedSamplePoint = 0U;
	double seg1 = SEG1_MAX; // We use a value of seg1 with added 1tq of propagation segment
	double alltq = TQ_QUANTITY_MAX;
	double nominalBitTime = 1.0 / baudrate;
	double timequanta, prescaler;
	uint8_t closealltq, closeseg1;

	/* Iterate through all combinations to find the closest sample point */
	for(int i = 0; i < SEG1_MAX; ++i)
	{
		do
		{
			calculatedSamplePoint = seg1 / alltq;

			if(calculatedSamplePoint >= caninit->SamplePoint)
			{
				if((calculatedSamplePoint - caninit->SamplePoint) < closediff)
				{
					closediff = calculatedSamplePoint - caninit->SamplePoint;
					closealltq = alltq;
					closeseg1 = seg1;
				}
			}

			else
			{
				if((caninit->SamplePoint - calculatedSamplePoint) < closediff)
				{
					closediff = caninit->SamplePoint - calculatedSamplePoint;
					closealltq = alltq;
					closeseg1 = seg1;
				}
			}
			--alltq;

		} while(alltq > TQ_QUANTITY_MIN);

		--seg1;
		alltq = TQ_QUANTITY_MAX;
	}

	timequanta = nominalBitTime / closealltq;
	prescaler = timequanta * APB_CLOCK_SPEED;

	/* Round to closest integer */
	prescaler += 0.5;
	prescaler = (uint8_t)prescaler;

	caninit->BaudratePrescaler = (uint32_t)CAN_PRESCALER(prescaler);
	caninit->TimeSeg1 = (uint32_t)CAN_TSEG1(closeseg1-1U); // We substract 1tq, because here we need a value without propagation segment
	caninit->TimeSeg2 = (uint32_t)CAN_TSEG2(closealltq-closeseg1);
}

void CAN1_Reset(void)
{
	CAN1->MCR |= CAN_RESET;
}

void CAN1_FilterConfig(CAN_Filter *filter)
{
	uint32_t filterpos = ((uint32_t)(1U << filter->FilterNum));

	/* Enter initialization mode for filters */
	_CAN1_FILTER_INIT;

	/* Set start bank */
	CAN1->FMR &= ~CAN_START_BANK_MASK;
	CAN1->FMR |= CAN_START_BANK_DEFAULT;

	/* Deactivate filter */
	CAN1->FA1R &= ~filterpos;

	/* Select 16-bit scale for the filter */
	CAN1->FS1R &= ~filterpos;

	/* Select mask filter mode */
	CAN1->FM1R &= ~filterpos;

	/* Enter filter ID and mask values */
	CAN1->sFilterRegister[filter->FilterNum].FR1 = ((filter->FilterMaskL << 16) | (filter->FilterIDL));
	CAN1->sFilterRegister[filter->FilterNum].FR2 = ((filter->FilterMaskH << 16) | (filter->FilterIDH));

	/* Assign filter for FIFO0 */
	CAN1->FFA1R &= ~filterpos;

	/* Activate filter */
	CAN1->FA1R |= filterpos;

	/* Leave initialization mode */
	_CAN1_FILTER_LEAVE_INIT;
}

void CAN1_SetDLC(struct Interface *interface, uint16_t len)
{
	CAN_Mailbox *txmsg = interface->msg;

	txmsg->DLC = len;
}

void CAN1_PrepareMsg(struct Interface *interface, uint8_t *data)
{
	CAN_Mailbox *txmsg = interface->msg;

	uint8_t packets = txmsg->DLC / CAN_MAX_DLC;
	uint8_t lastdlc = txmsg->DLC % CAN_MAX_DLC;

	/* Send all full-sized packets */
	for(int i = 0; i < packets; ++i)
	{
		txmsg->DLC = CAN_MAX_DLC;
		memcpy((uint8_t *)txmsg->Data, data, txmsg->DLC);
		CAN1_Send(txmsg);
		data += CAN_MAX_DLC;
	}

	/* Send last packet that is shorter than 8 bytes */
	if(0U != lastdlc)
	{
		txmsg->DLC = lastdlc;
		memcpy((uint8_t *)txmsg->Data, data, txmsg->DLC);
		CAN1_Send(txmsg);
	}
}

void CAN1_Send(CAN_Mailbox *txmsg)
{
	uint32_t tempreg = 0U;
	uint8_t txmailbox = 0U;

	/* Check if there is any mailbox empty */
	if(IS_ANY_TX_MAILBOX_EMPTY)
	{
		/* Find first empty mailbox */
		if((CAN1->TSR & CAN_TX_MAILBOX2_EMPTY) == CAN_TX_MAILBOX2_EMPTY)
		{
			txmailbox = 2;
		}

		else if((CAN1->TSR & CAN_TX_MAILBOX1_EMPTY) == CAN_TX_MAILBOX1_EMPTY)
		{
			txmailbox = 1;
		}

		else
		{
			txmailbox = 0;
		}

		/* Set ID of tx message */
		if(CAN_TX_MAILBOX_IDE == txmsg->IDE)
		{
			tempreg |= (txmsg->ID << 3) | CAN_TX_MAILBOX_IDE;
		}

		else
		{
			tempreg |= (txmsg->ID << 21);
		}
		CAN1->sTxMailBox[txmailbox].TIR = tempreg;

		/* Set Data length of tx message */
		CAN1->sTxMailBox[txmailbox].TDTR &= 0xFFFFFFF0U;
		CAN1->sTxMailBox[txmailbox].TDTR |= txmsg->DLC;

		/* Set the data to be sent */
		CAN1->sTxMailBox[txmailbox].TDLR = (((uint32_t)txmsg->Data[3] << 24) |
											((uint32_t)txmsg->Data[2] << 16) |
											((uint32_t)txmsg->Data[1] << 8) |
											((uint32_t)txmsg->Data[0]));

		CAN1->sTxMailBox[txmailbox].TDHR = (((uint32_t)txmsg->Data[7] << 24) |
											((uint32_t)txmsg->Data[6] << 16) |
											((uint32_t)txmsg->Data[5] << 8) |
											((uint32_t)txmsg->Data[4]));

		/* Request transmission of tx mailbox */
		_CAN_TX_MAILBOX_SEND(txmailbox);

		/* Wait for the request to be completed */
		while((CAN1->TSR & CAN_TX_MAILBOX_RQ_COMPLETED(txmailbox)) != CAN_TX_MAILBOX_RQ_COMPLETED(txmailbox));
	}
}

CAN_RXState CAN1_Receive(CAN_Mailbox *rxmsg, uint8_t fifonum)
 {
	if(IS_ANY_MSG_PENDING_FIFO0)
	{
		/* Get the ID of rx message */
		if(IS_RX_MAILBOX_EXTID(fifonum))
		{
			rxmsg->ID = (CAN1->sFIFOMailBox->RIR >> 3);
			rxmsg->IDE = CAN_TX_MAILBOX_IDE;
		}

		else
		{
			rxmsg->ID = (CAN1->sFIFOMailBox->RIR >> 21);
			rxmsg->IDE = 0U;
		}

		/* Get the data length of rx message */
		rxmsg->DLC = (uint8_t)(CAN1->sFIFOMailBox->RDTR & 0x0F);

		/* Get the data */
		rxmsg->Data[0] = (uint8_t)CAN1->sFIFOMailBox->RDLR;
		rxmsg->Data[1] = (uint8_t)(CAN1->sFIFOMailBox->RDLR >> 8);
		rxmsg->Data[2] = (uint8_t)(CAN1->sFIFOMailBox->RDLR >> 16);
		rxmsg->Data[3] = (uint8_t)(CAN1->sFIFOMailBox->RDLR >> 24);
		rxmsg->Data[4] = (uint8_t)CAN1->sFIFOMailBox->RDHR;
		rxmsg->Data[5] = (uint8_t)(CAN1->sFIFOMailBox->RDHR >> 8);
		rxmsg->Data[6] = (uint8_t)(CAN1->sFIFOMailBox->RDHR >> 16);
		rxmsg->Data[7] = (uint8_t)(CAN1->sFIFOMailBox->RDHR >> 24);

		/* Release FIFO */
		if(CAN_FIFO_0 == fifonum)
		{
			_CAN_RELEASE_FIFO0;
		}

		else
		{
			_CAN_RELEASE_FIFO1;
		}

		return CAN_RECEIVED;
	}

	return CAN_NOMESSAGE;
}

__attribute__ ((weak)) void CAN1_RX0Callback(void)
{
}

void CAN1_RX0_IRQHandler(void)
{
	if(IS_ANY_MSG_PENDING_FIFO0)
	{
		CAN1_RX0Callback();
		_CAN_FIFO0_MSG_PENDING_INTERRUPT_DISABLE;
	}
}
