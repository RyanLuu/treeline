#include "assets/audio.h"

#include "core/logging.h"
#define MINIAUDIO_IMPLEMENTATION
#include "lib/miniaudio/miniaudio.h"
#include "lib/miniaudio/research/miniaudio_engine.h"

namespace assets {

static constexpr char AudioRoot[] = "assets/audio/";

ma_resource_manager Audio::s_rm;
ma_context Audio::s_context;
ma_device Audio::s_device;
ma_engine Audio::s_engine;

bool Audio::enable() {
    ma_resource_manager_config rmConfig = ma_resource_manager_config_init();
    rmConfig.decodedFormat = ma_format_f32;
    rmConfig.decodedChannels = 0;
    rmConfig.decodedSampleRate = 44100;

    ma_result result = ma_resource_manager_init(&rmConfig, &Audio::s_rm);
    if (result != MA_SUCCESS) {
        LOG_ERROR("Failed to initialize resource manager: %s", ma_result_description(result));
        return false;
    }

    result = ma_context_init(NULL, 0, NULL, &Audio::s_context);
    if (result != MA_SUCCESS) {
        LOG_ERROR("Failed to initialize context: %s", ma_result_description(result));
        return false;
    }

    ma_device_info *deviceInfos;
    ma_uint32 deviceCount;
    result = ma_context_get_devices(&Audio::s_context, &deviceInfos, &deviceCount, NULL, NULL);
    if (result != MA_SUCCESS) {
        LOG_ERROR("Failed to enumerate playback devices");
        return false;
    }

    LOG_INFO("Found %lu playback devices:", deviceCount);
    for (ma_uint32 i = 0; i < deviceCount; i++) {
        LOG_INFO("%lu. %s", i + 1, deviceInfos[i].name);
    }

    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.pDeviceID = &deviceInfos[0].id;
    deviceConfig.playback.format = Audio::s_rm.config.decodedFormat;
    deviceConfig.playback.channels = 0;
    deviceConfig.sampleRate = Audio::s_rm.config.decodedSampleRate;
    deviceConfig.dataCallback = [](ma_device *device, void *output, const void *input, ma_uint32 frameCount) {
        ma_engine_read_pcm_frames(reinterpret_cast<ma_engine *>(device->pUserData), output, frameCount, NULL);
    };
    deviceConfig.pUserData = &Audio::s_engine;
    result = ma_device_init(&Audio::s_context, &deviceConfig, &Audio::s_device);
    if (result != MA_SUCCESS) {
        LOG_ERROR("Failed to initialize device for %s: %s", deviceInfos[0].name, ma_result_description(result));
        return false;
    }

    ma_engine_config engineConfig = ma_engine_config_init();
    engineConfig.pDevice = &Audio::s_device;
    engineConfig.pResourceManager = &Audio::s_rm;
    engineConfig.noAutoStart = MA_TRUE;
    ma_engine_init(&engineConfig, &Audio::s_engine);
    if (result != MA_SUCCESS) {
        LOG_ERROR("Failed to initialize engine for %s: %s", deviceInfos[0].name, ma_result_description(result));
        ma_device_uninit(&Audio::s_device);
        return false;
    }

    result = ma_engine_start(&Audio::s_engine);
    if (result != MA_SUCCESS) {
        LOG_ERROR("Failed to start engine for %s: %s", deviceInfos[0].name, ma_result_description(result));
        ma_device_uninit(&Audio::s_device);
        return false;
    }
    return true;
}

void Audio::disable() {
    ma_engine_uninit(&Audio::s_engine);
    ma_device_uninit(&Audio::s_device);
    ma_context_uninit(&Audio::s_context);
    ma_resource_manager_uninit(&Audio::s_rm);
}

void Audio::load(const std::string &filepath) {
    constexpr ma_uint32 flags = MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_DECODE | MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_ASYNC | MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_STREAM;
    ma_result result = ma_sound_init_from_file(&Audio::s_engine, (AudioRoot + filepath).c_str(), flags, NULL, NULL, &m_sound);
    if (result != MA_SUCCESS) {
        LOG_ERROR("Failed to load %s", filepath.c_str());
        exit(1);
    }
    LOG_INFO("Loaded audio [%s]", filepath.c_str());
}

void Audio::unload() {
    ma_sound_uninit(&m_sound);
}

void Audio::play(uint64_t msDelay) {
    if (msDelay > 0) {
        auto startTime = ma_engine_get_time(&Audio::s_engine) + (ma_engine_get_sample_rate(&Audio::s_engine) * msDelay / 1000);
        ma_sound_set_start_time_in_pcm_frames(&m_sound, startTime);
        ma_sound_set_stop_time_in_pcm_frames(&m_sound, (ma_uint64)(ma_int64)-1);
        ma_sound_start(&m_sound);
    } else {
        ma_sound_start(&m_sound);
    }
}

void Audio::pause(uint64_t msDelay) {
    if (msDelay > 0) {
        auto stopTime = ma_engine_get_time(&Audio::s_engine) + (ma_engine_get_sample_rate(&Audio::s_engine) * msDelay / 1000);
        ma_sound_set_stop_time_in_pcm_frames(&m_sound, stopTime);
    } else {
        ma_sound_stop(&m_sound);
    }
}

void Audio::toggle(uint64_t msDelay) {
    if (ma_sound_is_playing(&m_sound)) {
        pause(msDelay);
    } else {
        play(msDelay);
    }
}

uint64_t Audio::cursor() {
    ma_uint64 cursor;
    ma_sound_get_cursor_in_pcm_frames(&m_sound, &cursor);
    return cursor;
}

Assets<Audio> g_audio;

}  // namespace assets

