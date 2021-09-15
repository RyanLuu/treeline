#pragma once

#include "assets/assets.h"
#include "lib/miniaudio/miniaudio.h"

namespace assets {

struct UserData {
    ma_decoder decoder;
    bool playing;
};

class Audio : public Asset {
   public:
    static bool enable();
    static void disable();

    void load(const std::string &filepath) override;
    void unload() override;
    void play(uint64_t msDelay = 0);
    void pause(uint64_t msDelay = 0);
    void toggle(uint64_t msDelay = 0);
    uint64_t cursor();

   private:
    static ma_resource_manager s_rm;
    static ma_context s_context;
    static ma_device s_device;
    static ma_engine s_engine;

    ma_sound m_sound;
};

extern Assets<Audio> g_audio;

}  // namespace assets

