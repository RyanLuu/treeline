#include "window.h"

Window::Window(std::string title, size_t width, size_t height) {
    m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    if (m_window == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    m_renderer = SDL_CreateRenderer(m_window, -1, render_flags);
    if (m_renderer == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        exit(1);
    }
}

Window::~Window() {
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
}

void Window::clear(Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetRenderDrawColor(m_renderer, r, g, b, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(m_renderer);
}
