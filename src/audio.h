#pragma once

#include <string>

#include "src/assets.h"
#define MINIAUDIO_IMPLEMENTATION
#include "src/lib/miniaudio.h"
#include "src/logging.h"

static constexpr char AudioRoot[] = "assets/audio/";
static constexpr uint32_t Frequency = 44100;  // samples per second
static constexpr uint16_t Format = AUDIO_S16SYS;
static constexpr uint16_t Channels = 2;      // mono (1) or stereo (2)
static constexpr uint16_t ChunkSize = 4096;  // bytes per sample

struct UserData {
    ma_decoder decoder;
    bool playing;
};

class Audio : public Asset {
 public:
    void load(const std::string &filepath) override {
        ma_result result = ma_decoder_init_file((AudioRoot + filepath).c_str(), nullptr, &m_userData.decoder);
        if (result != MA_SUCCESS) {
            LOG_ERROR("Failed to load %s", filepath.c_str());
            exit(1);
        }
        ma_device_config config = ma_device_config_init(ma_device_type_playback);
        config.playback.format = m_userData.decoder.outputFormat;
        config.playback.channels = m_userData.decoder.outputChannels;
        config.sampleRate = m_userData.decoder.outputSampleRate;
        config.dataCallback = [](ma_device *device, void *output, const void *input, ma_uint32 frameCount) {
            UserData *data = reinterpret_cast<UserData *>(device->pUserData);
            if (data->playing) {
                ma_decoder *decoder = &data->decoder;
                if (decoder == nullptr) {
                    return;
                }
                ma_decoder_read_pcm_frames(decoder, output, frameCount);
            }
        };
        config.pUserData = &m_userData;
        if (ma_device_init(nullptr, &config, &m_device) != MA_SUCCESS) {
            LOG_ERROR("Failed to open playback device for %s", filepath.c_str());
            ma_decoder_uninit(&m_userData.decoder);
            exit(1);
        }
        m_userData.playing = false;
        if (ma_device_start(&m_device) != MA_SUCCESS) {
            LOG_ERROR("Failed to start playback device");
            ma_device_uninit(&m_device);
            ma_decoder_uninit(&m_userData.decoder);
            exit(1);
        }
        LOG_INFO("Loaded audio [%s]", filepath.c_str());
    }

    void unload() override {
        ma_device_uninit(&m_device);
        ma_decoder_uninit(&m_userData.decoder);
    }

    void play() {
        m_userData.playing = true;
    }

    void pause() {
        m_userData.playing = false;
    }

    void toggle() {
        if (!m_userData.playing) {
            play();
        } else {
            pause();
        }
    }

 private:
    ma_device m_device;
    UserData m_userData;
};

extern Assets<Audio> g_audio;

