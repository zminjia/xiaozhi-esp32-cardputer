#include "cardputer_audio_codec.h"
#include <esp_log.h>
#include <cstring>
#include <vector>
#include <driver/i2s_std.h>
#include <driver/i2s_pdm.h>

#define TAG "CardputerAudioCodec"

CardputerAudioCodec::CardputerAudioCodec(int input_sample_rate, int output_sample_rate,
                                         gpio_num_t spk_bclk, gpio_num_t spk_lrclk, gpio_num_t spk_sdata,
                                         gpio_num_t mic_pdm_clk, gpio_num_t mic_pdm_din)
    : spk_bclk_(spk_bclk), spk_lrclk_(spk_lrclk), spk_sdata_(spk_sdata),
      mic_pdm_clk_(mic_pdm_clk), mic_pdm_din_(mic_pdm_din) {
    duplex_ = true;
    input_reference_ = false;
    input_channels_ = 1;
    input_sample_rate_ = input_sample_rate;
    output_sample_rate_ = output_sample_rate;
    input_gain_ = 1.0f;
    output_volume_ = 50;

    // Create I2S channel configuration for Microphone (I2S_NUM_0) - PDM mode
    // 注意：PDM 只支持在 I2S0 上，所以麦克风必须使用 I2S_NUM_0
    i2s_chan_config_t mic_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    mic_chan_cfg.dma_desc_num = 8;
    mic_chan_cfg.dma_frame_num = 240;
    mic_chan_cfg.auto_clear = true;
    ESP_ERROR_CHECK(i2s_new_channel(&mic_chan_cfg, nullptr, &rx_handle_));

    // Configure PDM mode for input (Unit Mini PDM microphone)
    i2s_pdm_rx_config_t mic_pdm_cfg = {
        .clk_cfg = I2S_PDM_RX_CLK_DEFAULT_CONFIG((uint32_t)input_sample_rate_),
        // PDM mode data bit-width is fixed to 16
        .slot_cfg = I2S_PDM_RX_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .clk = mic_pdm_clk_,  // PDM Clock (G1, HY2.0-4P White)
            .din = mic_pdm_din_,  // PDM Data (G2, HY2.0-4P Yellow)
            .invert_flags = {
                .clk_inv = false,
            },
        },
    };
    ESP_ERROR_CHECK(i2s_channel_init_pdm_rx_mode(rx_handle_, &mic_pdm_cfg));

    // Create I2S channel configuration for Speaker (I2S_NUM_1)
    // 注意：由于 PDM 必须使用 I2S0，Speaker 使用 I2S1
    i2s_chan_config_t spk_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_1, I2S_ROLE_MASTER);
    spk_chan_cfg.dma_desc_num = 8;
    spk_chan_cfg.dma_frame_num = 240;
    spk_chan_cfg.auto_clear = true;
    ESP_ERROR_CHECK(i2s_new_channel(&spk_chan_cfg, &tx_handle_, nullptr));

    // Configure I2S standard mode for output (speaker)
    i2s_std_config_t spk_cfg = {
        .clk_cfg = {
            .sample_rate_hz = (uint32_t)output_sample_rate_,
            .clk_src = I2S_CLK_SRC_DEFAULT,
            .mclk_multiple = I2S_MCLK_MULTIPLE_256,
        },
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = spk_bclk_,
            .ws = spk_lrclk_,
            .dout = spk_sdata_,
            .din = I2S_GPIO_UNUSED,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            }
        }
    };
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(tx_handle_, &spk_cfg));

    ESP_LOGI(TAG, "Cardputer audio codec initialized:");
    ESP_LOGI(TAG, "  Microphone (I2S_NUM_0, PDM): CLK=%d, DIN=%d, sample_rate=%dHz",
             mic_pdm_clk_, mic_pdm_din_, input_sample_rate_);
    ESP_LOGI(TAG, "  Speaker (I2S_NUM_1, I2S): BCLK=%d, LRCLK=%d, SDATA=%d, sample_rate=%dHz",
             spk_bclk_, spk_lrclk_, spk_sdata_, output_sample_rate_);
    ESP_LOGI(TAG, "  Note: Built-in SPM1423 MIC (G46, G43) is disabled, G43 is used for Speaker LRCLK");
    ESP_LOGI(TAG, "  External Unit Mini PDM connected via HY2.0-4P: G1(Clock), G2(Data)");
    ESP_LOGI(TAG, "  Important: PDM only supports I2S0, so MIC uses I2S0 and Speaker uses I2S1");
}

CardputerAudioCodec::~CardputerAudioCodec() {
    if (rx_handle_ != nullptr) {
        i2s_channel_disable(rx_handle_);
        i2s_del_channel(rx_handle_);
    }
    if (tx_handle_ != nullptr) {
        i2s_channel_disable(tx_handle_);
        i2s_del_channel(tx_handle_);
    }
}

void CardputerAudioCodec::SetOutputVolume(int volume) {
    output_volume_ = volume;
    AudioCodec::SetOutputVolume(volume);
    ESP_LOGI(TAG, "Set output volume to %d", volume);
}

void CardputerAudioCodec::EnableInput(bool enable) {
    if (enable == input_enabled_) {
        return;
    }
    input_enabled_ = enable;
    if (rx_handle_ != nullptr) {
        if (enable) {
            ESP_ERROR_CHECK(i2s_channel_enable(rx_handle_));
        } else {
            ESP_ERROR_CHECK(i2s_channel_disable(rx_handle_));
        }
    }
    ESP_LOGI(TAG, "Set input enable to %s", enable ? "true" : "false");
}

void CardputerAudioCodec::EnableOutput(bool enable) {
    if (enable == output_enabled_) {
        return;
    }
    output_enabled_ = enable;
    if (tx_handle_ != nullptr) {
        if (enable) {
            ESP_ERROR_CHECK(i2s_channel_enable(tx_handle_));
        } else {
            ESP_ERROR_CHECK(i2s_channel_disable(tx_handle_));
        }
    }
    ESP_LOGI(TAG, "Set output enable to %s", enable ? "true" : "false");
}

int CardputerAudioCodec::Read(int16_t* dest, int samples) {
    if (!input_enabled_ || rx_handle_ == nullptr) {
        return 0;
    }
    size_t bytes_read = 0;
    esp_err_t ret = i2s_channel_read(rx_handle_, dest, samples * sizeof(int16_t), &bytes_read, portMAX_DELAY);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2S read failed: %s", esp_err_to_name(ret));
        return 0;
    }
    return bytes_read / sizeof(int16_t);
}

int CardputerAudioCodec::Write(const int16_t* data, int samples) {
    if (!output_enabled_ || tx_handle_ == nullptr) {
        return 0;
    }
    
    // Apply volume control
    std::vector<int16_t> volume_adjusted(samples);
    float volume_factor = (float)output_volume_ / 100.0f;
    for (int i = 0; i < samples; i++) {
        volume_adjusted[i] = (int16_t)(data[i] * volume_factor);
    }
    
    size_t bytes_written = 0;
    esp_err_t ret = i2s_channel_write(tx_handle_, volume_adjusted.data(), samples * sizeof(int16_t), &bytes_written, portMAX_DELAY);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2S write failed: %s", esp_err_to_name(ret));
        return 0;
    }
    return bytes_written / sizeof(int16_t);
}

