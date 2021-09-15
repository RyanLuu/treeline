
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <functional>
#include <string>

#include "assets/audio.h"
#include "assets/chart.h"
#include "assets/font.h"
#include "assets/texture.h"
#include "components/button.h"
#include "components/render.h"
#include "components/tags.h"
#include "components/transform.h"
#include "core/ecs.h"
#include "core/event.h"
#include "core/fps.h"
#include "core/logging.h"
#include "core/timer.h"
#include "io/keyboard.h"
#include "io/renderer.h"
#include "systems/button.h"
#include "systems/render.h"
#include "systems/target.h"
#include "systems/transform.h"

constexpr bool VSYNC_ENABLE = true;
SDL_Renderer *g_renderer;
ECS g_ecs;
EventManager g_eventManager;
Keyboard g_keyboard;
uint64_t updateTime;

void render(void) {
    // clear renderer
    SDL_SetRenderDrawColor(g_renderer, 63, 63, 63, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(g_renderer);

    uint64_t now = time_now();
    // Use 4 bytes rather than 8 for dt (2^32 us > 1 hr)
    uint32_t dt = now - updateTime;
    updateTime = now;
    g_ecs.updateSystems(dt);

    // present backbuffer
    SDL_RenderPresent(g_renderer);
    g_fpsCounter.incrementFrames();
}

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    logInit();

    // initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        LOG_ERROR("SDL_Init: %s\n", SDL_GetError());
        exit(1);
    }
    LOG_INFO("SDL successfully initialized!");
    if (TTF_Init() != 0) {
        LOG_ERROR("TTF_Init: %s\n", TTF_GetError());
        exit(1);
    }

    // SET UP WINDOW //
    SDL_Window *g_window = SDL_CreateWindow("treeline", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 750, 500, 0);
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | (VSYNC_ENABLE ? SDL_RENDERER_PRESENTVSYNC : 0);
    g_renderer = SDL_CreateRenderer(g_window, -1, render_flags);
    if (g_renderer == nullptr) {
        LOG_ERROR("SDL_CreateRenderer: %s", SDL_GetError());
        exit(1);
    }

    // ENABLE AUDIO
    assets::Audio::enable();

    // LOAD TEXTURES //
    assets::AssetId g_targetTexture = assets::g_textures.load("target.png");

    Archetype velocityArch = g_ecs.createArchetype(g_ecs.getComponentId<CVelocity>(),
                                                   g_ecs.getComponentId<CTranslation>());
    std::shared_ptr<VelocitySystem> velocitySystem = g_ecs.registerSystem<VelocitySystem>(velocityArch);

    Archetype buttonArch = g_ecs.createArchetype(g_ecs.getComponentId<CRender>(),
                                                 g_ecs.getComponentId<CSprite>(),
                                                 g_ecs.getComponentId<CButton>());
    std::shared_ptr<ButtonSystem> buttonSystem = g_ecs.registerSystem<ButtonSystem>(buttonArch);

    Archetype targetArch = g_ecs.createArchetype(g_ecs.getComponentId<CRender>(),
                                                 g_ecs.getComponentId<CSprite>(),
                                                 g_ecs.getComponentId<CTranslation>(),
                                                 g_ecs.getComponentId<CVelocity>(),
                                                 g_ecs.getComponentId<CTargetTag>());
    std::shared_ptr<TargetSystem> targetSystem = g_ecs.registerSystem<TargetSystem>(targetArch);

    Archetype renderArch = g_ecs.createArchetype(g_ecs.getComponentId<CRender>(),
                                                 g_ecs.getComponentId<CTranslation>());
    std::shared_ptr<RenderSystem> renderSystem = g_ecs.registerRenderSystem<RenderSystem>(renderArch);

    assets::AssetId baba = assets::g_textures.load("baba.png");
    assets::AssetId red = assets::g_textures.load("red.png");
    assets::AssetId grey = assets::g_textures.load("grey.png");

    assets::AssetId chartId = assets::g_charts.load("test.chart");
    assets::AssetId songId = assets::g_audio.load("music.mp3");
    assets::AssetId frutigerId = assets::g_fonts.load("frutiger.ttf");

    Entity babaEntity = g_ecs.createEntity();
    g_ecs.addComponents(babaEntity, CRender{1}, CSprite{baba, 40, 40},
                        CTranslation{30, 10}, CVelocity{50, 0});

    Entity helloEntity = g_ecs.createEntity();
    g_ecs.addComponents(helloEntity, CRender{4}, CText{frutigerId, SDL_Color{255, 255, 255, 127}, 0}, CTranslation{200, 120});

    for (size_t i = 0; i < 6; i++) {
        Entity buttonEntity = g_ecs.createEntity();
        g_ecs.addComponents(buttonEntity, CRender{1}, CSprite{grey, 50, 50},
                            CTranslation{100.f + 100 * i, 100}, CButton{static_cast<Key>(i), red, grey});
    }

    std::function<void(void)> f = []() { g_fpsCounter.report(); };
    Timer fpsCounterTimer = Timer{f, 1'000'000};

    // QUIT EVENT LISTENER //
    bool running = true;
    g_eventManager.addListener(EventType::KEYBOARD, [](const Event &event) {
        if (event.getParam<Key>("key") == Key::EXIT &&
            event.getParam<bool>("down") == true) {
            SDL_Event quitEvent = {.type = SDL_QUIT};
            SDL_PushEvent(&quitEvent);
        }
    });

    // METRICS EVENT LISTENER //
    g_eventManager.addListener(EventType::KEYBOARD, [&fpsCounterTimer](const Event &event) {
        if (event.getParam<Key>("key") == Key::METRICS &&
            event.getParam<bool>("down") == true) {
            fpsCounterTimer.toggle();
        }
    });

    // PLAY SONG EVENT LISTENER //
    g_eventManager.addListener(EventType::KEYBOARD, [g_targetTexture, songId, chartId](const Event &event) {
        if (event.getParam<Key>("key") == Key::SELECT &&
            event.getParam<bool>("down") == false) {
            assets::g_audio.get(songId)->toggle(1000);
            LOG_INFO("%lu", assets::g_audio.get(songId)->cursor());
            auto notes = assets::g_charts.get(chartId)->getNotes();
            for (auto note : notes) {
                int delay = 1'000'000;
                float speed = 300;
                float x = 100 + note.value * 100;
                float y = 100 + speed * (note.time + delay) / 1'000'000;
                Entity newTarget = g_ecs.createEntity();
                g_ecs.addComponents(newTarget, CRender{3}, CSprite{g_targetTexture, 50, 50}, CTranslation{x, y}, CVelocity{0, -speed}, CTargetTag{});
            }
        }
    });

    updateTime = time_now();
    Timer renderTimer = Timer{render};
    fpsCounterTimer.start();
    renderTimer.start();

    // EVENT LOOP //
    SDL_Event event;
    while (running) {
        if (SDL_WaitEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: {
                    LOG_INFO("SDL_QUIT signal received.");
                    running = false;
                } break;
                case SDL_KEYDOWN:
                case SDL_KEYUP: {
                    auto now = time_now();
                    g_keyboard.set(event.key.keysym.sym, event.type == SDL_KEYDOWN);
                    Event keyboardEvent = {
                        .type = EventType::KEYBOARD,
                        .params = {
                            {"key", g_keyboard.toKey(event.key.keysym.sym)},
                            {"down", event.type == SDL_KEYDOWN},
                            {"time", now}}};
                    g_eventManager.sendEvent(keyboardEvent);
                } break;
                default:
                    break;
            }
        }
    }

    if (fpsCounterTimer.isRunning()) {
        fpsCounterTimer.stop();
    }
    renderTimer.stop();

    assets::g_charts.unloadAll();
    assets::g_textures.unloadAll();
    assets::g_audio.unloadAll();
    assets::g_fonts.unloadAll();

    assets::Audio::disable();

    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}

