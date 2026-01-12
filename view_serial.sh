#!/bin/bash
# 简单的串口查看脚本

PORT="/dev/cu.usbmodem201401"
BAUD=115200

echo "连接到串口: $PORT @ $BAUD"
echo "按 Ctrl+C 退出"
echo "----------------------------------------"

# 使用 screen 查看串口（如果可用）
if command -v screen &> /dev/null; then
    screen $PORT $BAUD
# 或者使用 cat（简单但功能有限）
elif [ -c "$PORT" ]; then
    stty -f $PORT $BAUD raw -echo
    cat $PORT
else
    echo "错误: 无法访问串口 $PORT"
    echo "请确保设备已连接"
    exit 1
fi
