#pragma once

#include <SDL.h>
#include <SDL_mixer.h>

#include <string>

#include "src/assets.h"
#include "src/logging.h"

static constexpr char AudioRoot[] = "assets/audio/";

class Audio : public Asset {
 public:
    void load(const std::string &filepath) override {
        m_audio = Mix_LoadMUS((AudioRoot + filepath).c_str());
        if (!m_audio) {
            LOG_ERROR("Mix_LoadMUS: %s", Mix_GetError());
            exit(1);
        }
        LOG_INFO("Loaded audio [%s]", filepath.c_str());
    }
    void unload() override {
        Mix_FreeMusic(m_audio);
        m_audio = nullptr;
    }
    void play() {
        if (Mix_PlayMusic(m_audio, 1) == -1) {
            LOG_ERROR("Mix_PlayMusic: %s", Mix_GetError());
            exit(1);
        }
    }

 private:
    Mix_Music *m_audio;
};

extern Assets<Audio> g_audio;

