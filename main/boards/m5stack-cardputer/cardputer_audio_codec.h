#ifndef CARDPUTER_AUDIO_CODEC_H
#define CARDPUTER_AUDIO_CODEC_H

#include "audio_codec.h"
#include <driver/i2s_std.h>
#include <driver/gpio.h>

class CardputerAudioCodec : public AudioCodec {
public:
    // 分离的 I2S 通道配置
    // Microphone: mic_pdm_clk, mic_pdm_din (I2S_NUM_0) - PDM 模式 (Unit Mini PDM)
    // 注意：PDM 只支持在 I2S0 上，所以麦克风必须使用 I2S_NUM_0
    // Speaker: spk_bclk, spk_lrclk, spk_sdata (I2S_NUM_1) - I2S 标准模式
    CardputerAudioCodec(int input_sample_rate, int output_sample_rate,
                       gpio_num_t spk_bclk, gpio_num_t spk_lrclk, gpio_num_t spk_sdata,
                       gpio_num_t mic_pdm_clk, gpio_num_t mic_pdm_din);
    ~CardputerAudioCodec();

    void SetOutputVolume(int volume) override;
    void EnableInput(bool enable) override;
    void EnableOutput(bool enable) override;

    int Read(int16_t* dest, int samples) override;
    int Write(const int16_t* data, int samples) override;

private:
    i2s_chan_handle_t rx_handle_ = nullptr;   // Microphone (I2S_NUM_0) - PDM mode
    i2s_chan_handle_t tx_handle_ = nullptr;  // Speaker (I2S_NUM_1) - I2S mode
    gpio_num_t spk_bclk_;
    gpio_num_t spk_lrclk_;
    gpio_num_t spk_sdata_;
    gpio_num_t mic_pdm_clk_;  // PDM Clock
    gpio_num_t mic_pdm_din_;  // PDM Data
};

#endif // CARDPUTER_AUDIO_CODEC_H

