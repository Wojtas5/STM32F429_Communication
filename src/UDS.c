/*
 * UDP.c
 *
 *  Created on: 17 Apr 2020
 *      Author: wsadzik
 */

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

	if(UDS_TESTER_PRESENT_RQ_SID == msg[0])
	{
		if(UDS_END_OF_REQUEST == msg[1])
		{
			udspos.PositiveSID = UDS_TESTER_PRESENT_RP_SID;
			udspos.RequestedSID = UDS_TESTER_PRESENT_RQ_SID;

			iphdr.TotalLength = swap_uint16(sizeof(struct IP_Header) + sizeof(struct UDS_Pos));

			response = (uint8_t *)&udspos;
		}

		else
		{
			udsneg.NegativeSID = UDS_COMMAND_NOT_SUPPORTED;
			udsneg.RequestedSID = msg[0];
			udsneg.ResponseCode = UDS_INCORRECT_MSG_LEN_OR_INV_FORMAT;

			iphdr.TotalLength = swap_uint16(sizeof(struct IP_Header) + sizeof(struct UDS_Neg));

			response = (uint8_t *)&udsneg;
		}
	}

	else
	{
		udsneg.NegativeSID = UDS_COMMAND_NOT_SUPPORTED;
		udsneg.RequestedSID = msg[0];
		udsneg.ResponseCode = UDS_SERVICE_NOT_SUPPORTED;

		iphdr.TotalLength = swap_uint16(sizeof(struct IP_Header) + sizeof(struct UDS_Neg));

		response = (uint8_t *)&udsneg;
	}

	iphdr.Checksum = IP_CalculateChecksum(&iphdr);

	IP_Send(&iphdr, &ethhdr, response);
}
