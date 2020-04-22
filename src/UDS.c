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

struct UDS_Neg udsneg;
struct UDS_Pos udspos;

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
