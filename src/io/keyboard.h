#pragma once

#include <SDL.h>

#include <bitset>
#include <unordered_map>

enum class Key : uint8_t {
    NOTE_A,
    NOTE_B,
    NOTE_C,
    NOTE_D,
    NOTE_E,
    NOTE_F,
    SELECT,
    EXIT,
    METRICS,
    Count,  // hack to get the number of keys
    INVALID = 0xFF
};

struct KeyEvent {
    SDL_Keycode keycode;
    bool down;
};

class Keyboard {
 public:
    Keyboard() : m_indexMap{
                     {SDLK_t, Key::NOTE_A},
                     {SDLK_f, Key::NOTE_B},
                     {SDLK_v, Key::NOTE_C},
                     {SDLK_b, Key::NOTE_D},
                     {SDLK_h, Key::NOTE_E},
                     {SDLK_y, Key::NOTE_F},
                     {SDLK_g, Key::SELECT},
                     {SDLK_ESCAPE, Key::EXIT},
                     {SDLK_F12, Key::METRICS}} {}

    bool get(Key key) {
        return m_keys.test(static_cast<size_t>(key));
    }

    void set(SDL_Keycode keycode, bool b) {
        if (has(keycode)) {
            m_keys.set(static_cast<size_t>(m_indexMap[keycode]), b);
        }
    }

    bool has(SDL_Keycode keycode) {
        return m_indexMap.find(keycode) != m_indexMap.end();
    }

    Key toKey(SDL_Keycode keycode) {
        return has(keycode) ? m_indexMap[keycode] : Key::INVALID;
    }

 private:
    std::bitset<static_cast<size_t>(Key::Count)> m_keys;
    std::unordered_map<SDL_Keycode, Key> m_indexMap;
};

extern Keyboard g_keyboard;

