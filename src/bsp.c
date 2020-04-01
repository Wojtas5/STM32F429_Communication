/*
 * bsp.c
 *
 *  Created on: Apr 1, 2020
 *      Author: wsadzik
 */

#include "stm32f4xx.h"
#include "bsp.h"
#include "stdint.h"


void BSP_Init(void)
{
	RCC->AHB1ENR  |= GPIOB_ENABLE;                              // Provide clock for GPIOB peripheral

	GPIOB->MODER  |= (PIN0_OUTPUT | PIN7_OUTPUT | PIN14_OUTPUT); // Set PINs to output mode
	GPIOB->OTYPER |= (~LED_GREEN & ~LED_BLUE & ~LED_RED);       // Set PINs type to push-pull
}

void BSP_Delay_ms(uint16_t time)
{
	for (int i = 0; i < 4000*time; i++);
}

void BSP_LedGreenOn(void)
{
	GPIOB->ODR |= LED_GREEN;
}

void BSP_LedGreenOff(void)
{
	GPIOB->ODR &= ~LED_GREEN;
}

void BSP_LedBlueOn(void)
{
	GPIOB->ODR |= LED_BLUE;
}

void BSP_LedBlueOff(void)
{
	GPIOB->ODR &= ~LED_BLUE;
}

void BSP_LedRedOn(void)
{
	GPIOB->ODR |= LED_RED;
}

void BSP_LedRedOff(void)
{
	GPIOB->ODR &= ~LED_RED;
}
