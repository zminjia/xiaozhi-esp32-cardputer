#!/bin/bash
# 快速启动串口监视器的脚本

# 激活 ESP-IDF 环境
cd /Volumes/Michael/Coding/cardputer/esp-idf
. ./export.sh

# 进入项目目录
cd /Volumes/Michael/Coding/cardputer/xiaozhiai

# 启动串口监视器
idf.py -p /dev/cu.usbmodem201401 monitor
