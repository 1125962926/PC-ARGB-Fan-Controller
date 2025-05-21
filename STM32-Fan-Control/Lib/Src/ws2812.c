/*
 * ws2812.c
 *
 *  Created on: May 14, 2025
 *      Author: li-rongfu
 */

#include "ws2812.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "usart_process.h"

// DMA data buffer
// static: Prevent DMA data errors when the function execution ends
// per led: 3 color, 1 byte = 8 bit
// + 1: CodeReset
uint16_t WS2812_Buf[LED_TOTAL * COLOR_LEN + 1] = {0};

// =============== 效果演示 ================
/**
 * @brief  关闭所有 LED
 */
void close_light(void) {
	WS2812_Set_All_Color(WS2812_rgb2color(0, 0, 0));
	WS2812_Update();
	HAL_Delay(5); // 等待传输
	WS2812_Stop_PWM();
}

/**
 * @brief  单色模式
 * @param  fan_id： 风扇编号
 * @param  rgb：  颜色
 */
void single_light(uint8_t fan_id, uint8_t red, uint8_t green, uint8_t blue)
{
	if (fan_id < 1 || fan_id > FAN_COUNT)
		return;

	uint32_t color = WS2812_rgb2color(red, green, blue);
	// 若设置第二个风扇，则 index 初值为 LED_COUNT
	uint8_t start = (fan_id - 1) * LED_COUNT;
	for(uint8_t index = start; index < start + LED_COUNT; index++)
		WS2812_Set_Color(index, color);
	WS2812_Update();
	HAL_Delay(5); // 等待传输
}

/**
 * @brief  彩虹色循环
 * @param  speed： 变换速度 1-100
 * @param  step：  步幅 (1-10，值越大，灯珠之间颜色间隔越大)
 */
void rainbow_light(uint8_t speed, uint8_t step)
{
	if (step == 0 || step > 10)
			step = 1; // 默认步长

	for (int i = 0; i <= 0xFF; i++)	{
		for (uint8_t led_id = 0; led_id < LED_TOTAL; led_id++) {
			// i + led_id * 2 确保不同 LED 有不同的相位
			uint32_t color = get_rainbow_color(i + led_id * step);
			WS2812_Set_Color(led_id, color);

			// 如果模式更改，立即退出
			if (fan_config.color_mode != RAINBOW)
				return;
		}
		WS2812_Update();

		uint16_t delay_ms = speed_to_delay(speed);
		HAL_Delay(delay_ms);
	}
}

/**
 * @brief  呼吸灯循环
 * @param  speed： 变换速度 (1-100)
 * @param  step：  步幅 (1-50，值越大变化越快)
 */
void breathing_light(uint8_t speed, uint8_t step)
{
	uint32_t color;
	uint8_t r_base, g_base, b_base;

	if (step == 0 || step > 255)
		step = 1; // 默认步长

	for (int i = 0; i <= 0xFF; i += step) { // 循环所有颜色
		color = get_rainbow_color(i * step);
		WS2812_color2rgb(color, &r_base, &g_base, &b_base);

		// 渐亮（0 → 255）
		for (int j = 0; j <= 0xFF; j += step) {
			// 应用亮度到 RGB
			uint8_t r = (r_base * j) >> 8; // 等同于 r_base * （j / 256），即逐渐缩放 r_base
			uint8_t g = (g_base * j) >> 8; // 原本应该 / 255，但位移运算更快
			uint8_t b = (b_base * j) >> 8;

			// 设置所有 LED 为当前颜色
			WS2812_Set_All_Color(WS2812_rgb2color(r, g, b));
			WS2812_Update();
			HAL_Delay(speed_to_delay(speed));

			// 如果模式更改，立即退出
			if (fan_config.color_mode != BREATHING)
				return;
		}

		// 渐暗（254 → 0）
		for (int j = 0xFE - step; j >= 0; j -= step) {
			uint8_t r = (r_base * j) >> 8;
			uint8_t g = (g_base * j) >> 8;
			uint8_t b = (b_base * j) >> 8;
			WS2812_Set_All_Color(WS2812_rgb2color(r, g, b));
			WS2812_Update();
			HAL_Delay(speed_to_delay(speed));

			// 如果模式更改，立即退出
			if (fan_config.color_mode != BREATHING)
				return;
		}
	}
}

// =============== 内部函数 ================
/**
 * @brief  Stop PWM
 */
inline void WS2812_Stop_PWM(void) {
	HAL_TIM_PWM_Stop_DMA(&WS2812_TIM, WS2812_CHANNEL);
	__HAL_TIM_SetCounter(&WS2812_TIM, 0);
}

/**
 * @brief  Start PWM
 */
inline void WS2812_Start_PWM(void) {
	HAL_TIM_PWM_Start_DMA(&WS2812_TIM, WS2812_CHANNEL, (uint32_t *)WS2812_Buf, sizeof(WS2812_Buf) / sizeof(uint16_t));
}

/**
 * @brief  单个灯数据更新
 * @param  index:灯的标号
 * @param  Color:颜色24bit of RGB888
 * @return None
 */
void WS2812_Set_Color(uint8_t index, uint32_t Color)
{
	for (uint8_t i = 0; i < COLOR_LEN; i++) {
	    if (Color & (0x800000 >> i)) { // 三个字节，80 00 00 从最高位开始（第 23 位）
	        WS2812_Buf[COLOR_LEN * index + i] = High_Code;
	    } else {
	        WS2812_Buf[COLOR_LEN * index + i] = Low_Code;
	    }
	}
}

/**
 * @brief  所有灯数据更新
 * @param  Color:颜色24bit of RGB888
 * @return None
 */
void WS2812_Set_All_Color(uint32_t Color)
{
	for(uint8_t index = 0; index < LED_TOTAL; index++)
		WS2812_Set_Color(index, Color);
}

/**
 * @brief  rgb颜色转换为24bit格式(COLOR Order: Green Red Blue)
 * @param  red 8bit颜色数据
 * @param  green 8bit颜色数据
 * @param  blue 8bit颜色数据
 * @return 24bit(RGB888) color in a word(uint32_t)
 */
inline uint32_t WS2812_rgb2color(uint8_t red, uint8_t green, uint8_t blue)
{
	return (green << 16) | (red << 8) | blue;
}

/**
 * @brief  24bit颜色转换为RGB
 * @param  color: 24bit颜色
 * @param  r: 红色亮度（0-255）
 * @param  g: 绿色亮度（0-255）
 * @param  b: 蓝色亮度（0-255）
 */
void WS2812_color2rgb(uint32_t color, uint8_t *red, uint8_t *green, uint8_t *blue)
{
	*green = (color >> 16) & 0xFF;
	*red = (color >> 8) & 0xFF;
	*blue = color & 0xFF;
}

/**
 * @brief  更新 LED 颜色
 */
void WS2812_Update(void) {

	// add CodeReset
	WS2812_Buf[LED_TOTAL * COLOR_LEN] = CodeReset;

	WS2812_Stop_PWM();
	WS2812_Start_PWM();
}

/**
 * @brief  生成 RGB 值
 * @param  phase 是当前相位角（单位为弧度）（0-255），每次递增实现颜色渐变
 * 			phase 取值越接近，颜色越相近
 */
uint32_t get_rainbow_color(int phase)
{
	float frequency = 0.1; // 变动频率（值越小越慢）
	int center = 128;
	int width = 127;

	if (phase >= 628)
		phase = 0;  // 手动循环

	// sinf 是周期函数（周期为 2π），当 frequency * phase 增加 2π 时，颜色会重复
	uint8_t r = sinf(frequency * phase + 0) * width + center;
	uint8_t g = sinf(frequency * phase + 2) * width + center;
	uint8_t b = sinf(frequency * phase + 4) * width + center;
	return WS2812_rgb2color(r, g, b);
}

/**
 * @brief 将速度等级（1-100）转换为延时时间（ms）
 * @param speed 速度等级（1=最慢，100=最快）
 * @return 对应的延时时间（10ms-100ms）
 */
uint16_t speed_to_delay(uint8_t speed) {
    // 约束速度范围（1-100）
    if (speed < 1)
    	speed = 1;
    if (speed > 100)
    	speed = 100;

    // 线性映射：speed=1 → 100ms, speed=100 → 10ms
    return 100 - (speed - 1) * (100 - 10) / 99;
}
