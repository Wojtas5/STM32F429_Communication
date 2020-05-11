/*
 * misc.h
 *
 *  Created on: 16 Apr 2020
 *      Author: wsadzik
 */

#ifndef MISC_H_
#define MISC_H_

/* ======================== */
/*  Systick related macros  */
/* ======================== */

#define SYSTICK_USE_PROCESSOR_CLOCK ((uint32_t)(1U << 2))
#define SYSTICK_ENABLE_INTERRUPT    ((uint32_t)(1U << 1))
#define SYSTICK_COUNTER_ENABLE 		((uint32_t)(1U << 0))


/* ===================== */
/*  Function prototypes  */
/* ===================== */

uint16_t swap_uint16(uint16_t val);
uint32_t swap_uint32(uint32_t val);
void Delay_ms(uint16_t time);
void Systick_Init(uint32_t ticks);
uint32_t SysTick_GetTick(void);
void SysTick_Delay(uint32_t time);
uint32_t FormatTime(uint32_t Time);

#endif /* MISC_H_ */
