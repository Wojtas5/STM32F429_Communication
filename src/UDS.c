/*
 * UDP.c
 *
 *  Created on: 17 Apr 2020
 *      Author: wsadzik
 */

#include "stm32f4xx.h"
#include "UDS.h"
#include "IP.h"
#include "misc.h"

static uint8_t StopwatchStarted = 0U;
static uint32_t Time = 0U;

struct UDS_Neg udsneg;
struct UDS_Pos udspos;
struct UDS_PosDID udsposdid;
struct UDS_PosRoutine udsposroutine;

extern struct ETH_Header ethhdr;
extern struct IP_Header iphdr;

void UDS_Respond(uint8_t *msg)
{
	uint8_t *response;
	uint8_t requestedSID = msg[0];
	uint8_t reset = 0U;

	switch(requestedSID)
	{
		case UDS_TESTER_PRESENT_RQ_SID:
			if(UDS_END_OF_REQUEST == msg[1])
			{
				response = UDS_PreparePosResponse(requestedSID);
			}

			else
			{
				response = UDS_IncorrectMsgLenOrInvFormat(requestedSID);
			}
			break;

		case UDS_ECU_RESET_RQ_SID:
			if(UDS_END_OF_REQUEST == msg[1])
			{
				response = UDS_PreparePosResponse(requestedSID);
				reset = 1U;
			}

			else
			{
				response = UDS_IncorrectMsgLenOrInvFormat(requestedSID);
			}
			break;

		case UDS_READ_DATA_BY_ID_RQ_SID:
			response = UDS_ReadDataByID(msg);
			break;

		case UDS_ROUTINE_CONTROL_RQ_SID:
			response = UDS_RoutineControl(msg);
			break;

		default:
			response = UDS_ServiceNotSupported(requestedSID);
			break;
	}

	iphdr.Checksum = IP_CalculateChecksum(&iphdr);

	IP_Send(&iphdr, &ethhdr, response);

	if(reset)
	{
		/* Small delay for better stability */
		Delay_ms(1);
		NVIC_SystemReset();
	}
}


uint8_t *UDS_SubfunctionNotSupported(uint8_t RequestSID)
{
	return UDS_PrepareNegResponse(RequestSID, UDS_SUBFUNCTION_NOT_SUPPORTED);
}


uint8_t *UDS_RequestSequenceError(uint8_t RequestSID)
{
	return UDS_PrepareNegResponse(RequestSID, UDS_REQUEST_SEQUENCE_ERROR);
}


uint8_t *UDS_IncorrectMsgLenOrInvFormat(uint8_t RequestSID)
{
	return UDS_PrepareNegResponse(RequestSID, UDS_INCORRECT_MSG_LEN_OR_INV_FORMAT);
}


uint8_t *UDS_ServiceNotSupported(uint8_t RequestSID)
{
	return UDS_PrepareNegResponse(RequestSID, UDS_SERVICE_NOT_SUPPORTED);
}


uint8_t *UDS_PrepareNegResponse(uint8_t RequestSID, uint8_t ErrorSID)
{
	udsneg.NegativeSID = UDS_COMMAND_NOT_SUPPORTED;
	udsneg.RequestedSID = RequestSID;
	udsneg.ResponseCode = ErrorSID;

	iphdr.TotalLength = swap_uint16(sizeof(struct IP_Header) + sizeof(struct UDS_Neg));

	return (uint8_t *)&udsneg;
}


uint8_t *UDS_PreparePosResponse(uint8_t RequestSID)
{
	udspos.PositiveSID = (uint8_t)(RequestSID + UDS_RESPONSE_SID_OFFSET);
	udspos.RequestedSID = RequestSID;

	iphdr.TotalLength = swap_uint16(sizeof(struct IP_Header) + sizeof(struct UDS_Pos));

	return (uint8_t *)&udspos;
}


uint8_t *UDS_ReadDataByID(uint8_t *msg)
{
	uint16_t *dataid = (uint16_t *)(&msg[1]);
	*dataid = swap_uint16(*dataid);

	if(UDS_TIME_FROM_STARTUP_DID == *dataid)
	{
		udsposdid.PositiveSID = msg[0];
		udsposdid.DID = swap_uint16(*dataid);
		udsposdid.DataRecord = swap_uint16((uint16_t)(SysTick_GetTick()/1000));
	}

	else
	{
		return UDS_IncorrectMsgLenOrInvFormat(msg[0]);
	}

	iphdr.TotalLength = swap_uint16(sizeof(struct IP_Header) + sizeof(struct UDS_PosDID));

	return (uint8_t *)&udsposdid;
}


uint8_t *UDS_RoutineControl(uint8_t *msg)
{
	uint8_t *response;
	uint16_t *routineid = (uint16_t *)(&msg[2]);

	*routineid = swap_uint16(*routineid);

	if(UDS_STOPWATCH_ROUTINE == *routineid)
	{
		response = UDS_StopwatchRoutine(msg);
	}

	else
	{
		return UDS_SubfunctionNotSupported(msg[0]);
	}

	return response;
}


uint8_t *UDS_StopwatchRoutine(uint8_t *msg)
{
	/* Set default length of a frame to exclude TimeValue */
	iphdr.TotalLength = swap_uint16(sizeof(struct IP_Header) + sizeof(struct UDS_PosRoutine) - sizeof(uint32_t));

	switch(msg[1])
	{
		case UDS_STOPWATCH_START:
			if(0U == StopwatchStarted)
			{
				Time = SysTick_GetTick();
				StopwatchStarted = 1U;
			}

			else
			{
				return UDS_RequestSequenceError(msg[0]);
			}
			break;

		case UDS_STOPWATCH_STOP:
			if(1U == StopwatchStarted)
			{
				Time = SysTick_GetTick() - Time;
				StopwatchStarted = 0U;
			}

			else
			{
				return UDS_RequestSequenceError(msg[0]);
			}
			break;

		case UDS_STOPWATCH_READ:
			if(1U == StopwatchStarted)
			{
				udsposroutine.TimeValue = swap_uint32(FormatTime(SysTick_GetTick() - Time));
			}

			else
			{
				udsposroutine.TimeValue = swap_uint32(FormatTime(Time));
			}
			/* Set length of the frame to include TimeValue */
			iphdr.TotalLength = swap_uint16(sizeof(struct IP_Header) + sizeof(struct UDS_PosRoutine));

			break;

		default:
			return UDS_IncorrectMsgLenOrInvFormat(msg[0]);
			break;
	}

	udsposroutine.PositiveSID = (uint8_t)(UDS_ROUTINE_CONTROL_RQ_SID + UDS_RESPONSE_SID_OFFSET);
	udsposroutine.Subservice = msg[1];
	udsposroutine.Routine = swap_uint16(UDS_STOPWATCH_ROUTINE);

	return (uint8_t *)&udsposroutine;
}
