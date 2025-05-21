#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>      // 文件控制定义（如 O_RDWR）
#include <unistd.h>     // Unix 标准函数（如 read/write）
#include <termios.h>    // 串口配置（波特率等）
#include <errno.h>      // 错误号定义

static int fd;
#ifndef CRTSCTS
#define CRTSCTS 020000000000  /* Flow control.  八进制值，等同于 0x80000000*/
#endif
/**
 * @Description: 打开串口并返回文件描述符
 * @param {char} *device: 串口设备
 * @return {*} 成功返回 0，失败返回 -1
 */
int open_serial_port(const char *device) {
    fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        perror("无法打开串口设备");
        return -1;
    }

    // 恢复串口阻塞模式（等待数据）
    fcntl(fd, F_SETFL, 0);

    // 配置串口参数
    struct termios options;
    tcgetattr(fd, &options);

    // 设置波特率 115200 
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    // 8N1（8数据位，无校验，1停止位）
    options.c_cflag &= ~PARENB;   // 无校验
    options.c_cflag &= ~CSTOPB;   // 1停止位
    options.c_cflag &= ~CSIZE;    // 清除数据位掩码
    options.c_cflag |= CS8;       // 8数据位

    // 启用接收，忽略 modem 控制线
    options.c_cflag |= (CLOCAL | CREAD);

    // 关闭流控
    options.c_cflag &= ~CRTSCTS;  // 无硬件流控
    options.c_iflag &= ~(IXON | IXOFF | IXANY);  // 无软件流控

    // 原始输入模式（不处理特殊字符）
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    // 原始数据(RAW)输出
    options.c_oflag &= ~OPOST;

    // 最小读取字符数和超时（单位：0.1秒）
    options.c_cc[VMIN] = 1;      // 至少读取1个字符
    options.c_cc[VTIME] = 10;    // 超时 1秒（10 * 0.1s）

    // 应用配置
    if (tcsetattr(fd, TCSANOW, &options) != 0) {
        perror("串口配置失败");
        close(fd);
        return -1;
    }
    return 0;
}

/**
 * @Description: 关闭串口
 * @return {*}
 */
void close_serial_port(void) {
    close(fd);
}

/**
 * @Description: 发送数据并接收返回
 * @param {unsigned char} *tx_data
 * @param {int} tx_len
 * @param {unsigned char} *rx_buf
 * @param {int} rx_buf_size
 * @return {*} 接收数据长度
 */
int send_and_receive(const unsigned char *tx_data, int tx_len, unsigned char *rx_buf, int rx_buf_size) {
    // 发送数据
    int written = write(fd, tx_data, tx_len);
    if (written != tx_len) {
        perror("发送数据失败");
        return -1;
    }
    printf("已发送 %d 字节: ", written);
    for (int i = 0; i < written; i++) {
        printf("%02X ", tx_data[i]);
    }
    printf("\n");

    // 接收数据
    int received = read(fd, rx_buf, rx_buf_size);
    if (received < 0) {
        perror("接收数据失败");
        return -1;
    }
    // .* 表示精度（precision）由后面的参数动态指定（这里用 length 变量控制）
    printf("收到 %d 字节: ", received);
    printf("%.*s", received, rx_buf);
    printf("\n");

    return received;
}

