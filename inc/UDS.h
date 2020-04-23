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

} __attribute__ ((packed));

struct UDS_PosDID {
	uint8_t PositiveSID;

	uint16_t DID;

	uint16_t DataRecord;

} __attribute__ ((packed));

/* Services */
#define UDS_ECU_RESET_RQ_SID 				((uint8_t)0x11U)
#define UDS_ECU_RESET_RP_SID 				((uint8_t)0x51U)
#define UDS_READ_DATA_BY_ID_RQ_SID 			((uint8_t)0x22U)
#define UDS_TESTER_PRESENT_RQ_SID 			((uint8_t)0x3EU)
#define UDS_TESTER_PRESENT_RP_SID 			((uint8_t)0x7EU)

#define UDS_RESPONSE_SID_OFFSET 			((uint8_t)0x40U)

/* Data Identifiers */
#define UDS_TIME_FROM_STARTUP_DID 			((uint16_t)0x0105U)

/* Error SIDs */
#define UDS_COMMAND_NOT_SUPPORTED 			((uint8_t)0x7FU)
#define UDS_INCORRECT_MSG_LEN_OR_INV_FORMAT ((uint8_t)0x13U)
#define UDS_SERVICE_NOT_SUPPORTED 			((uint8_t)0x11U)
#define UDS_END_OF_REQUEST        			((uint8_t)0x00U)

/* ===================== */
/*  Function prototypes  */
/* ===================== */

void UDS_Respond(uint8_t *msg);
uint8_t *UDS_IncorrectMsgLenOrInvFormat(uint8_t RequestSID);
uint8_t *UDS_ServiceNotSupported(uint8_t RequestSID);
uint8_t *UDS_PrepareNegResponse(uint8_t RequestSID, uint8_t ErrorSID);
uint8_t *UDS_PreparePosResponse(uint8_t RequestSID);
uint8_t *UDS_ReadDataByID(uint8_t *msg);

#endif /* UDS_H_ */
