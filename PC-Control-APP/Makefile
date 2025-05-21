# 项目名称
TARGET = fan_controller

# 编译器
CC = gcc

# 编译选项
CFLAGS = -Wall -Wextra -std=c11 -O2

# 源文件 (自动扫描当前目录的.c文件)
SRCS = $(wildcard *.c)

# 头文件目录
INCLUDES = -I.

# 构建目录
BUILD_DIR = build

# 目标文件 (自动转换到build目录)
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

# 默认目标
all: $(TARGET)

# 创建构建目录
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# 链接目标 (输出到根目录)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

# 编译规则 (中间文件到build目录)
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# 清理
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# 声明伪目标
.PHONY: all clean install uninstall