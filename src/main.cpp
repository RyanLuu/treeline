#include <stdio.h>
#include <SDL2/SDL.h>

#include "window.h"

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }
    printf("SDL successfully initialized!\n");

    Window window("hello world");

    SDL_Quit();
    return 0;
}
