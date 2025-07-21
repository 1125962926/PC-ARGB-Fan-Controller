/*
 * @Author: Li RF
 * @Date: 2025-05-20 09:32:13
 * @LastEditors: Li RF
 * @LastEditTime: 2025-05-21 11:20:48
 * @Description: 
 * Email: 1125962926@qq.com
 * Copyright (c) 2025 Li RF, All Rights Reserved.
 */
#include "frame_define.h"
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

// 数据帧缓冲区
// 总帧长度: 帧头(2) + 数据长度(1) + 命令(1) + 数据 + 帧尾(2)
// 去除数据部分的长度（固定不变）
#define FRAME_LENGH_NO_DATA (2 + 1 + 1 + 2)
uint8_t frame_buffer[FRAME_LENGH_NO_DATA + FRAME_DATA_LENGH_MAX];

/**
 * @Description: 打印帮助信息
 * @param {char} *argv_0: 可执行文件名
 * @return {*}
 */
void print_help(char *argv_0) {
    printf("Usage:\n");
    printf("  Set color mode:   %s -m <mode> (0-3: CLOSE, RAINBOW, BREATHING, SINGLE)\n", argv_0);
    printf("  Set color speed:  %s -cs <speed> (1-100)\n", argv_0);
    printf("  Set color step:   %s -t <step> (1-10)\n", argv_0);
    printf("  Set RGB color:    %s -r <fan_id> <R(hex)> <G(hex)> <B(hex)>\n", argv_0);
    printf("  Set fan speed:    %s -fs <speed> (1-100)\n", argv_0);
    printf("                    Example: %s -r 1 ff 00 00\n", argv_0);
}

/**
 * @Description: 检查16进制字符串是否有效
 * @param {char} *str: 16进制字符串
 * @return {*}
 */
int is_valid_hex(const char *str) {
    if (str == NULL || strlen(str) == 0 || strlen(str) > 2) {
        return 0;
    }
    
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isxdigit(str[i])) {
            return 0;
        }
    }
    return 1;
}

/**
 * @Description: 将16进制字符串转换为字节
 * @param {char} *hex: 16进制字符串
 * @return {*}
 */
uint8_t hex_to_byte(const char *hex) {
    // strtol 用于将字符串（string）转换为长整型（long integer）
    // 支持不同的进制（如 2 进制、8 进制、10 进制、16 进制等）
    return (uint8_t)strtol(hex, NULL, 16); // 使用 16 进制
}

/**
 * @Description: 检查参数是否在有效范围内
 * @param {int} value: 参数
 * @param {int} min: 最小值
 * @param {int} max: 最大值
 * @param {char} *name: 参数名称
 * @return {*}
 */
int check_range(int value, int min, int max, const char *name) {
    if (value < min || value > max) {
        printf("Error: %s must be between %d and %d\n", name, min, max);
        return 0;
    }
    return 1;
}

/**
 * @Description: 打印数据帧
 * @param {uint8_t} *frame: 数据帧
 * @param {int} length: 数据帧长度
 * @return {*}
 */
void print_frame(int length) {
    printf("Encoded frame: \n");
    for (int i = 0; i < length; i++) {
        printf("%02x ", frame_buffer[i]);
    }
    printf("\n");
}

/**
 * @Description: 构建数据帧
 * @param {uint8_t} cmd
 * @param {uint8_t} *data
 * @param {int} data_length
 * @return {*} 帧长
 */
int build_frame(uint8_t cmd, uint8_t *data, int data_length) {
    int frame_length = FRAME_LENGH_NO_DATA + data_length;

    // 填充帧头
    frame_buffer[0] = FRAME_HEADER_1;
    frame_buffer[1] = FRAME_HEADER_2;
    
    // 数据长度
    frame_buffer[2] = data_length;
    
    // 命令
    frame_buffer[3] = cmd;
    
    // 数据
    memcpy(&frame_buffer[4], data, data_length);
    
    // 帧尾
    frame_buffer[frame_length - 2] = FRAME_TAIL_1;
    frame_buffer[frame_length - 1] = FRAME_TAIL_2;
    
    return frame_length;
}

/**
 * @Description: 解析命令行参数
 * @param {int} argc: 
 * @param {char} *argv: 
 * @return {*} 帧长
 */
int parse_command_line(int argc, char *argv[]) {
    int frame_length = 0;
    // 设置颜色模式
    if (strcmp(argv[1], "-m") == 0) {
        if (argc != DATA_LENTH_COLOR_MODE + 2) {
            printf("Error: 设置模式需要 1 个参数\n");
            return -1;
        }
        
        int mode = atoi(argv[2]);
        if (!check_range(mode, 0, MAX_COLOR_MODE, "mode")) return -1;
        
        uint8_t data[] = {mode};
        frame_length = build_frame(CMD_SET_COLOR_MODE, data, sizeof(data));
        
    } 
    // 设置颜色速度
    else if (strcmp(argv[1], "-cs") == 0) {  
        if (argc != DATA_LENTH_COLOR_SPEED + 2) {
            printf("Error: 设置速度需要 1 个参数\n");
            return -1;
        }
        
        int speed = atoi(argv[2]);
        if (!check_range(speed, 1, MAX_SPEED, "speed")) return -1;
        
        uint8_t data[] = {speed};
        frame_length = build_frame(CMD_SET_COLOR_SPEED, data, sizeof(data));
        
    } 
    // 设置步长
    else if (strcmp(argv[1], "-t") == 0) {
        if (argc != DATA_LENTH_COLOR_STEP + 2) {
            printf("Error: 设置步幅需要 1 个参数\n");
            return -1;
        }
        
        int step = atoi(argv[2]);
        if (!check_range(step, 1, MAX_STEP, "step")) return -1;
        
        uint8_t data[] = {step};
        frame_length = build_frame(CMD_SET_COLOR_STEP, data, sizeof(data));
        
    } 
    // 设置RGB颜色
    else if (strcmp(argv[1], "-r") == 0) {
        if (argc != DATA_LENTH_RGB_COLOR + 2) {
            printf("Error: 设置RGB颜色需要 4 个参数 (fan_id R G B)\n");
            return -1;
        }

        // 检查 R G B 是否为有效的16进制
        for (int i = 3; i < 6; i++) {
            if (!is_valid_hex(argv[i])) {
                printf("Error: Argument %d '%s' is not a valid hex value (00-FF)\n", i-1, argv[i]);
                return -1;
            }
        }
        
        int fan_id = atoi(argv[2]);
        uint8_t r = hex_to_byte(argv[3]);
        uint8_t g = hex_to_byte(argv[4]);
        uint8_t b = hex_to_byte(argv[5]);
        
        if (!check_range(fan_id, 1, MAX_FAN_ID, "fan_id")) return -1;
        
        uint8_t data[] = {fan_id, r, g, b};
        frame_length = build_frame(CMD_SET_RGB_COLOR, data, sizeof(data));
        
    }
    // 设置风扇转度
    else if (strcmp(argv[1], "-fs") == 0) {  
        if (argc != DATA_LENTH_FAN_SPEED + 2) {
            printf("Error: 设置速度需要 1 个参数\n");
            return -1;
        }
        
        int speed = atoi(argv[2]);
        if (!check_range(speed, 1, MAX_SPEED, "speed")) return -1;
        
        uint8_t data[] = {speed};
        frame_length = build_frame(CMD_SET_FAN_SPEED, data, sizeof(data));
        
    }
    else {
        printf("Error: unknown command\n");
        print_help(argv[0]);
        return -1;
    }
    return frame_length;
}

