/*
 * @Author: Li RF
 * @Date: 2025-05-20 09:17:36
 * @LastEditors: Li RF
 * @LastEditTime: 2025-05-21 13:30:47
 * @Description: 
 * Email: 1125962926@qq.com
 * Copyright (c) 2025 Li RF, All Rights Reserved.
 */
#include <stdio.h>
#include "serial.h"
#include "frame_define.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_help(argv[0]);
        return -1;
    }

    const char *device = "/dev/ttyCH341USB0";
    
    // 打开串口
    int ret = open_serial_port(device);
    if (ret != 0) {
        printf("Error: 无法打开串口 %s\n", device);
        return -1;
    }

    // 解析命令行参数
    int frame_length = parse_command_line(argc, argv);
    if (frame_length <= 0) {
        printf("Error: 解析命令行错误, frame_length: %d\n", frame_length);
        close_serial_port();
        return -1;
    }

    // 发送并接收数据
    unsigned char rx_buf[256];
    int received = send_and_receive(frame_buffer, frame_length, rx_buf, sizeof(rx_buf));
    if (received <= 0) {
        printf("Error: 收发数据错误\n");
        close_serial_port();
        return -1;
    }

    // 关闭串口
    close_serial_port();

    return 0;
}

