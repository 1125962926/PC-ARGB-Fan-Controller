/*
 * ws2812.h
 *
 *  Created on: May 14, 2025
 *      Author: li-rongfu
 */

#ifndef INC_WS2812_H_
#define INC_WS2812_H_

#include "tim.h"

// 8 leds per fan
#define LED_COUNT 	8
#define FAN_COUNT 	2
#define LED_TOTAL   (LED_COUNT * FAN_COUNT)
#define COLOR_LEN   24   // 8 bit * 3 color; COLOR Order: Green Red Blue

#define Low_Code 		30
#define High_Code 		60
#define CodeReset 		0

// TIM and DMA
#define WS2812_TIM  	htim3
#define WS2812_CHANNEL  TIM_CHANNEL_1

extern uint16_t WS2812_Buf[];


// =============== 效果演示 ================
// 关闭
void close_light(void);
// 单色
void single_light(uint8_t fan_id, uint8_t red, uint8_t green, uint8_t blue);
// 彩虹
void rainbow_light(uint8_t speed, uint8_t step);
// 呼吸灯
void breathing_light(uint8_t speed, uint8_t step);
// =============== 内部函数 ================
void WS2812_Stop_PWM(void);
void WS2812_Start_PWM(void);
void WS2812_Set_Color(uint8_t index, uint32_t Color);
void WS2812_Set_All_Color(uint32_t Color);
uint32_t WS2812_rgb2color(uint8_t red, uint8_t green, uint8_t blue);
void WS2812_color2rgb(uint32_t color, uint8_t *red, uint8_t *green, uint8_t *blue);
void WS2812_Update(void);

uint32_t get_rainbow_color(int phase);
uint16_t speed_to_delay(uint8_t speed);


#endif /* INC_WS2812_H_ */
