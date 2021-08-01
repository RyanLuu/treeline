#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>
#include <string>

#define DEFAULT_WIDTH (640)
#define DEFAULT_HEIGHT (480)

class Window {
    public:
        Window(std::string title, size_t width = DEFAULT_WIDTH, size_t height = DEFAULT_HEIGHT);
        ~Window();
    private:
        SDL_Window *m_window;
        SDL_Renderer *m_renderer;
};

#endif /* WINDOW_H */
