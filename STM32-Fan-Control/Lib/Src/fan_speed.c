/*
 * fan_speed.c
 *
 *  Created on: May 19, 2025
 *      Author: li-rongfu
 */

#include "fan_speed.h"
#include <stdint.h>

/**
 * @brief 将占空比（0~100）转换为CCR值
 * @param duty_percent 占空比（0~100）
 * @return CCRx值（0~2879）
 */
uint16_t DutyCycle_To_CCR(uint8_t duty_percent) {
	if (duty_percent == 0) return 0;         // 0% → 停转
    if (duty_percent >= 100) return FULL_PERIOD;    // 100% → 全速
    return (uint16_t)(((FULL_PERIOD + 1) * duty_percent) / 100);
}

/**
 * @brief  Set PWM
 */
inline void Fan_Speed_Set(uint8_t speed) {
	__HAL_TIM_SET_COMPARE(&FAN_SPEED_TIM, FAN_SPEED_CHANNEL, DutyCycle_To_CCR(speed));
}

/**
 * @brief  Start PWM
 */
inline void Fan_Speed_Start(void) {
	HAL_TIM_PWM_Start(&FAN_SPEED_TIM, FAN_SPEED_CHANNEL);
}

/**
 * @brief  Stop PWM
 */
inline void Fan_Speed_Stop(void) {
	HAL_TIM_PWM_Stop(&FAN_SPEED_TIM, FAN_SPEED_CHANNEL);
	__HAL_TIM_SetCounter(&FAN_SPEED_TIM, 0);
}

