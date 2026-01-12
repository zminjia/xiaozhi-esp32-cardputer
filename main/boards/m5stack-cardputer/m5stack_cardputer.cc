#include "wifi_board.h"
#include "codecs/dummy_audio_codec.h"
#include "cardputer_audio_codec.h"
#include "display/lcd_display.h"
#include "application.h"
#include "button.h"
#include "config.h"
#include "backlight.h"
#include "wifi_manager.h"

#include <esp_log.h>
#include <driver/spi_common.h>
#include <driver/gpio.h>
#include <driver/ledc.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_vendor.h>

#define TAG "M5StackCardputer"

// Cardputer 背光控制类 - 使用 PWM 控制
// 根据 M5Cardputer-UserDemo 官方示例，Cardputer 使用 PWM 控制背光
// 配置：GPIO 38, LEDC 通道 7, 频率 256Hz, 不反转, 偏移 16
class CardputerBacklight : public Backlight {
public:
    CardputerBacklight(gpio_num_t pin, bool output_invert) 
        : Backlight(), pin_(pin), output_invert_(output_invert) {
        // 配置 LEDC 定时器
        ledc_timer_config_t ledc_timer = {};
        ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;
        ledc_timer.timer_num = LEDC_TIMER_0;
        ledc_timer.duty_resolution = LEDC_TIMER_8_BIT;  // 8位分辨率，0-255
        ledc_timer.freq_hz = 256;  // 256Hz，与官方示例一致
        ledc_timer.clk_cfg = LEDC_AUTO_CLK;
        ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

        // 配置 LEDC 通道
        ledc_channel_config_t ledc_channel = {};
        ledc_channel.speed_mode = LEDC_LOW_SPEED_MODE;
        ledc_channel.channel = (ledc_channel_t)7;  // 通道 7，与官方示例一致
        ledc_channel.timer_sel = LEDC_TIMER_0;
        ledc_channel.intr_type = LEDC_INTR_DISABLE;
        ledc_channel.gpio_num = pin;
        // 初始 duty 设置为 255（最大亮度），确保背光立即开启
        ledc_channel.duty = 255;
        ledc_channel.hpoint = 0;
        ledc_channel.flags.output_invert = output_invert;
        ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
        
        // 立即更新 duty，确保背光开启
        ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)7);
        
        ESP_LOGI(TAG, "CardputerBacklight PWM initialized: pin=%d, channel=7, freq=256Hz, invert=%d, initial_duty=255", 
                 pin, output_invert);
    }

protected:
    void SetBrightnessImpl(uint8_t brightness) override {
        // 将 0-100 的亮度值映射到 PWM duty
        // 根据官方示例，有 16 的偏移量，但为了确保背光能亮，我们先尝试不使用偏移
        uint32_t duty = 0;
        if (brightness > 0) {
            // 先尝试直接映射到 0-255（不使用偏移），如果不行再试偏移版本
            duty = (brightness * 255) / 100;
            // 如果 duty 太小，至少设置为 16（偏移量）
            if (duty < 16) {
                duty = 16;
            }
        }
        
        ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)7, duty);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)7);
        ESP_LOGI(TAG, "Backlight set: brightness=%d, duty=%lu (GPIO%d)", brightness, duty, pin_);
    }

private:
    gpio_num_t pin_;
    bool output_invert_;
};

class M5StackCardputer : public WifiBoard {
private:
    Button boot_button_;
    LcdDisplay* display_;
    esp_lcd_panel_io_handle_t panel_io_ = nullptr;
    esp_lcd_panel_handle_t panel_ = nullptr;

    void InitializeSpi() {
        ESP_LOGI(TAG, "Initializing SPI for display");
        spi_bus_config_t buscfg = {};
        buscfg.mosi_io_num = DISPLAY_SPI_MOSI_PIN;
        buscfg.miso_io_num = GPIO_NUM_NC;
        buscfg.sclk_io_num = DISPLAY_SPI_SCK_PIN;
        buscfg.quadwp_io_num = GPIO_NUM_NC;
        buscfg.quadhd_io_num = GPIO_NUM_NC;
        buscfg.max_transfer_sz = DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(uint16_t);
        // 根据官方示例，Cardputer 使用 SPI3_HOST
        ESP_ERROR_CHECK(spi_bus_initialize(SPI3_HOST, &buscfg, SPI_DMA_CH_AUTO));
    }

    void InitializeButtons() {
        ESP_LOGI(TAG, "Initializing buttons");
        boot_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting) {
                auto* wifi_board = dynamic_cast<WifiBoard*>(&Board::GetInstance());
                if (wifi_board && !WifiManager::GetInstance().IsConnected()) {
                    wifi_board->EnterWifiConfigMode();
                }
            }
            app.ToggleChatState();
        });
    }

    void InitializeSt7789Display() {
        ESP_LOGI(TAG, "Initializing ST7789 display");
        ESP_LOGI(TAG, "Display pins - SCK:%d MOSI:%d DC:%d CS:%d RST:%d BL:%d", 
                 DISPLAY_SPI_SCK_PIN, DISPLAY_SPI_MOSI_PIN, DISPLAY_DC_PIN, 
                 DISPLAY_SPI_CS_PIN, DISPLAY_RESET_PIN, DISPLAY_BACKLIGHT_PIN);
        
        // 配置 LCD 面板 IO
        esp_lcd_panel_io_spi_config_t io_config = {};
        io_config.cs_gpio_num = DISPLAY_SPI_CS_PIN;
        io_config.dc_gpio_num = DISPLAY_DC_PIN;
        io_config.spi_mode = 0;  // SPI 模式 0，尝试模式 0（如果不行再试模式 3）
        io_config.pclk_hz = 40 * 1000 * 1000;  // 40MHz，与官方示例一致
        io_config.trans_queue_depth = 10;
        io_config.lcd_cmd_bits = 8;
        io_config.lcd_param_bits = 8;
        // 根据官方示例，Cardputer 使用 SPI3_HOST
        ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(SPI3_HOST, &io_config, &panel_io_));

        // 配置 LCD 面板
        esp_lcd_panel_dev_config_t panel_config = {};
        panel_config.reset_gpio_num = DISPLAY_RESET_PIN;
        panel_config.rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB;
        panel_config.bits_per_pixel = 16;
        ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(panel_io_, &panel_config, &panel_));
        
        // 初始化面板 - Cardputer 有复位引脚 GPIO33
        ESP_LOGI(TAG, "Resetting display panel");
        esp_lcd_panel_reset(panel_);
        vTaskDelay(pdMS_TO_TICKS(120));  // ST7789 复位后需要等待至少 120ms
        
        ESP_LOGI(TAG, "Initializing display panel");
        esp_lcd_panel_init(panel_);
        vTaskDelay(pdMS_TO_TICKS(20));  // 等待初始化完成
        
        ESP_LOGI(TAG, "Configuring display settings");
        esp_lcd_panel_invert_color(panel_, true);  // ST7789 通常需要颜色反转
        esp_lcd_panel_swap_xy(panel_, DISPLAY_SWAP_XY);
        esp_lcd_panel_mirror(panel_, DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y);
        
        ESP_LOGI(TAG, "Turning on display");
        esp_lcd_panel_disp_on_off(panel_, true);  // 开启显示屏
        vTaskDelay(pdMS_TO_TICKS(50));  // 等待显示屏完全开启
        
        // 创建显示对象
        display_ = new SpiLcdDisplay(panel_io_, panel_,
                                    DISPLAY_WIDTH, DISPLAY_HEIGHT, 
                                    DISPLAY_OFFSET_X, DISPLAY_OFFSET_Y, 
                                    DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y, DISPLAY_SWAP_XY);
    }

public:
    M5StackCardputer() : boot_button_(BOOT_BUTTON_GPIO) {
        ESP_LOGI(TAG, "Initializing M5Stack Cardputer board");
        
        // 根据 M5Cardputer-UserDemo 官方示例，Cardputer 使用 PWM 控制背光
        // 重要：必须先初始化背光（PWM），然后再初始化显示屏
        // 因为 Cardputer 的 RGB LED 和 LCD 背光共用电源（G38）
        if (DISPLAY_BACKLIGHT_PIN != GPIO_NUM_NC) {
            ESP_LOGI(TAG, "Initializing backlight first (power enable)");
            // 初始化背光 PWM（在 GetBacklight() 中完成）
            GetBacklight()->SetBrightness(100);  // 立即设置为最大亮度
            vTaskDelay(pdMS_TO_TICKS(100));  // 等待电源稳定
        }
        
        InitializeSpi();
        InitializeSt7789Display();
        InitializeButtons();
        
        ESP_LOGI(TAG, "M5Stack Cardputer board initialized");
    }

    virtual AudioCodec* GetAudioCodec() override {
        // Cardputer 使用分离的 I2S 通道：
        // - 外接 Unit Mini PDM MIC (I2S_NUM_0, PDM): G1 (Clock), G2 (Data) - 通过 HY2.0-4P
        //   注意：PDM 只支持在 I2S0 上，所以麦克风必须使用 I2S_NUM_0
        // - Speaker (I2S_NUM_1, I2S): G41 (BCLK), G42 (SDATA), G43 (LRCLK)
        // 注意：内置 SPM1423 MIC (G46, G43) 已禁用，G43 用于 Speaker 的 LRCLK
        static CardputerAudioCodec audio_codec(
            AUDIO_INPUT_SAMPLE_RATE, 
            AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_SPEAKER_BCLK,   // Speaker BCLK (G41)
            AUDIO_SPEAKER_LRCLK,  // Speaker LRCLK (G43)
            AUDIO_SPEAKER_SDATA,  // Speaker SDATA (G42)
            AUDIO_MIC_PDM_CLK,    // External PDM MIC Clock (G1, HY2.0-4P White)
            AUDIO_MIC_PDM_DIN     // External PDM MIC Data (G2, HY2.0-4P Yellow)
        );
        return &audio_codec;
    }

    virtual Display* GetDisplay() override {
        return display_;
    }
    
    virtual Backlight* GetBacklight() override {
        if (DISPLAY_BACKLIGHT_PIN != GPIO_NUM_NC) {
            // Cardputer 使用简单的 GPIO 控制背光，而不是 PWM
            static CardputerBacklight backlight(DISPLAY_BACKLIGHT_PIN, DISPLAY_BACKLIGHT_OUTPUT_INVERT);
            return &backlight;
        }
        return nullptr;
    }
};

DECLARE_BOARD(M5StackCardputer);

