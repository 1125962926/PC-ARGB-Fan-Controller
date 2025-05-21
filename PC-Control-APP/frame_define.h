/*
 * @Author: Li RF
 * @Date: 2025-05-20 09:22:25
 * @LastEditors: Li RF
 * @LastEditTime: 2025-05-21 11:29:43
 * @Description: 
 * Email: 1125962926@qq.com
 * Copyright (c) 2025 Li RF, All Rights Reserved.
 */
#ifndef FRAME_DEFINE_H
#define FRAME_DEFINE_H

#include <stdint.h>

// 帧定义
#define FRAME_HEADER_1      0xAA
#define FRAME_HEADER_2      0x55
#define FRAME_TAIL_1        0x0D
#define FRAME_TAIL_2        0x0A

// 部分数据的最大值
#define MAX_STEP 10
#define MAX_SPEED 100
#define MAX_FAN_ID 2
#define MAX_RGB_VALUE 255
#define MAX_COLOR_MODE 4

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
#define FRAME_DATA_LENGH_MAX 0x04

// 风扇模式枚举
enum Fan_Mode_List {
    CLOSE       = 0,
    RAINBOW     = 1,
    BREATHING   = 2,
    SINGLE      = 3,
};

extern uint8_t frame_buffer[];

void print_help(char *argv_0);
int is_valid_hex(const char *str);
uint8_t hex_to_byte(const char *hex);
int check_range(int value, int min, int max, const char *name);
void print_frame(int length);
int parse_command_line(int argc, char *argv[]);
int build_frame(uint8_t cmd, uint8_t *data, int data_length);

#endif