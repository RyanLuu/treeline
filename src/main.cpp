#include <stdio.h>
#include <SDL2/SDL.h>

#include "window.h"

#define FPS 60

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Log("SDL successfully initialized!\n");

    Window window("hello world");

    bool running = true;
    while (running) {
        SDL_Event event;
        if (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                default:
                    break;
            }
        }
        window.clear();
        SDL_Delay(1000 / FPS);
    }

    return 0;
}
