// main.cpp
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib> // for rand()

#include "debug.h"
#include "shaders.h"

using namespace glm;

// ------------------------------
// STRUCTS
// ------------------------------

struct Rect
{
    mat4 projection;           // orthographic projection
    vec2 pos = vec2(100.0f, 100.0f);
    vec2 size = vec2(64.0f, 64.0f);
    vec4 color = vec4(1.0f);

    Rect()
    {
        int w, h;
        SDL_GetWindowSize(SDL_GL_GetCurrentWindow(), &w, &h);
        set_projection(w, h);
    }

    void set_projection(int w, int h)
    {
        projection = ortho(0.0f, (float)w, (float)h, 0.0f, -1.0f, 1.0f);
    }
};

struct RectInstance
{
    vec2 pos;
    vec2 size;
    vec4 color;
};

struct Player : Rect
{
    // movement flags
    bool moveUp = false;
    bool moveDown = false;
    bool moveLeft = false;
    bool moveRight = false;

    float speed = 450.0f; // pixels per second
};

// ------------------------------
// GLOBALS
// ------------------------------
SDL_Window* window = nullptr;
SDL_GLContext ctx = nullptr;
const char* windowTitle = "BatchedRenderTest";
int windowWidth = 1280;
int windowHeight = 720;
bool isRunning = false;
bool isPolygonModeLine = false;

// OpenGL handles
unsigned int VAO, VBO, EBO, instanceVBO, ShaderProgram;

// settings
int MAX_RECTS = 10001;
Player* player = nullptr;

// ------------------------------
// FUNCTION DECLARATIONS
// ------------------------------

void shutdown();
unsigned int compile_shader(unsigned int type, const char* source);
void set_mat4(const std::string& name, const mat4& matrix);
void set_vec2(const std::string& name, const vec2& vec);
void set_vec4(const std::string& name, const vec4& vec);
void draw_rect(Rect* rect, const void* indices);
void draw_rect_batched(const std::vector<RectInstance>& rects);

// ------------------------------
// MAIN
// ------------------------------
int main()
{
    // --------------------------
    // Initialize SDL + OpenGL
    // --------------------------
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    // OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // create window
    window = SDL_CreateWindow(windowTitle, windowWidth, windowHeight, SDL_WINDOW_OPENGL);
    if (!window) { std::cerr << "Failed to create window: " << SDL_GetError() << std::endl; return -1; }

    // create context
    ctx = SDL_GL_CreateContext(window);
    if (!ctx) { std::cerr << "Failed to create GL context: " << SDL_GetError() << std::endl; return -1; }

    // init glew
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { std::cerr << "Failed to init GLEW" << std::endl; return -1; }

    glViewport(0, 0, windowWidth, windowHeight);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // init debug overlay
    Debug::Init(window, ctx);

    isRunning = true;

    // --------------------------
    // Shaders
    // --------------------------
    unsigned int vertexShader   = compile_shader(GL_VERTEX_SHADER, Shaders::RECT_VERTEX_SHADER);
    unsigned int fragmentShader = compile_shader(GL_FRAGMENT_SHADER, Shaders::RECT_FRAGMENT_SHADER);

    ShaderProgram = glCreateProgram();
    glAttachShader(ShaderProgram, vertexShader);
    glAttachShader(ShaderProgram, fragmentShader);
    glLinkProgram(ShaderProgram);

    // check program
    int success;
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        int length;
        glGetProgramiv(ShaderProgram, GL_INFO_LOG_LENGTH, &length);
        std::string infoLog(length, '\0');
        glGetProgramInfoLog(ShaderProgram, length, &length, &infoLog[0]);
        std::cerr << "Failed to link shader program: " << infoLog << std::endl;
        return -1;
    }

    // cleanup shaders
    glDetachShader(ShaderProgram, vertexShader);
    glDetachShader(ShaderProgram, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // --------------------------
    // Vertex data
    // --------------------------
    float vertices[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };
    unsigned int indices[] = { 0, 1, 3, 3, 2, 1 };

    // generate buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // instance buffer
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(RectInstance) * MAX_RECTS, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(RectInstance), (void*)0);
    glVertexAttribDivisor(1, 1);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(RectInstance), (void*)(sizeof(vec2)));
    glVertexAttribDivisor(2, 1);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(RectInstance), (void*)(2 * sizeof(vec2)));
    glVertexAttribDivisor(3, 1);

    glBindVertexArray(0);

    // --------------------------
    // Init player
    // --------------------------
    player = new Player();

    // --------------------------
    // Main loop
    // --------------------------
    Uint64 lastTime = SDL_GetTicks();
    while (isRunning)
    {
        Uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        // ----------------------
        // Handle events
        // ----------------------
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            Debug::ProcessEvents(e);

            if (e.type == SDL_EVENT_QUIT) isRunning = false;

            if (e.type == SDL_EVENT_KEY_DOWN)
            {
                if (e.key.key == SDLK_F1 && !e.key.repeat)
                {
                    isPolygonModeLine = !isPolygonModeLine;
                    glPolygonMode(GL_FRONT_AND_BACK, isPolygonModeLine ? GL_LINE : GL_FILL);
                }
                if (e.key.key == SDLK_F2 && !e.key.repeat)
                {
                    Debug::showSettings = !Debug::showSettings;
                }
            }

            // player movement
            bool pressed = (e.type == SDL_EVENT_KEY_DOWN);
            if (e.key.key == SDLK_W) player->moveUp    = pressed;
            if (e.key.key == SDLK_S) player->moveDown  = pressed;
            if (e.key.key == SDLK_A) player->moveLeft  = pressed;
            if (e.key.key == SDLK_D) player->moveRight = pressed;
        }

        // ----------------------
        // Update
        // ----------------------
        if (player->moveUp)    player->pos.y -= player->speed * deltaTime;
        if (player->moveDown)  player->pos.y += player->speed * deltaTime;
        if (player->moveLeft)  player->pos.x -= player->speed * deltaTime;
        if (player->moveRight) player->pos.x += player->speed * deltaTime;

        // player collision with window border
        if (player->pos.y < 0)
            player->pos.y = 0;
        if (player->pos.x < 0)
            player->pos.x = 0;
        if (player->pos.x + player->size.x > windowWidth)
            player->pos.x = windowWidth - player->size.x;
        if (player->pos.y + player->size.y > windowHeight)
            player->pos.y = windowHeight - player->size.y;

        // ----------------------
        // Render
        // ----------------------
        glClearColor(0.0f, 0.0f, 0.0f , 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Debug overlay
        Debug::BeginFrame();
        Debug::ShowSettings(MAX_RECTS);

        // prepare rectangles
        std::vector<RectInstance> rects;
        if (Debug::STRESS_TEST_AMOUNT > 0)
        {
            rects.resize(Debug::STRESS_TEST_AMOUNT);
            for (int i = 0; i < Debug::STRESS_TEST_AMOUNT; i++)
            {
                rects[i].pos   = vec2(rand() % windowWidth, rand() % windowHeight);
                rects[i].size  = vec2(32, 32);
                rects[i].color = vec4(1, 0, 0, 1);
            }
        }

        // add player rectangle
        rects.push_back({ player->pos, player->size, player->color });

        // draw all rectangles
        draw_rect_batched(rects);

        // draw overlay
        Debug::ShowOverlay(deltaTime);
        Debug::EndFrame();

        SDL_GL_SwapWindow(window);
    }

    shutdown();
    return 0;
}

// ------------------------------
// FUNCTIONS
// ------------------------------

void shutdown()
{
    Debug::Shutdown();

    if (ShaderProgram != 0) glDeleteProgram(ShaderProgram);
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
    if (EBO != 0) glDeleteBuffers(1, &EBO);
    if (instanceVBO != 0) glDeleteBuffers(1, &instanceVBO);
    if (player != nullptr) delete player;
    if (ctx) SDL_GL_DestroyContext(ctx);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

unsigned int compile_shader(unsigned int type, const char* source)
{
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        int length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        std::string infoLog(length, '\0');
        glGetShaderInfoLog(shader, length, &length, &infoLog[0]);

        std::cerr << "Shader compile error: " << infoLog << std::endl;
        return 0;
    }
    return shader;
}

void set_mat4(const std::string& name, const mat4& matrix)
{
    glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, name.c_str()), 1, GL_FALSE, &matrix[0][0]);
}
void set_vec2(const std::string& name, const vec2& vec)
{
    glUniform2fv(glGetUniformLocation(ShaderProgram, name.c_str()), 1, &vec[0]);
}
void set_vec4(const std::string& name, const vec4& vec)
{
    glUniform4fv(glGetUniformLocation(ShaderProgram, name.c_str()), 1, &vec[0]);
}

void draw_rect(Rect* rect, const void* indices)
{
    glUseProgram(ShaderProgram);
    set_mat4("uProjection", rect->projection);
    set_vec2("uOffset", rect->pos);
    set_vec2("uSize", rect->size);
    set_vec4("uColor", rect->color);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);
    glBindVertexArray(0);
}

void draw_rect_batched(const std::vector<RectInstance>& rects)
{
    glUseProgram(ShaderProgram);
    mat4 proj = ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f, -1.0f, 1.0f);
    set_mat4("uProjection", proj);

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, rects.size() * sizeof(RectInstance), rects.data());

    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, rects.size());
    glBindVertexArray(0);
}
