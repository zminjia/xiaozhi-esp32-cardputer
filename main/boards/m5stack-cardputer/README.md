# M5Stack Cardputer 开发板配置

本目录包含 M5Stack Cardputer 开发板的小智 AI 配置。

## 硬件规格

- **芯片**: ESP32-S3
- **显示屏**: 1.14 英寸 TFT (ST7789), 240x135 分辨率
- **键盘**: 56 键 QWERTY 键盘
- **音频**: MEMS 麦克风和内置扬声器
- **Flash**: 8MB

## 重要提示

⚠️ **引脚配置需要验证**: 本配置中的引脚定义是基于常见 M5Stack 产品的推测，**需要根据实际硬件进行验证和调整**。

## 配置说明

### 当前配置状态

1. **显示屏**: 已配置 ST7789 驱动，引脚需要验证
2. **音频**: 当前使用 `DummyAudioCodec`，需要根据实际硬件配置 I2S 引脚和编解码器
3. **按钮**: 使用 BOOT 按钮作为主要交互按钮

### 需要验证的引脚

请参考 M5Stack Cardputer 的官方文档验证以下引脚：

- **显示屏 SPI 引脚**: `DISPLAY_SPI_SCK_PIN`, `DISPLAY_SPI_MOSI_PIN`, `DISPLAY_DC_PIN`, `DISPLAY_SPI_CS_PIN`
- **背光控制**: `DISPLAY_BACKLIGHT_PIN`
- **I2S 音频引脚**: `AUDIO_I2S_GPIO_*` (MCLK, WS, BCLK, DIN, DOUT)
- **按钮**: `BOOT_BUTTON_GPIO`

## 编译和烧录

### 方法一：使用 release.py 脚本（推荐）

```bash
python scripts/release.py m5stack-cardputer
```

### 方法二：手动配置

1. **设置目标芯片**:
   ```bash
   idf.py set-target esp32s3
   ```

2. **进入配置菜单**:
   ```bash
   idf.py menuconfig
   ```
   
   选择: `Xiaozhi Assistant` -> `Board Type` -> `M5Stack Cardputer`

3. **编译和烧录**:
   ```bash
   idf.py build
   idf.py flash monitor
   ```

## 进入下载模式

根据 M5Stack Cardputer 的文档：
1. 将设备侧面的电源开关置于 "OFF" 位置
2. 按住设备侧面的 "G0" 按键
3. 通过 USB-C 数据线连接至电脑
4. 松开 "G0" 按键，设备进入下载模式

## 音频配置

Cardputer 使用 MEMS 麦克风和内置扬声器。如果当前配置无法正常工作，可能需要：

1. **验证 I2S 引脚配置**: 检查 `config.h` 中的 `AUDIO_I2S_GPIO_*` 定义
2. **配置音频编解码器**: 如果有外部编解码器（如 ES8311），需要：
   - 在 `config.h` 中配置 I2C 引脚和编解码器地址
   - 在 `m5stack_cardputer.cc` 中将 `DummyAudioCodec` 替换为相应的编解码器类（如 `Es8311AudioCodec`）

## 键盘支持

Cardputer 配备 56 键键盘，但当前配置未实现键盘输入功能。如果需要键盘支持，需要：

1. 查找键盘扫描芯片的 I2C 地址和引脚
2. 实现键盘扫描和按键事件处理
3. 将按键事件集成到小智 AI 的交互流程中

## 参考资料

- [M5Stack Cardputer 官方文档](https://docs.m5stack.com/en/core/Cardputer)
- [小智 AI 自定义开发板指南](../../../docs/custom-board.md)
- [xiaozhi-esp32 GitHub 仓库](https://github.com/78/xiaozhi-esp32)

## 故障排除

### 显示屏不显示

1. 检查 SPI 引脚配置是否正确
2. 验证显示屏的 SPI 模式（当前设置为模式 0）
3. 检查颜色反转和镜像设置
4. 确认背光引脚配置

### 音频无输出/输入

1. 验证 I2S 引脚配置
2. 检查是否需要外部编解码器
3. 确认采样率设置（当前为 24kHz）

### 无法连接 Wi-Fi

1. 检查 BOOT 按钮引脚配置
2. 确认按钮按下时能正确触发 Wi-Fi 配置重置

## 贡献

如果您验证了正确的引脚配置或实现了新功能，欢迎提交 Pull Request！

