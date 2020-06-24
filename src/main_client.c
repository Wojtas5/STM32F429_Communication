#if CLIENT
#include "stm32f4xx.h"
#include "bsp.h"
#include "misc.h"
#include "CAN.h"
#include "UDS.h"
#include "interface.h"

/* Macros */
#define CAN1_SPEED_500kbps 500000U
#define CAN1_DEFAULT_FRAME_ID 0x0101U

/* Global variables */

/* Global structures */
CAN_Mailbox cantx, canrx[20];
CAN_Mailbox *currcanrx = canrx;
struct Interface CAN_interface = {CAN, &CAN1_PrepareMsg, &CAN1_SetDLC, (CAN_Mailbox *)&cantx};

/* Function prototypes */
static void CAN1_DisableFiltering(void);

int main(void)
{
	/* Initialize onboard LEDs and a button */
	BSP_Init();

	/* Initialize Systick timer peripheral */
	/* For some reason variable SystemCoreClock divided by 1000 doesn't gives us 1 tick per 1 ms
	 * TODO Check why 60000 is the good value */
	SysTick_Init(60000U);

	/* Initialize CAN1 peripheral */
	CAN1_Init(CAN1_SPEED_500kbps);

	/* Disable filtering of CAN frames */
	CAN1_DisableFiltering();

	/* Set default ID of CAN frame */
	cantx.ID = CAN1_DEFAULT_FRAME_ID;

	/* Begin sending messages */
	UDS_TesterPresent(&CAN_interface);
	UDS_TimeFromStartupDID(&CAN_interface);
	UDS_StartStopwatch(&CAN_interface);
	CAN1_Receive(currcanrx, CAN_FIFO_0); // Receive one message, because FIFO can only hold 3
	currcanrx += 1;
	UDS_StartStopwatch(&CAN_interface);
	SysTick_Delay(10);

	UDS_ReadStopwatch(&CAN_interface, 0);
	UDS_ReadStopwatch(&CAN_interface, 1);
	UDS_StopStopwatch(&CAN_interface, 0);
	SysTick_Delay(10);

	UDS_ReadStopwatch(&CAN_interface, 0);
	UDS_ReadStopwatch(&CAN_interface, 1);
	SysTick_Delay(10);

	UDS_ReadStopwatch(&CAN_interface, 0);
	UDS_ReadStopwatch(&CAN_interface, 1);
	SysTick_Delay(10);

	while(1)
	{
	}
}

static void CAN1_DisableFiltering(void)
{
	CAN_Filter canfilter;
	canfilter.FilterNum = 0U;
	canfilter.FilterIDH = 0U;
	canfilter.FilterIDL = 0U;
	canfilter.FilterMaskH = 0U;
	canfilter.FilterMaskL = 0U;

	CAN1_FilterConfig(&canfilter);
}

void SysTick_DelayCallback(void)
{
	if(CAN_RECEIVED == CAN1_Receive(currcanrx, CAN_FIFO_0))
	{
		currcanrx += 1;
	}
}

#endif
