/*
 * usart_ocess.c
 *
 *  Created on: May 15, 2025
 *      Author: li-rongfu
 */
#include "usart_process.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

uint8_t uart_rx_buf[USART_BUF_SIZE];

// 颜色配置初始化
DeviceConfig fan_config = {
    .color_speed = 60,
    .color_mode = 1,
    .color_step = 5,
	.fan_id = 1,
	.color_r = 0,
	.color_g = 255,
	.color_b = 255,
	.fan_speed = 30,
};

// 数据帧参数初始化
FrameParser frame_parser = {0};

/**
 * @brief  重置数据帧参数
 */
void reset_frame_parser(void)
{
    frame_parser.length = 0;
    frame_parser.count = 0;
    frame_parser.cmd = 0;
    frame_parser.state = 0;  // 0 代表等待帧头1
}

/**
 * @brief  处理数据帧
 * @param  data：   命令 + 数据
 * @param  length： 数据长度
 */
void process_frame(uint8_t cmd, uint8_t *data, uint8_t length)
{
    switch(cmd) {
        case CMD_SET_COLOR_MODE: {
        	if(length == DATA_LENTH_COLOR_MODE) { // 1 参数
        		fan_config.color_mode = data[0];
				printf("Set color mode to %d\n", data[0]);
			} else
				printf("DATA_LENTH do not match!\n");
			break;
        }
        case CMD_SET_COLOR_SPEED: {
        	if(length == DATA_LENTH_COLOR_SPEED) {
        		fan_config.color_speed = data[0];
				printf("Set color speed to %d\n", data[0]);
			} else
				printf("DATA_LENTH do not match!\n");
			break;
        }
        case CMD_SET_COLOR_STEP: {
            if(length == DATA_LENTH_COLOR_STEP) {
            	fan_config.color_step = data[0];
                printf("Set color step to %d\n", fan_config.color_step);
            } else
				printf("DATA_LENTH do not match!\n");
            break;
        }
        case CMD_SET_RGB_COLOR: {
            if(length == DATA_LENTH_RGB_COLOR) {
            	fan_config.fan_id = data[0];
            	fan_config.color_r = data[1];
            	fan_config.color_g = data[2];
            	fan_config.color_b = data[3];
                printf("Set fan %d RGB color to R:%d, G:%d, B:%d\n", fan_config.fan_id,
                		fan_config.color_r, fan_config.color_g, fan_config.color_b);
            } else
				printf("DATA_LENTH do not match!\n");
            break;
        }
        case CMD_SET_FAN_SPEED: {
        	if(length == DATA_LENTH_FAN_SPEED) {
        		fan_config.fan_speed = data[0];
        		printf("Set fan speed to %d\n", fan_config.fan_speed);
        	} else
				printf("DATA_LENTH do not match!\n");
        	break;
        }
        default:
            printf("Unknown command: 0x%02X\n", cmd);
            break;
    }
}

/**
 * @brief  接收数据帧
 * 			数据帧格式： 帧头1  帧头2  数据长度  命令  数据  帧尾1  帧尾2
 * 			例：AA 55 01 02 50 0D 0A
 * 			01 表示数据的长度
 * 			02 表示命令，设置颜色速度
 * 			50 表示数据，速度值
 * @param  length： 数据长度
 */
void receive_frame(uint16_t length)
{
	for(int i=0; i< length; i++) {
		// 从串口缓冲区取出数据
		uint8_t byte = uart_rx_buf[i];
		switch(frame_parser.state) {
			case 0: {// 等待帧头1
				if(byte == FRAME_HEADER_1)
					frame_parser.state = 1;
				break;
			}
			case 1: {// 等待帧头2
				if(byte == FRAME_HEADER_2)
					frame_parser.state = 2;
				else
					reset_frame_parser();
				break;
			}
			case 2: {// 等待数据长度
				frame_parser.length = byte;
				frame_parser.state = 3;
				break;
			}
			case 3: {// 等待命令
				frame_parser.cmd = byte;
				if(frame_parser.length > 0)
					frame_parser.state = 4; // 有参数，进入参数接收状态
				else
					frame_parser.state = 5; // 无参数，直接等待帧尾
				break;
			}
			case 4: {// 接收参数
				frame_parser.data[frame_parser.count++] = byte;
				if(frame_parser.count >= frame_parser.length)
					frame_parser.state = 5; // 参数接收完成，等待帧尾
				break;
			}
			case 5: {// 等待帧尾1
				if(byte == FRAME_TAIL_1)
					frame_parser.state = 6;
				else
					reset_frame_parser();
				break;
			}
			case 6: {// 等待帧尾2
				if(byte == FRAME_TAIL_2)
					// 完整帧接收完成，处理数据
					process_frame(frame_parser.cmd, frame_parser.data, frame_parser.length);
				reset_frame_parser();
				break;
			}
		}
	}
	memset(uart_rx_buf, 0, sizeof(uart_rx_buf));
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
	if(huart->Instance == USART_INSTANCE) {
		receive_frame(Size);
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, uart_rx_buf, USART_BUF_SIZE);
	}
}


