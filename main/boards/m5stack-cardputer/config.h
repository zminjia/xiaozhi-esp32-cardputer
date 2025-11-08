#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

// M5Stack Cardputer Board configuration
// 注意：以下引脚配置需要根据实际硬件验证和调整

#include <driver/gpio.h>

// 音频配置
#define AUDIO_INPUT_SAMPLE_RATE  24000
#define AUDIO_OUTPUT_SAMPLE_RATE 24000

// I2S 音频引脚配置（根据官方文档）
// Speaker (NS4168): G41 (BCLK), G42 (SDATA), G43 (LRCLK), G44
// 外接 MIC 通过 HY2.0-4P: G1, G2 (GND, 5V, G2, G1)
// 注意：内置 SPM1423 MIC (G46 DAT, G43 CLK) 已禁用，G43 用于 Speaker 的 LRCLK
#define AUDIO_SPEAKER_BCLK  GPIO_NUM_41  // Speaker BCLK
#define AUDIO_SPEAKER_SDATA GPIO_NUM_42  // Speaker SDATA
#define AUDIO_SPEAKER_LRCLK GPIO_NUM_43  // Speaker LRCLK (与内置 MIC 的 CLK 冲突，已禁用内置 MIC)

// 外接 PDM MIC 引脚（Unit Mini PDM，通过 HY2.0-4P）
// HY2.0-4P: Black (GND), Red (5V), Yellow (Data/G2), White (Clock/G1)
// Unit Mini PDM 使用 PDM 协议，只需要两个引脚：Clock 和 Data
#define AUDIO_MIC_PDM_CLK   GPIO_NUM_1   // PDM MIC Clock (HY2.0-4P White)
#define AUDIO_MIC_PDM_DIN   GPIO_NUM_2   // PDM MIC Data (HY2.0-4P Yellow)

// 音频编解码器配置
// Cardputer 可能使用内置 DAC/ADC，如果没有外部编解码器，可能需要使用 dummy_audio_codec
#define AUDIO_CODEC_PA_PIN       GPIO_NUM_NC
#define AUDIO_CODEC_I2C_SDA_PIN  GPIO_NUM_NC  // 如果没有 I2C 编解码器
#define AUDIO_CODEC_I2C_SCL_PIN  GPIO_NUM_NC
#define AUDIO_CODEC_ES8311_ADDR  ES8311_CODEC_DEFAULT_ADDR

// 按钮配置
// Cardputer 有键盘，但我们可以使用 BOOT 按钮作为主要交互按钮
#define BUILTIN_LED_GPIO        GPIO_NUM_NC
#define BOOT_BUTTON_GPIO        GPIO_NUM_0    // BOOT 按钮，需要验证
#define VOLUME_UP_BUTTON_GPIO   GPIO_NUM_NC
#define VOLUME_DOWN_BUTTON_GPIO GPIO_NUM_NC

// 显示屏配置 - ST7789V2 1.14英寸 TFT
// Cardputer 使用 1.14 英寸 ST7789V2 屏幕，分辨率 240x135
// 引脚定义来自 M5Stack Cardputer 官方文档
#define DISPLAY_SPI_SCK_PIN     GPIO_NUM_36    // SCK - SPI 时钟
#define DISPLAY_SPI_MOSI_PIN    GPIO_NUM_35    // DAT - SPI 数据
#define DISPLAY_DC_PIN          GPIO_NUM_34     // RS - 数据/命令选择
#define DISPLAY_SPI_CS_PIN      GPIO_NUM_37    // CS - 片选
#define DISPLAY_RESET_PIN       GPIO_NUM_33    // RST - 复位引脚

#define DISPLAY_WIDTH   240
#define DISPLAY_HEIGHT  135
#define DISPLAY_MIRROR_X true   // 水平镜像，用于水平显示
#define DISPLAY_MIRROR_Y false
#define DISPLAY_SWAP_XY  true    // 交换 XY 轴，用于水平显示

#define DISPLAY_OFFSET_X  40
#define DISPLAY_OFFSET_Y  55

// 背光控制
// 注意：G38 同时连接 DISP_BL 和 RGB LED 的 VDD，是电源使能引脚
#define DISPLAY_BACKLIGHT_PIN GPIO_NUM_38     // DISP_BL - 背光控制（电源使能）
#define DISPLAY_BACKLIGHT_OUTPUT_INVERT false // 如果背光不亮，尝试改为 true

#endif // _BOARD_CONFIG_H_

