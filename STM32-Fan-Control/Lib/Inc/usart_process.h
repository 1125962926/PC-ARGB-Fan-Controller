/*
 * usart_process.h
 *
 *  Created on: May 15, 2025
 *      Author: li-rongfu
 */

#ifndef INC_USART_PROCESS_H_
#define INC_USART_PROCESS_H_

#include "usart.h"

#define USART_HANDLE   	huart1
#define USART_INSTANCE 	USART1
#define USART_BUF_SIZE  50

#define FRAME_HEADER_1      0xAA
#define FRAME_HEADER_2      0x55
#define FRAME_TAIL_1        0x0D
#define FRAME_TAIL_2        0x0A
#define MAX_FRAME_LENGTH    32

// 颜色配置
typedef struct {
	__IO uint8_t color_speed; // 1 - 100
	__IO uint8_t color_mode;
	__IO uint8_t color_step; // 1 - 10
	__IO uint8_t fan_speed;
	uint8_t fan_id;
	uint8_t color_r;
	uint8_t color_g;
	uint8_t color_b;
} DeviceConfig;

// 数据帧参数
typedef struct {
    uint8_t data[MAX_FRAME_LENGTH];
    uint8_t length; 	// 数据长度
    uint8_t count; 		// 接收计数
    uint8_t state; 		// 数据帧接收状态
    uint8_t cmd; 		// 数据帧命令类型
} FrameParser;

// 帧命令类型
enum FRAME_CMD_TYPE {
//	CMD_SET_ALL 		= 0x00,
	CMD_SET_COLOR_MODE 	= 0x01,
	CMD_SET_COLOR_SPEED = 0x02,
	CMD_SET_COLOR_STEP	= 0x03,
	CMD_SET_RGB_COLOR	= 0x04, // 只在单色模式生效
	CMD_SET_FAN_SPEED	= 0x05,
};

// 数据长度
enum FRAME_DATA_LENGH {
	DATA_LENTH_COLOR_MODE 	= 0x01,
	DATA_LENTH_COLOR_SPEED  = 0x01,
	DATA_LENTH_COLOR_STEP	= 0x01,
	DATA_LENTH_RGB_COLOR	= 0x04, // fan id + r + g + b
	DATA_LENTH_FAN_SPEED	= 0x01,
};

enum Fan_Mode_List {
	CLOSE 		= 0,
	RAINBOW 	= 1,
	BREATHING 	= 2,
	SINGLE		= 3,
};

extern uint8_t uart_rx_buf[];
extern DeviceConfig fan_config;





#endif /* INC_USART_PROCESS_H_ */
