#pragma once

#include <bitset>
#include <unordered_map>
#include <SDL2/SDL.h>

enum class Key : size_t {
    KEY_LL, KEY_L, KEY_R, KEY_RR, EXIT,
    Count  // hack to get the number of keys
};

struct KeyEvent {
    SDL_Keycode keycode;
    bool down;
};

class Keyboard {
    public:
        Keyboard() : m_indexMap{
            {SDLK_d, Key::KEY_LL},
                {SDLK_f, Key::KEY_L},
                {SDLK_j, Key::KEY_R},
                {SDLK_k, Key::KEY_RR},
                {SDLK_ESCAPE, Key::EXIT}
        } {}
        bool get(SDL_Keycode keycode) {
            return m_keys.test(static_cast<size_t>(m_indexMap[keycode]));
        }
        void set(SDL_Keycode keycode, bool b) {
            m_keys.set(static_cast<size_t>(m_indexMap[keycode]), b);
        }
        bool has(SDL_Keycode keycode) {
            return m_indexMap.find(keycode) != m_indexMap.end();
        }
        Key toKey(SDL_Keycode keycode) {
            return m_indexMap[keycode];
        }
    private:
        std::bitset<static_cast<size_t>(Key::Count)> m_keys;
        std::unordered_map<SDL_Keycode, Key> m_indexMap;
};

extern Keyboard g_keyboard;

