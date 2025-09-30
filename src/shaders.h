#ifndef GAME_SHADERS_H
#define GAME_SHADERS_H

namespace Shaders
{
    // Vertex shader for instanced rectangles
    inline const char* RECT_VERTEX_SHADER = R"(
#version 430 core

layout(location = 0) in vec2 aPos;

layout(location = 1) in vec2 iOffset;
layout(location = 2) in vec2 iSize;
layout(location = 3) in vec4 iColor;

uniform mat4 uProjection;

out vec4 vColor;

void main()
{
    vec2 scaledPos = aPos * iSize + iOffset;
    gl_Position = uProjection * vec4(scaledPos, 0.0, 1.0);
    vColor = iColor;
}
)";

    inline const char* RECT_FRAGMENT_SHADER = R"(
#version 430 core

in vec4 vColor;
out vec4 FragColor;

void main()
{
    FragColor = vColor;
}
)";
}

#endif //GAME_SHADERS_H