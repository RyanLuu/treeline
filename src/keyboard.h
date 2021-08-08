#pragma once

#include <bitset>
#include <unordered_map>
#include <SDL2/SDL.h>

const size_t NUM_KEYS = 4;

class Keyboard {
    public:
        Keyboard() : m_indexMap{
            {SDLK_d, 0},
            {SDLK_f, 1},
            {SDLK_j, 2},
            {SDLK_k, 3},
        } {}
        bool get(SDL_Keycode keycode) {
            return m_keys.test(m_indexMap[keycode]);
        }
        void set(SDL_Keycode keycode, bool b) {
            m_keys.set(m_indexMap[keycode], b);
        }
        bool has(SDL_Keycode keycode) {
            return m_indexMap.find(keycode) != m_indexMap.end();
        }
    private:
        std::bitset<NUM_KEYS> m_keys;
        std::unordered_map<SDL_Keycode, size_t> m_indexMap;
};
