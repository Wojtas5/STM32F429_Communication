/*
 * bsp.h
 *
 *  Created on: Apr 1, 2020
 *      Author: wsadzik
 */

#ifndef BSP_H_
#define BSP_H_

#define LED_GREEN (1U << 0)
#define LED_BLUE  (1U << 7)
#define LED_RED   (1U << 14)

#define PIN0_OUTPUT  0b01
#define PIN7_OUTPUT  (0b01 << 14)
#define PIN14_OUTPUT (0b01 << 28)

#define GPIOB_ENABLE (1U << 1)

void BSP_Init(void);
void BSP_Delay_ms(uint16_t time);
void BSP_LedGreenOn(void);
void BSP_LedGreenOff(void);
void BSP_LedBlueOn(void);
void BSP_LedBlueOff(void);
void BSP_LedRedOn(void);
void BSP_LedRedOff(void);

#endif /* BSP_H_ */
