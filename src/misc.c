/*
 * misc.c
 *
 *  Created on: 16 Apr 2020
 *      Author: wsadzik
 */

#include "stm32f4xx.h"
#include "stdint.h"
#include "misc.h"

/* Global variables */
uint32_t Tick;

uint16_t swap_uint16(uint16_t val)
{
    return (val << 8) | (val >> 8 );
}


uint32_t swap_uint32(uint32_t val)
{
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | (val >> 16);
}


void Delay_ms(uint16_t time)
{
	for (int i = 0; i < 4000*time; ++i);
}


void Systick_Init(uint32_t ticks)
{
	SysTick->LOAD = (uint32_t)(ticks - 1U);
	SysTick->VAL  = 0U;
	SysTick->CTRL = (SYSTICK_USE_PROCESSOR_CLOCK |
					 SYSTICK_ENABLE_INTERRUPT |
					 SYSTICK_COUNTER_ENABLE);

	NVIC_SetPriority(SysTick_IRQn, 1U);
	NVIC_EnableIRQ(SysTick_IRQn);
}


void SysTick_Handler(void)
{
	++Tick;
}


uint32_t SysTick_GetTick(void)
{
	return Tick;
}


void SysTick_Delay(uint32_t time)
{
	uint32_t tickstart = 0U;
	tickstart = SysTick_GetTick();
	while((SysTick_GetTick() - tickstart) < time)
	{
	}
}
