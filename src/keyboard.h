#pragma once

#include <SDL.h>

#include <bitset>
#include <unordered_map>

enum class Key : size_t {
    NOTE_A,
    NOTE_B,
    NOTE_C,
    NOTE_D,
    SELECT,
    EXIT,
    METRICS,
    Count  // hack to get the number of keys
};

struct KeyEvent {
    SDL_Keycode keycode;
    bool down;
};

class Keyboard {
 public:
    Keyboard() : m_indexMap{
                     {SDLK_d, Key::NOTE_A},
                     {SDLK_f, Key::NOTE_B},
                     {SDLK_j, Key::NOTE_C},
                     {SDLK_k, Key::NOTE_D},
                     {SDLK_RETURN, Key::SELECT},
                     {SDLK_ESCAPE, Key::EXIT},
                     {SDLK_F12, Key::METRICS}} {}
    bool get(Key key) {
        return m_keys.test(static_cast<size_t>(key));
    }
    bool get(SDL_Keycode keycode) {
        return get(toKey(keycode));
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

