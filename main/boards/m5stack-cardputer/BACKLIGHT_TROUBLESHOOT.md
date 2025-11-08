# 背光问题排查指南

## 当前配置

根据 [M5Stack Cardputer 官方文档](https://docs.m5stack.com/zh_CN/core/Cardputer)：

- **背光引脚**: GPIO 38 (DISP_BL)
- **特殊说明**: G38 同时连接 DISP_BL 和 RGB LED 的 VDD，是**电源使能引脚**
- **控制方式**: GPIO 直接控制（非 PWM）

## 已尝试的修复

1. ✅ 创建了 CardputerBacklight 类（GPIO 直接控制）
2. ✅ 在构造函数中直接配置 GPIO 并设置电平
3. ✅ 在显示屏初始化之前开启背光
4. ✅ 添加了详细的调试日志

## 如果背光仍然不亮

### 方法 1: 尝试反转逻辑

修改 `config.h` 中的：
```c
#define DISPLAY_BACKLIGHT_OUTPUT_INVERT true  // 改为 true
```

然后重新编译和烧录。

### 方法 2: 检查串口日志

查看串口输出，确认：
- `Backlight GPIO 38 set to X` - 确认设置的电平
- `Backlight GPIO 38 actual level: X` - 确认实际读取的电平
- 如果设置和读取不一致，可能是 GPIO 配置问题

### 方法 3: 检查硬件

1. **电源开关**: 确保设备侧面的电源开关处于 `ON` 状态
2. **电池电量**: 确保电池有电或通过 USB 供电
3. **硬件连接**: 检查显示屏连接是否正常

### 方法 4: 手动测试 GPIO

如果以上都不行，可以在代码中临时添加测试代码：

```cpp
// 在构造函数开始处添加
gpio_config_t test_conf = {};
test_conf.intr_type = GPIO_INTR_DISABLE;
test_conf.mode = GPIO_MODE_OUTPUT;
test_conf.pin_bit_mask = (1ULL << GPIO_NUM_38);
test_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
test_conf.pull_up_en = GPIO_PULLUP_DISABLE;
gpio_config(&test_conf);

// 尝试高电平
gpio_set_level(GPIO_NUM_38, 1);
vTaskDelay(pdMS_TO_TICKS(500));
ESP_LOGI(TAG, "GPIO 38 set to HIGH, check if backlight is on");

// 尝试低电平
gpio_set_level(GPIO_NUM_38, 0);
vTaskDelay(pdMS_TO_TICKS(500));
ESP_LOGI(TAG, "GPIO 38 set to LOW, check if backlight is on");

// 再试高电平
gpio_set_level(GPIO_NUM_38, 1);
ESP_LOGI(TAG, "GPIO 38 set to HIGH again");
```

## 参考资源

- [M5Stack Cardputer 官方文档](https://docs.m5stack.com/zh_CN/core/Cardputer)
- [M5Stack Cardputer V1.1 文档](https://docs.m5stack.com/zh_CN/core/Cardputer%20V1.1)

