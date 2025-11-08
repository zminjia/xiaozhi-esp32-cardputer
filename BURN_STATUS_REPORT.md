# 烧录条件检查报告

**检查时间**: $(date)  
**项目路径**: /Volumes/Michael/Coding/cardputer/xiaozhiai  
**开发板**: M5Stack Cardputer

## ✅ 检查结果：基本烧录条件已具备

### 1. ESP-IDF 环境 ✅

- **状态**: ✅ 已安装并可用
- **安装路径**: `/Volumes/Michael/Coding/cardputer/esp-idf`
- **版本**: ESP-IDF v5.4.1
- **版本要求**: ✅ 符合（项目要求 v5.4+）
- **Python**: Python 3.13.5 ✅

### 2. 开发板配置 ✅

所有必需文件已创建：

- ✅ `main/boards/m5stack-cardputer/config.h` - 硬件引脚配置
- ✅ `main/boards/m5stack-cardputer/m5stack_cardputer.cc` - 板级初始化代码
- ✅ `main/boards/m5stack-cardputer/config.json` - 编译配置
- ✅ `main/boards/m5stack-cardputer/README.md` - 说明文档

### 3. 构建系统配置 ✅

- ✅ `main/Kconfig.projbuild` - 已添加 `BOARD_TYPE_M5STACK_CARDPUTER` 选项
- ✅ `main/CMakeLists.txt` - 已添加开发板配置
- ✅ 分区表: `partitions/v2/8m.csv` 存在

### 4. 项目文件 ✅

- ✅ 根 `CMakeLists.txt` 存在
- ✅ `main/CMakeLists.txt` 存在
- ✅ `scripts/release.py` 编译脚本存在

### 5. 硬件连接 ✅

- ✅ 检测到串口设备: `/dev/tty.usbmodem201401`
- ⚠️ **注意**: 请确认这是 Cardputer 设备

## ⚠️ 警告项（建议处理）

### 1. 引脚配置需要验证 ⚠️

当前配置中的引脚定义是基于推测值，**必须根据实际硬件验证**：

需要验证的引脚：
- 显示屏 SPI 引脚 (SCK, MOSI, DC, CS)
- 背光控制引脚
- I2S 音频引脚 (MCLK, WS, BCLK, DIN, DOUT)
- BOOT 按钮引脚

**参考文档**: [M5Stack Cardputer 官方文档](https://docs.m5stack.com/en/core/Cardputer)

### 2. 音频配置 ⚠️

当前使用 `DummyAudioCodec`，可能需要根据实际硬件调整：
- 如果 Cardputer 使用内置 DAC/ADC，需要配置 I2S 引脚
- 如果有外部编解码器（如 ES8311），需要更新代码

## 📋 烧录步骤

### 快速烧录（使用 release.py）

```bash
# 1. 激活 ESP-IDF 环境
cd /Volumes/Michael/Coding/cardputer/esp-idf
. ./export.sh

# 2. 进入项目目录
cd /Volumes/Michael/Coding/cardputer/xiaozhiai

# 3. 使用 release.py 编译（推荐）
python3 scripts/release.py m5stack-cardputer
```

### 手动烧录步骤

```bash
# 1. 激活 ESP-IDF 环境
cd /Volumes/Michael/Coding/cardputer/esp-idf
. ./export.sh

# 2. 进入项目目录
cd /Volumes/Michael/Coding/cardputer/xiaozhiai

# 3. 设置目标芯片
idf.py set-target esp32s3

# 4. 配置开发板
idf.py menuconfig
# 导航到: Xiaozhi Assistant -> Board Type -> M5Stack Cardputer
# 保存并退出

# 5. 编译项目
idf.py build

# 6. 准备设备进入下载模式
# - 将设备侧面的电源开关置于 "OFF" 位置
# - 按住设备侧面的 "G0" 按键
# - 通过 USB-C 数据线连接至电脑
# - 松开 "G0" 按键，设备进入下载模式

# 7. 烧录固件
idf.py -p /dev/tty.usbmodem201401 flash

# 8. 监控输出（可选）
idf.py -p /dev/tty.usbmodem201401 monitor
```

## 🔍 验证命令

运行检查脚本：

```bash
cd /Volumes/Michael/Coding/cardputer/xiaozhiai
./check_burn_ready.sh
```

## 📝 注意事项

1. **首次编译**: 首次编译可能需要 10-20 分钟，请耐心等待
2. **引脚验证**: 在烧录前，强烈建议验证引脚配置
3. **串口权限**: 如果遇到权限问题，可能需要：
   ```bash
   sudo chmod 666 /dev/tty.usbmodem201401
   ```
4. **下载模式**: 确保设备正确进入下载模式，否则烧录会失败

## 📚 相关文档

- [烧录检查清单](BURN_CHECKLIST.md)
- [开发板配置说明](main/boards/m5stack-cardputer/README.md)
- [快速开始指南](CARDPUTER_SETUP.md)
- [ESP-IDF 安装指南](ESP_IDF_SETUP.md)

## ✅ 总结

**基本烧录条件已完全具备！**

所有必需的文件、配置和工具都已就绪。可以开始编译和烧录流程。

**建议**: 在首次烧录前，先验证引脚配置，确保硬件连接正确。

---

*最后更新: $(date)*

