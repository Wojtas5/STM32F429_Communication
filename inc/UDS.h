/*
 * UDS.h
 *
 *  Created on: 17 Apr 2020
 *      Author: wsadzik
 */

#ifndef UDS_H_
#define UDS_H_

#include "stdint.h"
#include "interface.h"

typedef enum {
	STOPWATCH_READY = 0x00U,

	STOPWATCH_RUNNING = 0x01U,

	STOPWATCH_STOPPED = 0x02U

} UDS_Stopwatch_state;

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

struct UDS_PosRoutine {
	uint8_t PositiveSID;

	uint8_t Subservice;

	uint16_t Routine;

	uint8_t StopwatchID;

	uint32_t TimeValue;

} __attribute__ ((packed));

typedef struct {
	uint8_t ID;

	UDS_Stopwatch_state State;

	uint32_t TimeValue;

} UDS_Stopwatch;

/* UDS message indexes */
#define REQUEST_SID_INDEX          			0U
#define SUBSERVICE_INDEX					1U
#define DID_INDEX 							1U
#define STOPWATCH_ROUTINE_INDEX				2U
#define STOPWATCH_ID_INDEX 		   			4U

/* Services */
#define UDS_RESPONSE_SID_OFFSET 			((uint8_t)0x40U)
#define UDS_ECU_RESET_RQ_SID 				((uint8_t)0x11U)
#define UDS_READ_DATA_BY_ID_RQ_SID 			((uint8_t)0x22U)
#define UDS_ROUTINE_CONTROL_RQ_SID 			((uint8_t)0x31U)
#define UDS_TESTER_PRESENT_RQ_SID 			((uint8_t)0x3EU)
#define UDS_TIME_FROM_STARTUP_RQ_SID 		((uint32_t)0x22010500U)
#define UDS_STOPWATCH_START_RQ_SID 			((uint32_t)0x31011301U)
#define UDS_STOPWATCH_STOP_RQ_SID(id) 		((uint64_t)(0x3102130100000000U | (id << 24)))
#define UDS_STOPWATCH_READ_RQ_SID(id) 		((uint64_t)(0x3103130100000000U | (id << 24)))

/* Data Identifiers */
#define UDS_TIME_FROM_STARTUP_DID 			((uint16_t)0x0105U)

/* Routine control */
#define UDS_MAX_STOPWATCH_COUNT				17U
#define UDS_STOPWATCH_ROUTINE 				((uint16_t)0x1301U)
#define UDS_STOPWATCH_START 				((uint8_t)0x01U)
#define UDS_STOPWATCH_STOP 					((uint8_t)0x02U)
#define UDS_STOPWATCH_READ 					((uint8_t)0x03U)

/* Error SIDs */
#define UDS_COMMAND_NOT_SUPPORTED 			((uint8_t)0x7FU)
#define UDS_REQUEST_SEQUENCE_ERROR			((uint8_t)0x24U)
#define UDS_INCORRECT_MSG_LEN_OR_INV_FORMAT ((uint8_t)0x13U)
#define UDS_SUBFUNCTION_NOT_SUPPORTED		((uint8_t)0x12U)
#define UDS_SERVICE_NOT_SUPPORTED 			((uint8_t)0x11U)
#define UDS_END_OF_REQUEST        			((uint8_t)0x00U)

/* ===================== */
/*  Function prototypes  */
/* ===================== */

void UDS_Respond(struct Interface *interface, uint8_t *msg);
uint8_t *UDS_IncorrectMsgLenOrInvFormat(struct Interface *interface, uint8_t RequestSID);
uint8_t *UDS_ServiceNotSupported(struct Interface *interface, uint8_t RequestSID);
uint8_t *UDS_PrepareNegResponse(struct Interface *interface, uint8_t RequestSID, uint8_t ErrorSID);
uint8_t *UDS_PreparePosResponse(struct Interface *interface, uint8_t RequestSID);
uint8_t *UDS_ReadDataByID(struct Interface *interface, uint8_t *msg);
uint8_t *UDS_RoutineControl(struct Interface *interface, uint8_t *msg);
uint8_t *UDS_StopwatchRoutine(struct Interface *interface, uint8_t *msg);
void UDS_TesterPresent(struct Interface *interface);
void UDS_Reset(struct Interface *interface);
void UDS_TimeFromStartupDID(struct Interface *interface);
void UDS_StartStopwatch(struct Interface *interface);
void UDS_StopStopwatch(struct Interface *interface, uint8_t stopwatchid);
void UDS_ReadStopwatch(struct Interface *interface, uint8_t stopwatchid);

#endif /* UDS_H_ */
