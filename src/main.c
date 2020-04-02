/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f4xx.h"
#include "bsp.h"


int main(void)
{
	BSP_Init();

	while(1)
	{
		BSP_LedGreenOn();

		BSP_Delay_ms(500);

		BSP_LedGreenOff();

		BSP_Delay_ms(100);
	}
}
