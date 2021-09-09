#pragma once

#include <SDL.h>
#include <SDL_mixer.h>

#include <string>

#include "./assets.h"

static constexpr char AudioRoot[] = "assets/audio/";

class Audio : public Asset {
 public:
    void load(const std::string &filepath) override {
        m_audio = Mix_LoadMUS((AudioRoot + filepath).c_str());
        if (!m_audio) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Mix_LoadMUS: %s",
                         Mix_GetError());
            exit(1);
        }
    }
    void unload() override {
        Mix_FreeMusic(m_audio);
        m_audio = nullptr;
    }
    void play() {
        if (Mix_PlayMusic(m_audio, 1) == -1) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Mix_PlayMusic: %s",
                         Mix_GetError());
            exit(1);
        }
    }

 private:
    Mix_Music *m_audio;
};

extern Assets<Audio> g_audio;

