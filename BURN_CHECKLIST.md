# 烧录条件检查清单

## ✅ 已完成的检查项

### 1. ESP-IDF 环境 ✅
- [x] ESP-IDF 已安装在 `/Volumes/Michael/Coding/cardputer/esp-idf`
- [x] Python 3.13.5 已安装
- [x] ESP-IDF 环境可以激活

### 2. 开发板配置 ✅
- [x] 开发板目录存在: `main/boards/m5stack-cardputer/`
- [x] 配置文件完整:
  - [x] `config.h` - 硬件引脚配置
  - [x] `m5stack_cardputer.cc` - 板级初始化代码
  - [x] `config.json` - 编译配置
  - [x] `README.md` - 说明文档

### 3. 构建系统配置 ✅
- [x] `Kconfig.projbuild` 已添加开发板选项
- [x] `CMakeLists.txt` 已添加开发板配置
- [x] 分区表文件存在: `partitions/v2/8m.csv`

### 4. 硬件连接 ✅
- [x] 检测到串口设备: `/dev/tty.usbmodem201401`
- [x] 设备已连接（需要确认是 Cardputer）

## ⚠️ 需要验证的项

### 1. 引脚配置验证 ⚠️
**状态**: 需要根据实际硬件验证

当前配置的引脚（在 `config.h` 中）：
- 显示屏 SPI 引脚需要验证
- I2S 音频引脚需要验证
- 背光控制引脚需要验证
- BOOT 按钮引脚需要验证

**操作**: 参考 [M5Stack Cardputer 官方文档](https://docs.m5stack.com/en/core/Cardputer) 验证引脚定义

### 2. ESP-IDF 版本 ⚠️
**状态**: 需要确认版本兼容性

- 项目推荐: ESP-IDF v5.4 或更高
- 当前安装: 需要检查版本

**操作**: 
```bash
cd /Volumes/Michael/Coding/cardputer/esp-idf
. ./export.sh
idf.py --version
```

### 3. 音频配置 ⚠️
**状态**: 当前使用 DummyAudioCodec

- 需要验证 Cardputer 的音频硬件配置
- 可能需要配置 I2S 引脚或外部编解码器

**操作**: 根据实际硬件调整 `m5stack_cardputer.cc` 中的音频编解码器

## 📋 烧录前准备步骤

### 步骤 1: 激活 ESP-IDF 环境

```bash
cd /Volumes/Michael/Coding/cardputer/esp-idf
. ./export.sh
```

### 步骤 2: 进入项目目录

```bash
cd /Volumes/Michael/Coding/cardputer/xiaozhiai
```

### 步骤 3: 设置目标芯片

```bash
idf.py set-target esp32s3
```

### 步骤 4: 配置开发板

```bash
idf.py menuconfig
```

在菜单中：
- 导航到: `Xiaozhi Assistant` -> `Board Type`
- 选择: `M5Stack Cardputer`
- 保存并退出

### 步骤 5: 编译项目

```bash
idf.py build
```

### 步骤 6: 准备设备进入下载模式

根据 M5Stack Cardputer 文档：
1. 将设备侧面的电源开关置于 "OFF" 位置
2. 按住设备侧面的 "G0" 按键
3. 通过 USB-C 数据线连接至电脑
4. 松开 "G0" 按键，设备进入下载模式

### 步骤 7: 烧录固件

```bash
# 确认串口号（当前检测到: /dev/tty.usbmodem201401）
idf.py -p /dev/tty.usbmodem201401 flash

# 或者使用 monitor 同时监控输出
idf.py -p /dev/tty.usbmodem201401 flash monitor
```

## 🔍 快速验证命令

运行以下命令快速检查所有条件：

```bash
#!/bin/bash
echo "=== 烧录条件检查 ==="
echo ""

echo "1. ESP-IDF 环境:"
cd /Volumes/Michael/Coding/cardputer/esp-idf && . ./export.sh >/dev/null 2>&1 && idf.py --version 2>&1 | head -1 || echo "❌ ESP-IDF 未激活"

echo ""
echo "2. 开发板配置:"
cd /Volumes/Michael/Coding/cardputer/xiaozhiai
test -f main/boards/m5stack-cardputer/config.h && echo "✅ config.h" || echo "❌ config.h"
test -f main/boards/m5stack-cardputer/m5stack_cardputer.cc && echo "✅ m5stack_cardputer.cc" || echo "❌ m5stack_cardputer.cc"
test -f main/boards/m5stack-cardputer/config.json && echo "✅ config.json" || echo "❌ config.json"

echo ""
echo "3. 构建系统:"
grep -q "BOARD_TYPE_M5STACK_CARDPUTER" main/Kconfig.projbuild && echo "✅ Kconfig" || echo "❌ Kconfig"
grep -q "m5stack-cardputer" main/CMakeLists.txt && echo "✅ CMakeLists" || echo "❌ CMakeLists"

echo ""
echo "4. 分区表:"
test -f partitions/v2/8m.csv && echo "✅ 分区表存在" || echo "❌ 分区表缺失"

echo ""
echo "5. 串口设备:"
ls /dev/tty.* 2>/dev/null | grep -i "usb\|serial\|cu\." | head -1 && echo "✅ 检测到串口" || echo "❌ 未检测到串口"

echo ""
echo "=== 检查完成 ==="
```

## ⚠️ 注意事项

1. **引脚配置**: 当前引脚配置是推测值，必须根据实际硬件验证
2. **音频配置**: 当前使用 DummyAudioCodec，可能需要根据实际硬件调整
3. **串口权限**: 如果遇到权限问题，可能需要将用户添加到 dialout 组
4. **首次编译**: 首次编译可能需要较长时间，请耐心等待

## 📚 相关文档

- [M5Stack Cardputer 官方文档](https://docs.m5stack.com/en/core/Cardputer)
- [开发板配置说明](main/boards/m5stack-cardputer/README.md)
- [快速开始指南](CARDPUTER_SETUP.md)
- [ESP-IDF 安装指南](ESP_IDF_SETUP.md)

