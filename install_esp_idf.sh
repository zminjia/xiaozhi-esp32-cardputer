#!/bin/bash

# ESP-IDF 安装脚本
# 用于修复 ESP-IDF 路径问题

set -e

ESP_IDF_PATH="/Volumes/Michael/Coding/cardputer/esp-idf"
ESP_IDF_VERSION="v5.4.1"

echo "=========================================="
echo "ESP-IDF 安装脚本"
echo "=========================================="
echo ""
echo "安装路径: $ESP_IDF_PATH"
echo "版本: $ESP_IDF_VERSION"
echo ""

# 检查目录是否存在
if [ -d "$ESP_IDF_PATH" ]; then
    echo "⚠️  目录已存在: $ESP_IDF_PATH"
    read -p "是否删除并重新安装? (y/n) " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo "删除旧目录..."
        rm -rf "$ESP_IDF_PATH"
    else
        echo "取消安装"
        exit 1
    fi
fi

# 创建父目录
mkdir -p "$(dirname "$ESP_IDF_PATH")"
cd "$(dirname "$ESP_IDF_PATH")"

# 克隆 ESP-IDF
echo "正在克隆 ESP-IDF..."
git clone --recursive https://github.com/espressif/esp-idf.git "$ESP_IDF_PATH"
cd "$ESP_IDF_PATH"

# 切换到指定版本
echo "切换到版本 $ESP_IDF_VERSION..."
git checkout "$ESP_IDF_VERSION"
git submodule update --init --recursive

# 安装工具
echo "正在安装 ESP-IDF 工具（仅 ESP32-S3）..."
./install.sh esp32s3

# 验证安装
echo ""
echo "验证安装..."
. ./export.sh
idf_version=$(idf.py --version)
echo "✅ ESP-IDF 安装成功!"
echo "版本信息: $idf_version"
echo ""
echo "=========================================="
echo "安装完成!"
echo "=========================================="
echo ""
echo "下一步操作:"
echo "1. 在 Cursor/VSCode 中重新加载窗口 (Cmd+Shift+P -> Developer: Reload Window)"
echo "2. 或者运行以下命令激活 ESP-IDF 环境:"
echo "   cd $ESP_IDF_PATH"
echo "   . ./export.sh"
echo ""

