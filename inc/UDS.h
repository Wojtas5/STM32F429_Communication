/*
 * UDS.h
 *
 *  Created on: 17 Apr 2020
 *      Author: wsadzik
 */

#ifndef UDS_H_
#define UDS_H_

#include "stdint.h"

struct UDS_Neg {
	uint8_t NegativeSID;

	uint8_t RequestedSID;

	uint8_t ResponseCode;

} __attribute__ ((packed));

struct UDS_Pos {
	uint8_t PositiveSID;

	uint8_t RequestedSID;

} __attribute__ ((packed));;

#define UDS_TESTER_PRESENT_RQ_SID 			((uint8_t)0x3EU)
#define UDS_TESTER_PRESENT_RP_SID 			((uint8_t)0x7EU)
#define UDS_COMMAND_NOT_SUPPORTED 			((uint8_t)0x7FU)
#define UDS_INCORRECT_MSG_LEN_OR_INV_FORMAT ((uint8_t)0x13U)
#define UDS_SERVICE_NOT_SUPPORTED 			((uint8_t)0x11U)
#define UDS_END_OF_REQUEST        			((uint8_t)0x00U)

/* ===================== */
/*  Function prototypes  */
/* ===================== */

void UDS_Respond(uint8_t *msg);

#endif /* UDS_H_ */
