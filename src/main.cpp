
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <chrono>

#include "assets.h"
#include "audio.h"
#include "chart.h"
#include "components/button.h"
#include "components/render.h"
#include "components/tags.h"
#include "components/transform.h"
#include "ecs.h"
#include "event.h"
#include "fps.h"
#include "keyboard.h"
#include "renderer.h"
#include "systems/button.h"
#include "systems/render.h"
#include "systems/target.h"
#include "systems/transform.h"
#include "texture.h"
#include "timer.h"

constexpr bool VSYNC_ENABLE = true;
SDL_Renderer *g_renderer;
ECS g_ecs;
EventManager g_eventManager;
Assets<Texture> g_textures;
Assets<Chart> g_charts;
Assets<Audio> g_audio;
Keyboard g_keyboard;
auto updateTime = std::chrono::system_clock::now();

void render() {
    // clear renderer
    SDL_SetRenderDrawColor(g_renderer, 63, 63, 63, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(g_renderer);

    auto now = std::chrono::system_clock::now();
    double dt = std::chrono::duration_cast<std::chrono::duration<double, std::chrono::seconds::period>>(now - updateTime).count();
    updateTime = now;
    g_ecs.updateSystems(dt);

    // present backbuffer
    SDL_RenderPresent(g_renderer);
    g_fpsCounter.incrementFrames();
}

int main(int argc, char *argv[]) {

    // initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_Log("SDL successfully initialized!\n");
    SDL_version compileVersion;
    SDL_MIXER_VERSION(&compileVersion);
    const SDL_version *runtimeVersion = Mix_Linked_Version();
    SDL_Log("SDL_mixer: C %d.%d.%d, R %d.%d.%d",
            compileVersion.major, compileVersion.minor, compileVersion.patch,
            runtimeVersion->major, runtimeVersion->minor, runtimeVersion->patch);

    // SET UP WINDOW //
    SDL_Window *g_window = SDL_CreateWindow("treeline", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 750, 500, 0);
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | (VSYNC_ENABLE ? SDL_RENDERER_PRESENTVSYNC : 0);
    g_renderer = SDL_CreateRenderer(g_window, -1, render_flags);
    if (g_renderer == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateRenderer: %s", SDL_GetError());
        exit(1);
    }

    // SET UP MIXER //
    int mixer_flags = MIX_INIT_MP3;
    if (mixer_flags != (Mix_Init(mixer_flags) & mixer_flags)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Mix_Init: %s", Mix_GetError());
        exit(1);
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Mix_OpenAudio: %s", Mix_GetError());
        exit(1);
    }

    // REGISTER COMPONENTS //
    g_ecs.registerComponent<CSprite>();
    g_ecs.registerComponent<CTranslation>();
    g_ecs.registerComponent<CVelocity>();
    g_ecs.registerComponent<CButton>();
    g_ecs.registerComponent<CTargetTag>();

    Archetype velocityArch = g_ecs.createArchetype(g_ecs.getComponentId<CVelocity>(), g_ecs.getComponentId<CTranslation>());
    std::shared_ptr<VelocitySystem> velocitySystem = g_ecs.registerSystem<VelocitySystem>(velocityArch);

    Archetype buttonArch = g_ecs.createArchetype(g_ecs.getComponentId<CSprite>(), g_ecs.getComponentId<CButton>());
    std::shared_ptr<ButtonSystem> buttonSystem = g_ecs.registerSystem<ButtonSystem>(buttonArch);

    Archetype targetArch = g_ecs.createArchetype(g_ecs.getComponentId<CSprite>(), g_ecs.getComponentId<CTranslation>(), g_ecs.getComponentId<CVelocity>(), g_ecs.getComponentId<CTargetTag>());
    std::shared_ptr<TargetSystem> targetSystem = g_ecs.registerSystem<TargetSystem>(targetArch);

    Archetype renderArch = g_ecs.createArchetype(g_ecs.getComponentId<CSprite>(), g_ecs.getComponentId<CTranslation>());
    std::shared_ptr<RenderSystem> renderSystem = g_ecs.registerSystem<RenderSystem>(renderArch);

    unsigned int baba = g_textures.load("baba.png");
    unsigned int red = g_textures.load("red.png");
    unsigned int grey = g_textures.load("grey.png");

    g_charts.load("test.chart");

    unsigned int song = g_audio.load("music.mp3");
    g_audio.get(song)->play();

    Entity babaEntity = g_ecs.createEntity();
    g_ecs.addComponents(babaEntity, CSprite{baba, 100, 100}, CTranslation{30, 10}, CVelocity{50, 0});

    Entity dEntity = g_ecs.createEntity();
    g_ecs.addComponents<CSprite, CTranslation, CButton>(dEntity, CSprite{grey, 50, 50}, CTranslation{100, 100}, CButton{SDLK_d, red, grey});
    Entity fEntity = g_ecs.createEntity();
    g_ecs.addComponents<CSprite, CTranslation, CButton>(fEntity, CSprite{grey, 50, 50}, CTranslation{200, 100}, CButton{SDLK_f, red, grey});
    Entity jEntity = g_ecs.createEntity();
    g_ecs.addComponents<CSprite, CTranslation, CButton>(jEntity, CSprite{grey, 50, 50}, CTranslation{300, 100}, CButton{SDLK_j, red, grey});
    Entity kEntity = g_ecs.createEntity();
    g_ecs.addComponents<CSprite, CTranslation, CButton>(kEntity, CSprite{grey, 50, 50}, CTranslation{400, 100}, CButton{SDLK_k, red, grey});

    // QUIT EVENT LISTENER (ESC) //
    bool running = true;
    g_eventManager.addListener(EventType::KEYBOARD, [](const Event &event) {
        if (event.getParam<Key>("key") == Key::EXIT && event.getParam<bool>("down") == false) {
            SDL_Event quitEvent = {.type = SDL_QUIT};
            SDL_PushEvent(&quitEvent);
        }
    });

    Timer fpsCounterTimer = Timer([]() {g_fpsCounter.report();}, 1000);
    Timer renderTimer = Timer(render, 0);
    updateTime = std::chrono::system_clock::now();
    fpsCounterTimer.start();
    renderTimer.start();
    SDL_Event event;

    // EVENT LOOP //
    while (running) {
        if (SDL_WaitEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    SDL_Log("SDL_QUIT signal received.");
                    running = false;
                    break;
                case SDL_KEYDOWN:
                case SDL_KEYUP: {
                        g_keyboard.set(event.key.keysym.sym, event.type == SDL_KEYDOWN);
                        Event keyboardEvent = {
                            .type = EventType::KEYBOARD,
                            .params = {
                                {"key", g_keyboard.toKey(event.key.keysym.sym)},
                                {"down", event.type == SDL_KEYDOWN}}};
                        g_eventManager.sendEvent(keyboardEvent);
                    }
                    break;
                default:
                    break;
            }
        }
    }

    fpsCounterTimer.stop();
    renderTimer.stop();

    g_charts.unloadAll();
    g_textures.unloadAll();
    g_audio.unloadAll();

    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    return 0;
}

