#ifndef GAME_DEBUG_H
#define GAME_DEBUG_H

#include <SDL3/SDL.h>
#include <GL/glew.h>
#include "imgui/imgui.h"
#include <string>

namespace Debug
{
    inline int STRESS_TEST_AMOUNT = 0;
    inline bool showSettings = false;

    void Init(SDL_Window* window, SDL_GLContext ctx);
    void ProcessEvents(SDL_Event e);
    void Shutdown();
    void BeginFrame();
    void EndFrame();
    void ShowOverlay(float deltaTime);
    void ShowSettings(int MAX_RECTS);
} // Debug

#endif //GAME_DEBUG_H