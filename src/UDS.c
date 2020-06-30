/*
 * UDP.c
 *
 *  Created on: 17 Apr 2020
 *      Author: wsadzik
 */

#include "stm32f4xx.h"
#include "UDS.h"
#include "misc.h"

struct UDS_Neg udsneg;
struct UDS_Pos udspos;
struct UDS_PosDID udsposdid;
struct UDS_PosRoutine udsposroutine;

UDS_Stopwatch Stopwatches[UDS_MAX_STOPWATCH_COUNT];

void UDS_Respond(struct Interface *interface, uint8_t *msg)
{
	uint8_t *response;
	uint8_t requestedSID = msg[REQUEST_SID_INDEX];
	uint8_t reset = 0U;

	switch(requestedSID)
	{
		case UDS_TESTER_PRESENT_RQ_SID:
			if(UDS_END_OF_REQUEST == msg[SUBSERVICE_INDEX])
			{
				response = UDS_PreparePosResponse(interface, requestedSID);
			}

			else
			{
				response = UDS_IncorrectMsgLenOrInvFormat(interface, requestedSID);
			}
			break;

		case UDS_ECU_RESET_RQ_SID:
			if(UDS_END_OF_REQUEST == msg[SUBSERVICE_INDEX])
			{
				response = UDS_PreparePosResponse(interface, requestedSID);
				reset = 1U;
			}

			else
			{
				response = UDS_IncorrectMsgLenOrInvFormat(interface, requestedSID);
			}
			break;

		case UDS_READ_DATA_BY_ID_RQ_SID:
			response = UDS_ReadDataByID(interface, msg);
			break;

		case UDS_ROUTINE_CONTROL_RQ_SID:
			response = UDS_RoutineControl(interface, msg);
			break;

		default:
			response = UDS_ServiceNotSupported(interface, requestedSID);
			break;
	}

	interface->output(interface, response);

	if(reset)
	{
		/* Small delay for better stability */
		Delay_ms(1);
		NVIC_SystemReset();
	}
}


uint8_t *UDS_SubfunctionNotSupported(struct Interface *interface, uint8_t RequestSID)
{
	return UDS_PrepareNegResponse(interface, RequestSID, UDS_SUBFUNCTION_NOT_SUPPORTED);
}


uint8_t *UDS_RequestSequenceError(struct Interface *interface, uint8_t RequestSID)
{
	return UDS_PrepareNegResponse(interface, RequestSID, UDS_REQUEST_SEQUENCE_ERROR);
}


uint8_t *UDS_IncorrectMsgLenOrInvFormat(struct Interface *interface, uint8_t RequestSID)
{
	return UDS_PrepareNegResponse(interface, RequestSID, UDS_INCORRECT_MSG_LEN_OR_INV_FORMAT);
}


uint8_t *UDS_ServiceNotSupported(struct Interface *interface, uint8_t RequestSID)
{
	return UDS_PrepareNegResponse(interface, RequestSID, UDS_SERVICE_NOT_SUPPORTED);
}


uint8_t *UDS_PrepareNegResponse(struct Interface *interface, uint8_t RequestSID, uint8_t ErrorSID)
{
	udsneg.NegativeSID = UDS_COMMAND_NOT_SUPPORTED;
	udsneg.RequestedSID = RequestSID;
	udsneg.ResponseCode = ErrorSID;

	interface->len(interface, sizeof(struct UDS_Neg));

	return (uint8_t *)&udsneg;
}


uint8_t *UDS_PreparePosResponse(struct Interface *interface, uint8_t RequestSID)
{
	udspos.PositiveSID = (uint8_t)(RequestSID + UDS_RESPONSE_SID_OFFSET);
	udspos.RequestedSID = RequestSID;

	interface->len(interface, sizeof(struct UDS_Pos));

	return (uint8_t *)&udspos;
}


uint8_t *UDS_ReadDataByID(struct Interface *interface, uint8_t *msg)
{
	uint16_t *dataid = (uint16_t *)(&msg[DID_INDEX]);
	*dataid = swap_uint16(*dataid);

	if(UDS_TIME_FROM_STARTUP_DID == *dataid)
	{
		udsposdid.PositiveSID = msg[REQUEST_SID_INDEX];
		udsposdid.DID = swap_uint16(*dataid);
		udsposdid.DataRecord = swap_uint16((uint16_t)(SysTick_GetTick()/1000));
	}

	else
	{
		return UDS_IncorrectMsgLenOrInvFormat(interface, msg[REQUEST_SID_INDEX]);
	}

	interface->len(interface, sizeof(struct UDS_PosDID));

	return (uint8_t *)&udsposdid;
}


uint8_t *UDS_RoutineControl(struct Interface *interface, uint8_t *msg)
{
	uint8_t *response;
	uint16_t *routineid = (uint16_t *)(&msg[STOPWATCH_ROUTINE_INDEX]);
	*routineid = swap_uint16(*routineid);

	if(UDS_STOPWATCH_ROUTINE == *routineid)
	{
		response = UDS_StopwatchRoutine(interface, msg);
	}

	else
	{
		return UDS_SubfunctionNotSupported(interface, msg[REQUEST_SID_INDEX]);
	}

	return response;
}


uint8_t *UDS_StopwatchRoutine(struct Interface *interface, uint8_t *msg)
{
	uint8_t id = 0xFFU;

	/* Set default length of a frame to exclude TimeValue */
	interface->len(interface, sizeof(struct UDS_PosRoutine) - sizeof(udsposroutine.TimeValue));

	switch(msg[SUBSERVICE_INDEX])
	{
		case UDS_STOPWATCH_START:
			for(int i = 0; i < UDS_MAX_STOPWATCH_COUNT; ++i)
			{
				if(STOPWATCH_READY == Stopwatches[i].State)
				{
					id = i;
					break;
				}
			}

			if(0xFFU == id)
			{
				for(int i = 0; i < UDS_MAX_STOPWATCH_COUNT; ++i)
				{
					if(STOPWATCH_STOPPED == Stopwatches[i].State)
					{
						id = i;
						break;
					}

					if((UDS_MAX_STOPWATCH_COUNT-1) == i)
					{
						return UDS_ServiceNotSupported(interface, msg[REQUEST_SID_INDEX]);
					}
				}
			}

			Stopwatches[id].ID = id;
			Stopwatches[id].State = STOPWATCH_RUNNING;
			Stopwatches[id].TimeValue = SysTick_GetTick();
			udsposroutine.StopwatchID = Stopwatches[id].ID;
			break;

		case UDS_STOPWATCH_STOP:
			if(msg[STOPWATCH_ID_INDEX] < UDS_MAX_STOPWATCH_COUNT)
			{
				if(STOPWATCH_RUNNING == Stopwatches[msg[STOPWATCH_ID_INDEX]].State)
				{
					Stopwatches[msg[STOPWATCH_ID_INDEX]].State = STOPWATCH_STOPPED;
					Stopwatches[msg[STOPWATCH_ID_INDEX]].TimeValue = SysTick_GetTick() - Stopwatches[msg[STOPWATCH_ID_INDEX]].TimeValue;
					udsposroutine.StopwatchID = Stopwatches[msg[STOPWATCH_ID_INDEX]].ID;
				}

				else
				{
					return UDS_IncorrectMsgLenOrInvFormat(interface, msg[REQUEST_SID_INDEX]);
				}
			}

			else
			{
				return UDS_IncorrectMsgLenOrInvFormat(interface, msg[REQUEST_SID_INDEX]);
			}
			break;

		case UDS_STOPWATCH_READ:
			if(msg[STOPWATCH_ID_INDEX] < UDS_MAX_STOPWATCH_COUNT)
			{
				if(STOPWATCH_RUNNING == Stopwatches[msg[STOPWATCH_ID_INDEX]].State)
				{
					udsposroutine.TimeValue = swap_uint32(FormatTime(SysTick_GetTick() - Stopwatches[msg[STOPWATCH_ID_INDEX]].TimeValue));
				}

				else if(STOPWATCH_STOPPED == Stopwatches[msg[STOPWATCH_ID_INDEX]].State)
				{
					Stopwatches[msg[STOPWATCH_ID_INDEX]].State = STOPWATCH_READY;
					udsposroutine.TimeValue = swap_uint32(FormatTime(Stopwatches[msg[STOPWATCH_ID_INDEX]].TimeValue));
				}

				else
				{
					return UDS_IncorrectMsgLenOrInvFormat(interface, msg[REQUEST_SID_INDEX]);
				}
			}

			else
			{
				return UDS_IncorrectMsgLenOrInvFormat(interface, msg[REQUEST_SID_INDEX]);
			}

			udsposroutine.StopwatchID = Stopwatches[msg[STOPWATCH_ID_INDEX]].ID;

			/* Set length of the frame to include TimeValue */
			interface->len(interface, sizeof(struct UDS_PosRoutine));
			break;

		default:
			return UDS_IncorrectMsgLenOrInvFormat(interface, msg[REQUEST_SID_INDEX]);
			break;
	}

	udsposroutine.PositiveSID = (uint8_t)(UDS_ROUTINE_CONTROL_RQ_SID + UDS_RESPONSE_SID_OFFSET);
	udsposroutine.Subservice = msg[SUBSERVICE_INDEX];
	udsposroutine.Routine = swap_uint16(UDS_STOPWATCH_ROUTINE);

	return (uint8_t *)&udsposroutine;
}


void UDS_TesterPresent(struct Interface *interface)
{
	uint8_t msg = UDS_TESTER_PRESENT_RQ_SID;

	interface->len(interface, sizeof(UDS_TESTER_PRESENT_RQ_SID));
	interface->output(interface, &msg);
}


void UDS_Reset(struct Interface *interface)
{
	uint8_t msg = UDS_ECU_RESET_RQ_SID;

	interface->len(interface, sizeof(UDS_ECU_RESET_RQ_SID));
	interface->output(interface, &msg);
}


void UDS_TimeFromStartupDID(struct Interface *interface)
{
	uint32_t msg = swap_uint32(UDS_TIME_FROM_STARTUP_RQ_SID);

	interface->len(interface, sizeof(UDS_TIME_FROM_STARTUP_RQ_SID) - sizeof(uint8_t));
	interface->output(interface, (uint8_t *)&msg);
}


void UDS_StartStopwatch(struct Interface *interface)
{
	uint32_t msg = swap_uint32(UDS_STOPWATCH_START_RQ_SID);

	interface->len(interface, sizeof(UDS_STOPWATCH_START_RQ_SID));
	interface->output(interface, (uint8_t *)&msg);
}


void UDS_StopStopwatch(struct Interface *interface, uint8_t stopwatchid)
{
	uint64_t msg = swap_uint64(UDS_STOPWATCH_STOP_RQ_SID(stopwatchid));

	interface->len(interface, sizeof(UDS_STOPWATCH_STOP_RQ_SID(stopwatchid)) + sizeof(uint8_t));
	interface->output(interface, (uint8_t *)&msg);
}


void UDS_ReadStopwatch(struct Interface *interface, uint8_t stopwatchid)
{
	uint64_t msg = swap_uint64(UDS_STOPWATCH_READ_RQ_SID(stopwatchid));

	interface->len(interface, sizeof(UDS_STOPWATCH_READ_RQ_SID(stopwatchid)) + sizeof(uint8_t));
	interface->output(interface, (uint8_t *)&msg);
}
