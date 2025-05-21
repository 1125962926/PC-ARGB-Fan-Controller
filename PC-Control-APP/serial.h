/*
 * @Author: Li RF
 * @Date: 2025-05-20 09:17:12
 * @LastEditors: Li RF
 * @LastEditTime: 2025-05-21 13:22:05
 * @Description: 
 * Email: 1125962926@qq.com
 * Copyright (c) 2025 Li RF, All Rights Reserved.
 */
#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

int open_serial_port(const char *device);
void close_serial_port(void);
int send_and_receive(const unsigned char *tx_data, int tx_len, unsigned char *rx_buf, int rx_buf_size);


#endif