# M5Stack Cardputer 小智 AI 部署指南

## ✅ 已完成的工作

我已经为 M5Stack Cardputer 创建了完整的开发板配置，包括：

1. ✅ **开发板目录结构**: `main/boards/m5stack-cardputer/`
2. ✅ **配置文件** (`config.h`): 定义了硬件引脚和参数
3. ✅ **板级初始化代码** (`m5stack_cardputer.cc`): 实现了显示屏、按钮等初始化
4. ✅ **编译配置** (`config.json`): 指定了目标芯片和分区表
5. ✅ **Kconfig 配置**: 在 `Kconfig.projbuild` 中添加了开发板选项
6. ✅ **CMake 配置**: 在 `CMakeLists.txt` 中添加了构建配置
7. ✅ **说明文档** (`README.md`): 详细的使用和配置说明

## ⚠️ 重要提示

**引脚配置需要验证**: 当前配置中的引脚定义是基于常见 M5Stack 产品的推测，**必须根据 Cardputer 的实际硬件进行验证和调整**。

主要需要验证的引脚：
- 显示屏 SPI 引脚 (SCK, MOSI, DC, CS)
- 背光控制引脚
- I2S 音频引脚 (MCLK, WS, BCLK, DIN, DOUT)
- BOOT 按钮引脚

## 🚀 快速开始

### 1. 验证硬件引脚

首先，请参考 [M5Stack Cardputer 官方文档](https://docs.m5stack.com/en/core/Cardputer) 获取准确的引脚定义，然后修改 `main/boards/m5stack-cardputer/config.h` 中的引脚配置。

### 2. 编译和烧录

#### 方法一：使用 release.py 脚本（推荐）

```bash
cd /Volumes/Michael/Coding/cardputer/xiaozhiai
python scripts/release.py m5stack-cardputer
```

#### 方法二：手动配置

```bash
# 设置目标芯片
idf.py set-target esp32s3

# 进入配置菜单，选择开发板
idf.py menuconfig
# 导航到: Xiaozhi Assistant -> Board Type -> M5Stack Cardputer

# 编译
idf.py build

# 烧录（请替换为你的串口号）
idf.py -p /dev/ttyUSB0 flash monitor
```

### 3. 进入下载模式

根据 M5Stack Cardputer 文档：
1. 将设备侧面的电源开关置于 "OFF" 位置
2. 按住设备侧面的 "G0" 按键
3. 通过 USB-C 数据线连接至电脑
4. 松开 "G0" 按键，设备进入下载模式

### 4. 配置 Wi-Fi

烧录完成后：
1. 设备启动后会创建 Wi-Fi 热点
2. 连接到热点，使用浏览器访问配置页面
3. 输入你的 Wi-Fi 凭据
4. 设备将连接到 Wi-Fi 并接入小智 AI 服务器

## 📝 下一步工作

### 音频配置

当前使用 `DummyAudioCodec`，需要根据实际硬件配置：

1. **如果 Cardputer 使用内置 DAC/ADC**:
   - 验证 I2S 引脚配置
   - 可能需要实现自定义音频编解码器

2. **如果有外部编解码器** (如 ES8311):
   - 在 `config.h` 中配置 I2C 引脚和编解码器地址
   - 在 `m5stack_cardputer.cc` 中将 `DummyAudioCodec` 替换为 `Es8311AudioCodec`

### 键盘支持（可选）

Cardputer 配备 56 键键盘，如果需要键盘输入功能：
1. 查找键盘扫描芯片的 I2C 地址和引脚
2. 实现键盘扫描和按键事件处理
3. 集成到小智 AI 的交互流程中

## 📚 参考资源

- [M5Stack Cardputer 官方文档](https://docs.m5stack.com/en/core/Cardputer)
- [小智 AI 自定义开发板指南](docs/custom-board.md)
- [xiaozhi-esp32 GitHub 仓库](https://github.com/78/xiaozhi-esp32)

## 🐛 故障排除

### 显示屏不显示
- 检查 SPI 引脚配置
- 验证 SPI 模式（当前为模式 0）
- 检查颜色反转和镜像设置

### 音频无输出/输入
- 验证 I2S 引脚配置
- 检查是否需要外部编解码器
- 确认采样率设置（当前为 24kHz）

### 无法连接 Wi-Fi
- 检查 BOOT 按钮引脚配置
- 确认按钮按下时能正确触发 Wi-Fi 配置重置

## 💡 提示

- 建议先验证显示屏配置，确保基本显示功能正常
- 然后逐步配置音频功能
- 最后考虑添加键盘支持（如果需要）

祝您使用愉快！如有问题，请参考项目 Issues 或提交新的 Issue。

