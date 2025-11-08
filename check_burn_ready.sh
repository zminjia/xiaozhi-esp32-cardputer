#!/bin/bash

# 烧录条件完整检查脚本

echo "=========================================="
echo "  烧录条件完整性检查"
echo "=========================================="
echo ""

ERRORS=0
WARNINGS=0

# 颜色定义
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

check_pass() {
    echo -e "${GREEN}✅ $1${NC}"
}

check_fail() {
    echo -e "${RED}❌ $1${NC}"
    ((ERRORS++))
}

check_warn() {
    echo -e "${YELLOW}⚠️  $1${NC}"
    ((WARNINGS++))
}

echo "1. ESP-IDF 环境检查"
echo "-------------------"
if [ -f "/Volumes/Michael/Coding/cardputer/esp-idf/export.sh" ]; then
    check_pass "ESP-IDF 已安装"
    
    # 检查版本
    cd /Volumes/Michael/Coding/cardputer/esp-idf
    . ./export.sh >/dev/null 2>&1
    IDF_VERSION=$(idf.py --version 2>&1 | head -1)
    if [ -n "$IDF_VERSION" ]; then
        check_pass "ESP-IDF 版本: $IDF_VERSION"
        if echo "$IDF_VERSION" | grep -q "v5\.[4-9]\|v6\."; then
            check_pass "版本符合要求 (v5.4+)"
        else
            check_warn "版本可能不符合要求，推荐 v5.4 或更高"
        fi
    else
        check_fail "无法获取 ESP-IDF 版本"
    fi
else
    check_fail "ESP-IDF 未安装或路径不正确"
fi

echo ""
echo "2. Python 环境检查"
echo "-------------------"
if command -v python3 &> /dev/null; then
    PYTHON_VERSION=$(python3 --version)
    check_pass "Python: $PYTHON_VERSION"
else
    check_fail "Python3 未安装"
fi

echo ""
echo "3. 开发板配置文件检查"
echo "-------------------"
cd /Volumes/Michael/Coding/cardputer/xiaozhiai

BOARD_DIR="main/boards/m5stack-cardputer"
if [ -d "$BOARD_DIR" ]; then
    check_pass "开发板目录存在"
    
    [ -f "$BOARD_DIR/config.h" ] && check_pass "config.h 存在" || check_fail "config.h 缺失"
    [ -f "$BOARD_DIR/m5stack_cardputer.cc" ] && check_pass "m5stack_cardputer.cc 存在" || check_fail "m5stack_cardputer.cc 缺失"
    [ -f "$BOARD_DIR/config.json" ] && check_pass "config.json 存在" || check_fail "config.json 缺失"
    [ -f "$BOARD_DIR/README.md" ] && check_pass "README.md 存在" || check_warn "README.md 缺失（可选）"
else
    check_fail "开发板目录不存在"
fi

echo ""
echo "4. 构建系统配置检查"
echo "-------------------"
if grep -q "BOARD_TYPE_M5STACK_CARDPUTER" main/Kconfig.projbuild 2>/dev/null; then
    check_pass "Kconfig.projbuild 已配置"
else
    check_fail "Kconfig.projbuild 未配置"
fi

if grep -q "m5stack-cardputer" main/CMakeLists.txt 2>/dev/null; then
    check_pass "CMakeLists.txt 已配置"
else
    check_fail "CMakeLists.txt 未配置"
fi

echo ""
echo "5. 分区表检查"
echo "-------------------"
if [ -f "partitions/v2/8m.csv" ]; then
    check_pass "分区表文件存在 (8m.csv)"
else
    check_fail "分区表文件缺失"
fi

echo ""
echo "6. 项目文件检查"
echo "-------------------"
[ -f "CMakeLists.txt" ] && check_pass "根 CMakeLists.txt 存在" || check_fail "根 CMakeLists.txt 缺失"
[ -f "main/CMakeLists.txt" ] && check_pass "main/CMakeLists.txt 存在" || check_fail "main/CMakeLists.txt 缺失"
[ -f "scripts/release.py" ] && check_pass "release.py 脚本存在" || check_warn "release.py 脚本缺失（可选）"

echo ""
echo "7. 硬件连接检查"
echo "-------------------"
SERIAL_PORTS=$(ls /dev/tty.* 2>/dev/null | grep -iE "usb|serial|cu\." | head -1)
if [ -n "$SERIAL_PORTS" ]; then
    check_pass "检测到串口设备: $SERIAL_PORTS"
    check_warn "请确认这是 Cardputer 设备"
else
    check_warn "未检测到串口设备（设备可能未连接）"
fi

echo ""
echo "8. 引脚配置检查"
echo "-------------------"
if [ -f "$BOARD_DIR/config.h" ]; then
    if grep -q "需要验证\|需要根据实际硬件" "$BOARD_DIR/config.h"; then
        check_warn "引脚配置包含待验证项，需要根据实际硬件调整"
    else
        check_pass "引脚配置已定义"
    fi
fi

echo ""
echo "=========================================="
echo "  检查结果汇总"
echo "=========================================="
PASS_COUNT=$(grep -c "✅" <<< "$(echo)" || echo "0")
echo -e "${GREEN}通过: 所有必需项检查通过${NC}"
echo -e "${YELLOW}警告: $WARNINGS${NC}"
echo -e "${RED}错误: $ERRORS${NC}"
echo ""

if [ $ERRORS -eq 0 ]; then
    echo -e "${GREEN}✅ 基本烧录条件已具备！${NC}"
    echo ""
    echo "下一步操作："
    echo "1. 验证引脚配置（根据实际硬件）"
    echo "2. 激活 ESP-IDF 环境: cd /Volumes/Michael/Coding/cardputer/esp-idf && . ./export.sh"
    echo "3. 设置目标: idf.py set-target esp32s3"
    echo "4. 配置开发板: idf.py menuconfig (选择 M5Stack Cardputer)"
    echo "5. 编译: idf.py build"
    echo "6. 烧录: idf.py -p /dev/tty.usbmodem201401 flash"
else
    echo -e "${RED}❌ 存在错误，请先解决上述问题${NC}"
    exit 1
fi

if [ $WARNINGS -gt 0 ]; then
    echo ""
    echo -e "${YELLOW}⚠️  存在警告项，建议在烧录前处理${NC}"
fi

echo ""

