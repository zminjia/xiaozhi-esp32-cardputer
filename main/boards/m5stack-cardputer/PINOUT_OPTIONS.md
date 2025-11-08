# M5Stack Cardputer 引脚配置选项

## 需要验证的引脚

由于无法找到 M5Stack Cardputer 的官方引脚定义，我们需要尝试不同的引脚组合。

## 常见的 ST7789 引脚配置模式

### 选项 1: 当前配置（推测）
```
SCK:  GPIO 18
MOSI: GPIO 19
DC:   GPIO 15
CS:   GPIO 5
RST:  NC
BL:   GPIO 38
```

### 选项 2: 常见 SPI 配置
```
SCK:  GPIO 18
MOSI: GPIO 23 (但 ESP32-S3 不支持)
DC:   GPIO 2
CS:   GPIO 5
RST:  GPIO 4
BL:   GPIO 1
```

### 选项 3: 参考其他 M5Stack 产品
```
SCK:  GPIO 18
MOSI: GPIO 19
DC:   GPIO 2
CS:   GPIO 5
RST:  GPIO 4
BL:   GPIO 1
```

## 排查步骤

1. **查看串口日志**，确认当前使用的引脚
2. **尝试不同的 SPI 模式**（0, 1, 2, 3）
3. **尝试不同的引脚组合**
4. **检查是否需要复位引脚**

## 建议

如果显示屏仍然无反应，建议：
1. 查看 M5Stack Cardputer 的官方 Arduino 示例代码
2. 检查 M5Cardputer 库的源码，查看实际引脚定义
3. 使用示波器或逻辑分析仪检查 SPI 信号

