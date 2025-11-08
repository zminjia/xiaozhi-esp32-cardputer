# 显示屏问题修复说明

## 已应用的修复

1. ✅ **添加显示屏开启命令**: `esp_lcd_panel_disp_on_off(panel_, true)`
2. ✅ **更改 SPI 模式**: 从模式 0 改为模式 3（ST7789 常用模式）
3. ✅ **添加调试日志**: 输出引脚配置信息

## 当前引脚配置（需要验证）

```
SCK:  GPIO 18
MOSI: GPIO 19
DC:   GPIO 15
CS:   GPIO 5
RST:  NC (未配置)
BL:   GPIO 38
```

## 可能的问题

### 1. 引脚配置不正确
M5Stack Cardputer 的实际引脚可能与当前配置不同。需要：
- 查看 M5Stack Cardputer 官方原理图
- 或使用 I2C 扫描工具查找实际连接

### 2. SPI 模式不正确
已尝试模式 3，如果仍不行，可以尝试：
- 模式 0
- 模式 1
- 模式 2

### 3. 需要复位引脚
某些 ST7789 显示屏需要硬件复位。如果当前配置不行，可能需要：
- 查找复位引脚
- 或使用软件复位序列

### 4. 背光未开启
背光引脚可能不正确，或需要不同的控制方式。

## 下一步排查步骤

1. **查看串口日志**，确认：
   - 显示屏初始化是否成功
   - 是否有错误信息
   - 引脚配置是否正确输出

2. **尝试不同的 SPI 模式**：
   - 如果模式 3 不行，尝试模式 0

3. **验证引脚配置**：
   - 参考 M5Stack Cardputer 官方文档
   - 或使用示波器/逻辑分析仪检查 SPI 信号

4. **检查背光**：
   - 确认背光引脚是否正确
   - 尝试手动控制背光

## 参考资源

- [M5Stack Cardputer 官方文档](https://docs.m5stack.com/en/core/Cardputer)
- [ST7789 数据手册](https://www.displayfuture.com/Display/datasheet/controller/ST7789.pdf)

