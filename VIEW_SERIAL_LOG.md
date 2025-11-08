# 如何查看串口日志

## 方法 1: 使用 ESP-IDF monitor（推荐）

在项目目录下运行：

```bash
cd /Volumes/Michael/Coding/cardputer/esp-idf
. ./export.sh
cd /Volumes/Michael/Coding/cardputer/xiaozhiai
idf.py -p /dev/tty.usbmodem201401 monitor
```

**退出方法**: 按 `Ctrl+]` 退出监视器

## 方法 2: 使用 screen 命令

```bash
screen /dev/tty.usbmodem201401 115200
```

**退出方法**: 按 `Ctrl+A` 然后按 `K`，再按 `Y` 确认

## 方法 3: 使用 minicom

```bash
minicom -D /dev/tty.usbmodem201401 -b 115200
```

**退出方法**: 按 `Ctrl+A` 然后按 `X`

## 方法 4: 使用 Arduino IDE 串口监视器

如果你安装了 Arduino IDE：
1. 打开 Arduino IDE
2. 工具 -> 端口 -> 选择 `/dev/tty.usbmodem201401`
3. 工具 -> 串口监视器
4. 设置波特率为 115200

## 查看特定日志

如果你想只查看背光相关的日志，可以使用 grep 过滤：

```bash
idf.py -p /dev/tty.usbmodem201401 monitor | grep -i "backlight\|pwm\|GPIO.*38\|Cardputer"
```

## 保存日志到文件

```bash
idf.py -p /dev/tty.usbmodem201401 monitor > serial_log.txt 2>&1
```

或者使用 tee 同时显示和保存：

```bash
idf.py -p /dev/tty.usbmodem201401 monitor | tee serial_log.txt
```

## 常见问题

### 1. 找不到串口设备

检查设备是否连接：
```bash
ls -la /dev/tty.* | grep -i usb
```

### 2. 权限问题

如果提示权限不足，可能需要：
```bash
sudo chmod 666 /dev/tty.usbmodem201401
```

或者将用户添加到 dialout 组（Linux）：
```bash
sudo usermod -a -G dialout $USER
```

### 3. 设备被占用

如果提示设备被占用，关闭其他可能使用该串口的程序（如 Arduino IDE、其他终端等）

## 当前设备

根据之前的配置，你的设备应该是：
- **串口**: `/dev/tty.usbmodem201401`
- **波特率**: 115200（ESP-IDF 默认）

## 查看背光相关日志

运行监视器后，你应该能看到类似这样的日志：

```
I (xxx) M5StackCardputer: Initializing M5Stack Cardputer board
I (xxx) M5StackCardputer: Initializing backlight first (power enable)
I (xxx) M5StackCardputer: CardputerBacklight PWM initialized: pin=38, channel=7, freq=256Hz, invert=0, initial_duty=255
I (xxx) M5StackCardputer: Backlight set: brightness=100, duty=255 (GPIO38)
I (xxx) M5StackCardputer: Initializing SPI for display
I (xxx) M5StackCardputer: Initializing ST7789 display
...
```

如果看到这些日志，说明背光 PWM 已经初始化。如果背光还是不亮，可能需要检查硬件连接或尝试其他配置。

