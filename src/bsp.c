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
	RCC->AHB1ENR  |= GPIOB_ENABLE;                               // Provide clock for GPIOB peripheral
	RCC->AHB1ENR  |= GPIOC_ENABLE;
	RCC->APB2ENR  |= SYSCFG_ENABLE;

	GPIOB->MODER  |= (PIN0_OUTPUT | PIN7_OUTPUT | PIN14_OUTPUT); // Set PINs direction to output
	GPIOB->OTYPER &= (~LED_GREEN & ~LED_BLUE & ~LED_RED);        // Set PINs type to push-pull

	GPIOC->MODER  &= PIN13_INPUT;
	GPIOC->PUPDR  &= PIN13_NOPULL;

	SYSCFG->EXTICR[3] |= (1U << 5);                              // Selects PC13 pin as source of EXTIx

	EXTI->IMR     |= USER_BUTTON; 								 // Interrupt request from this pin is not masked
	EXTI->RTSR    |= USER_BUTTON;								 // Rising-edge trigger selection

	NVIC_EnableIRQ(EXTI15_10_IRQn);                              // Enable specific interrupt in NVIC
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

void BSP_LedRedToggle(void)
{
	GPIOB->ODR ^= LED_RED;
}

/* ============================================= */
/*  Functions to be moved to a different module  */
/* ============================================= */

void EXTI15_10_IRQHandler(void)
{
	if ((EXTI->PR & USER_BUTTON) != 0)
	{
		BSP_LedRedToggle();

		EXTI->PR |= USER_BUTTON;  // Clear interrupt flag
	}
}

void RCC_SYSCFGEnable(void)
{
	RCC->APB2ENR |= SYSCFG_ENABLE;
}

