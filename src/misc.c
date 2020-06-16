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
volatile uint32_t Tick;

uint16_t swap_uint16(uint16_t val)
{
    return (val << 8) | (val >> 8 );
}


uint32_t swap_uint32(uint32_t val)
{
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF);
    return (val << 16) | (val >> 16);
}


uint64_t swap_uint64(uint64_t val)
{
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL) | ((val >> 8) & 0x00FF00FF00FF00FFULL);
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL) | ((val >> 16) & 0x0000FFFF0000FFFFULL);
    return (val << 32) | (val >> 32);
}


uint16_t inc_swapped_uint16(uint16_t val)
{
	if((uint8_t)(val >> 8) != 0xFF)
	{
		return val + 0x0100U;
	}

	else
	{
		return (uint16_t)(val + 0x0101U);
	}
}


uint32_t inc_swapped_uint32(uint32_t val)
{
	if((uint8_t)(val >> 24) != 0xFF)
	{
		return val + 0x01000000U;
	}

	else
	{
		return (uint32_t)(val + 0x01010000U);
	}
}


void Delay_ms(uint16_t time)
{
	for(int i = 0; i < 4000*time; ++i);
}


void SysTick_Init(uint32_t ticks)
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
		SysTick_DelayCallback();
	}
}


__attribute__ ((weak)) void SysTick_DelayCallback(void)
{
}


uint32_t FormatTime(uint32_t Time)
{
	uint32_t ftime;
	uint8_t h, m, s, ms;

	h = (uint8_t)((Time/1000)/3600);
	m = (uint8_t)(((Time - (h*3600*1000))/1000)/60);
	s = (uint8_t)((Time - (h*3600*1000) - (m*60*1000))/1000);
	ms = (uint8_t)((Time - (h*3600*1000) - (m*60*1000) - (s*1000))/10); // Miliseconds are divided by 10 to fit on 8 bytes

	ftime = (uint32_t)(h << 24 | m << 16 | s << 8 | ms);

	return ftime;
}
