#include <SDL2/SDL.h>

struct CButton {
    SDL_Keycode keycode;
    unsigned int activeSprite;
    unsigned int idleSprite;
};

