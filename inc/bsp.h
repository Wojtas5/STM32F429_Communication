/*
 * bsp.h
 *
 *  Created on: Apr 1, 2020
 *      Author: wsadzik
 */

#ifndef BSP_H_
#define BSP_H_

/* ==================== */
/*  Board defined PINs  */
/* ==================== */
#define LED_GREEN    ((uint32_t)(1U << 0))
#define LED_BLUE     ((uint32_t)(1U << 7))
#define LED_RED      ((uint32_t)(1U << 14))
#define USER_BUTTON  ((uint32_t)(1U << 13))

/* ================================ */
/*  GPIOx registers related macros  */
/* ================================ */

/* MODER */
#define PIN13_INPUT  ((uint32_t)(0b00 << 26))

#define PIN0_OUTPUT  ((uint32_t)0b01)
#define PIN7_OUTPUT  ((uint32_t)(0b01 << 14))
#define PIN14_OUTPUT ((uint32_t)(0b01 << 28))

#define PIN0_AF    ((uint32_t)(0b10 << 0))
#define PIN1_AF    ((uint32_t)(0b10 << 2))
#define PIN2_AF    ((uint32_t)(0b10 << 4))
#define PIN3_AF    ((uint32_t)(0b10 << 6))
#define PIN4_AF    ((uint32_t)(0b10 << 8))
#define PIN5_AF    ((uint32_t)(0b10 << 10))
#define PIN6_AF    ((uint32_t)(0b10 << 12))
#define PIN7_AF    ((uint32_t)(0b10 << 14))
#define PIN8_AF    ((uint32_t)(0b10 << 16))
#define PIN9_AF    ((uint32_t)(0b10 << 18))
#define PIN10_AF   ((uint32_t)(0b10 << 20))
#define PIN11_AF   ((uint32_t)(0b10 << 22))
#define PIN12_AF   ((uint32_t)(0b10 << 24))
#define PIN13_AF   ((uint32_t)(0b10 << 26))
#define PIN14_AF   ((uint32_t)(0b10 << 28))
#define PIN15_AF   ((uint32_t)(0b10 << 30))

/* PUPDR */
#define PIN1_NOPULL  ((uint32_t)(0b00 << 2))
#define PIN2_NOPULL  ((uint32_t)(0b00 << 4))
#define PIN4_NOPULL  ((uint32_t)(0b00 << 8))
#define PIN5_NOPULL  ((uint32_t)(0b00 << 10))
#define PIN7_NOPULL  ((uint32_t)(0b00 << 14))
#define PIN11_NOPULL ((uint32_t)(0b00 << 22))
#define PIN13_NOPULL ((uint32_t)(0b00 << 26))

/* OSPEEDR */
#define PIN1_HS      ((uint32_t)(0b10 << 2))
#define PIN2_HS      ((uint32_t)(0b10 << 4))
#define PIN4_HS 	 ((uint32_t)(0b10 << 8))
#define PIN5_HS	 	 ((uint32_t)(0b10 << 10))
#define PIN7_HS	 	 ((uint32_t)(0b10 << 14))
#define PIN11_HS 	 ((uint32_t)(0b10 << 22))
#define PIN13_HS 	 ((uint32_t)(0b10 << 26))

/* AFR */
#define PIN1_ETH     ((uint32_t)(0b1011 << 4))
#define PIN2_ETH     ((uint32_t)(0b1011 << 8))
#define PIN4_ETH     ((uint32_t)(0b1011 << 16))
#define PIN5_ETH     ((uint32_t)(0b1011 << 20))
#define PIN7_ETH     ((uint32_t)(0b1011 << 28))
#define PIN11_ETH    ((uint32_t)(0b1011 << 12))
#define PIN13_ETH    ((uint32_t)(0b1011 << 20))

/* ============================== */
/*  RCC registers related macros  */
/* ============================== */

/* AHB1ENR */
#define GPIOA_ENABLE  ((uint32_t)(1U << 0))
#define GPIOB_ENABLE  ((uint32_t)(1U << 1))
#define GPIOC_ENABLE  ((uint32_t)(1U << 2))
#define GPIOG_ENABLE  ((uint32_t)(1U << 6))

/* APB2ENR */
#define SYSCFG_ENABLE ((uint32_t)(1U << 14))

/* ===================== */
/*  Function prototypes  */
/* ===================== */
void BSP_Init(void);
void BSP_Delay_ms(uint16_t time);
void BSP_LedGreenOn(void);
void BSP_LedGreenOff(void);
void BSP_LedBlueOn(void);
void BSP_LedBlueOff(void);
void BSP_LedRedOn(void);
void BSP_LedRedOff(void);
void BSP_LedRedToggle(void);

void RCC_SYSCFGEnable(void);

#endif /* BSP_H_ */
