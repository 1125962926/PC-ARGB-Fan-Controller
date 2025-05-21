/*
 * fan_speed.h
 *
 *  Created on: May 19, 2025
 *      Author: li-rongfu
 */

#ifndef INC_FAN_SPEED_H_
#define INC_FAN_SPEED_H_

#include "tim.h"

// TIM
#define FAN_SPEED_TIM  		htim2
#define FAN_SPEED_CHANNEL  	TIM_CHANNEL_2

// 满装载值 2880 - 1
#define FULL_PERIOD 2879

void Fan_Speed_Set(uint8_t speed);
void Fan_Speed_Start(void);
void Fan_Speed_Stop(void);

#endif /* INC_FAN_SPEED_H_ */
