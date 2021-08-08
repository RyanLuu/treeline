#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "assets.h"
#include "chart.h"
#include "components/render.h"
#include "components/transform.h"
#include "ecs.h"
#include "keyboard.h"
#include "renderer.h"
#include "systems/render.h"
#include "systems/transform.h"
#include "texture.h"

constexpr unsigned int FPS = 100;
constexpr unsigned int NOMINAL_DT = 1000 / FPS;

SDL_Renderer *g_renderer;
ECS g_ecs;
Assets<Texture> g_textures;
Assets<Chart> g_charts;
Keyboard g_keyboard;

int main(int argc, char *argv[]) {

    // initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Log("SDL successfully initialized!\n");

    SDL_Window *g_window = SDL_CreateWindow("treeline", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 400, 0);

    Uint32 render_flags = SDL_RENDERER_ACCELERATED;
    g_renderer = SDL_CreateRenderer(g_window, -1, render_flags);
    if (g_renderer == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(g_window);
        SDL_Quit();
        exit(1);
    }

    // register components
    g_ecs.registerComponent<CSprite>();
    g_ecs.registerComponent<CTranslation>();
    g_ecs.registerComponent<CVelocity>();

    Archetype renderArch = g_ecs.createArchetype(g_ecs.getComponentId<CSprite>(), g_ecs.getComponentId<CTranslation>());
    std::shared_ptr<RenderSystem> renderSystem = g_ecs.registerSystem<RenderSystem>(renderArch);

    Archetype velocityArch = g_ecs.createArchetype(g_ecs.getComponentId<CVelocity>(), g_ecs.getComponentId<CTranslation>());
    std::shared_ptr<VelocitySystem> velocitySystem = g_ecs.registerSystem<VelocitySystem>(velocityArch);

    unsigned int baba = g_textures.load("baba.png");

    unsigned int chart = g_charts.load("test.chart");
    g_charts.unload(chart);

    Entity babaEntity = g_ecs.createEntity();
    g_ecs.addComponents<CSprite, CTranslation, CVelocity>(babaEntity, CSprite{baba, 100, 100}, CTranslation{30, 10}, CVelocity{50, 0});

    bool running = true;
    unsigned int updateTime = SDL_GetTicks();;
    while (true) {
        unsigned int frameStart = SDL_GetTicks();

        // exhaust event queue
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYDOWN:
                    SDL_Keycode keycode = event.key.keysym.sym;
                    if (g_keyboard.has(keycode)) {
                        g_keyboard.set(keycode, true);
                    }
                    break;
                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        running = false;
                    }
                    SDL_Keycode keycode = event.key.keysym.sym;
                    if (g_keyboard.has(keycode)) {
                        g_keyboard.set(keycode, false);
                    }
                    break;
                case SDL_QUIT:
                    running = false;
                    break;
                default:
                    break;
            }
        }

        if (!running) {
            break;
        }

        // clear renderer
        SDL_SetRenderDrawColor(g_renderer, 63, 63, 63, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(g_renderer);

        unsigned int now = SDL_GetTicks();
        unsigned int dt = now - updateTime;
        updateTime = now;
        g_ecs.updateSystems(dt);

        // present backbuffer
        SDL_RenderPresent(g_renderer);

        // delay to ensure each pass through the loop is ~NOMINAL_DT
        int timeLeft = NOMINAL_DT - (SDL_GetTicks() - frameStart);
        if (timeLeft > 0) {
            SDL_Delay(timeLeft);
        }
    }

    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    return 0;
}

