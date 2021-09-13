#pragma once

#include <string>

#include "assets/assets.h"
#include "lib/miniaudio.h"

namespace assets {

struct UserData {
    ma_decoder decoder;
    bool playing;
};

class Audio : public Asset {
   public:
    void load(const std::string &filepath) override;
    void unload() override;
    void play();
    void pause();
    void toggle();

   private:
    ma_device m_device;
    UserData m_userData;
};

extern Assets<Audio> g_audio;

}  // namespace assets

