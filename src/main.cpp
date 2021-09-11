
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <functional>
#include <string>

#include "ecs/ecs.h"
#include "src/assets.h"
#include "src/audio.h"
#include "src/chart.h"
#include "src/components/button.h"
#include "src/components/render.h"
#include "src/components/tags.h"
#include "src/components/transform.h"
#include "src/event.h"
#include "src/fps.h"
#include "src/keyboard.h"
#include "src/logging.h"
#include "src/renderer.h"
#include "src/systems/button.h"
#include "src/systems/render.h"
#include "src/systems/target.h"
#include "src/systems/transform.h"
#include "src/texture.h"
#include "src/timer.h"

constexpr bool VSYNC_ENABLE = true;
SDL_Renderer *g_renderer;
ECS g_ecs;
EventManager g_eventManager;
Assets<Texture> g_textures;
Assets<chart::Chart> g_charts;
Assets<Audio> g_audio;
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
    LOG_INFO("SDL successfully initialized!\n");
    SDL_version compileVersion;
    SDL_MIXER_VERSION(&compileVersion);
    const SDL_version *runtimeVersion = Mix_Linked_Version();
    LOG_INFO("SDL_mixer: C %d.%d.%d, R %d.%d.%d",
             compileVersion.major, compileVersion.minor, compileVersion.patch,
             runtimeVersion->major, runtimeVersion->minor, runtimeVersion->patch);

    // SET UP WINDOW //
    SDL_Window *g_window = SDL_CreateWindow("treeline", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 750, 500, 0);
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | (VSYNC_ENABLE ? SDL_RENDERER_PRESENTVSYNC : 0);
    g_renderer = SDL_CreateRenderer(g_window, -1, render_flags);
    if (g_renderer == nullptr) {
        LOG_ERROR("SDL_CreateRenderer: %s", SDL_GetError());
        exit(1);
    }

    // SET UP MIXER //
    int mixer_flags = MIX_INIT_MP3;
    if (mixer_flags != (Mix_Init(mixer_flags) & mixer_flags)) {
        LOG_ERROR("Mix_Init: %s", Mix_GetError());
        exit(1);
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
        LOG_ERROR("Mix_OpenAudio: %s", Mix_GetError());
        exit(1);
    }

    // REGISTER COMPONENTS //
    g_ecs.registerComponent<CSprite>();
    g_ecs.registerComponent<CTranslation>();
    g_ecs.registerComponent<CVelocity>();
    g_ecs.registerComponent<CButton>();
    g_ecs.registerComponent<CTargetTag>();

    Archetype velocityArch =
        g_ecs.createArchetype(g_ecs.getComponentId<CVelocity>(),
                              g_ecs.getComponentId<CTranslation>());
    std::shared_ptr<VelocitySystem> velocitySystem =
        g_ecs.registerSystem<VelocitySystem>(velocityArch);

    Archetype buttonArch = g_ecs.createArchetype(
        g_ecs.getComponentId<CSprite>(), g_ecs.getComponentId<CButton>());
    std::shared_ptr<ButtonSystem> buttonSystem =
        g_ecs.registerSystem<ButtonSystem>(buttonArch);

    Archetype targetArch = g_ecs.createArchetype(
        g_ecs.getComponentId<CSprite>(), g_ecs.getComponentId<CTranslation>(),
        g_ecs.getComponentId<CVelocity>(), g_ecs.getComponentId<CTargetTag>());
    std::shared_ptr<TargetSystem> targetSystem =
        g_ecs.registerSystem<TargetSystem>(targetArch);

    Archetype renderArch = g_ecs.createArchetype(
        g_ecs.getComponentId<CSprite>(), g_ecs.getComponentId<CTranslation>());
    std::shared_ptr<RenderSystem> renderSystem =
        g_ecs.registerSystem<RenderSystem>(renderArch);

    unsigned int baba = g_textures.load("baba.png");
    unsigned int red = g_textures.load("red.png");
    unsigned int grey = g_textures.load("grey.png");

    g_charts.load("test.chart");

    Entity babaEntity = g_ecs.createEntity();
    g_ecs.addComponents(babaEntity, CSprite{baba, 100, 100},
                        CTranslation{30, 10}, CVelocity{50, 0});

    Entity dEntity = g_ecs.createEntity();
    g_ecs.addComponents<CSprite, CTranslation, CButton>(
        dEntity, CSprite{grey, 50, 50}, CTranslation{100, 100},
        CButton{Key::NOTE_A, red, grey});
    Entity fEntity = g_ecs.createEntity();
    g_ecs.addComponents<CSprite, CTranslation, CButton>(
        fEntity, CSprite{grey, 50, 50}, CTranslation{200, 100},
        CButton{Key::NOTE_B, red, grey});
    Entity jEntity = g_ecs.createEntity();
    g_ecs.addComponents<CSprite, CTranslation, CButton>(
        jEntity, CSprite{grey, 50, 50}, CTranslation{300, 100},
        CButton{Key::NOTE_C, red, grey});
    Entity kEntity = g_ecs.createEntity();
    g_ecs.addComponents<CSprite, CTranslation, CButton>(
        kEntity, CSprite{grey, 50, 50}, CTranslation{400, 100},
        CButton{Key::NOTE_D, red, grey});

    std::function<void(void)> f = []() { g_fpsCounter.report(); };
    Timer fpsCounterTimer = Timer{f, 1000};

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
    g_eventManager.addListener(EventType::KEYBOARD, [](const Event &event) {
        if (event.getParam<Key>("key") == Key::SELECT &&
            event.getParam<bool>("down") == false) {
            unsigned int song = g_audio.load("music.mp3");
            g_audio.get(song)->play();
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

    g_charts.unloadAll();
    g_textures.unloadAll();
    g_audio.unloadAll();

    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    return 0;
}

