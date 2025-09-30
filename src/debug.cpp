#include "debug.h"
#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_opengl3.h"
#include <sstream>
#include <iomanip>

// windows ram usage
#ifdef _WIN32
#include <Windows.h>
#include <Psapi.h>
#endif

namespace Debug
{
    void Init(SDL_Window* window, SDL_GLContext ctx)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGui_ImplSDL3_InitForOpenGL(window, ctx);
        ImGui_ImplOpenGL3_Init("#version 430");
    }

    void ProcessEvents(SDL_Event e)
    {
        ImGui_ImplSDL3_ProcessEvent(&e);
    }

    void Shutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
    }

    void BeginFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }

    void EndFrame()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    static size_t GetRAMUsageMB()
    {
#ifdef _WIN32
        PROCESS_MEMORY_COUNTERS_EX pmc;
        if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
        {
            return pmc.WorkingSetSize / (1024 * 1024);
        }
#endif
        return 0;
    }

    void ShowOverlay(float deltaTime)
    {
        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoNav;

        ImGui::SetNextWindowPos(ImVec2(10, 10));
        ImGui::Begin("Debug Overlay", nullptr, flags);

        float fps = 1.0f / deltaTime;
        ImGui::Text("FPS: %.1f", fps);
        ImGui::Text("Frame Time: %.2f ms", deltaTime * 1000.0f);
        ImGui::Text("RAM %zu MB", GetRAMUsageMB());
        // Keybinds
        ImGui::Separator();
        ImGui::Text("Keybinds:");
        ImGui::Text("W,A,S,D - Movement");
        ImGui::Text("F1 - Toggle GL Polygon Mode");
        ImGui::Text("F2 - Show Settings");
        ImGui::End();
    }

    void ShowSettings(int MAX_RECTS)
    {
        if (!showSettings) return;
        ImGui::Begin("Settings");

        ImGui::SliderInt("Stress Test Amount", &STRESS_TEST_AMOUNT, 0, MAX_RECTS - 1);
        ImGui::TextWrapped("To use more in your stress test, you have to increase the 'MAX_RECTS' variable in the code!");

        ImGui::End();
    }

} // Debug