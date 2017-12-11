#pragma once

#include <GL/glew.h>

// Shader sources
const GLchar* vertexSource = R"glsl(
    #version 150 core

    in vec2 position;
    in vec3 color;
    in vec2 texcoord;

    out vec3 Color;
    out vec2 Texcoord;

    void main()
    {
        Color = color;
        Texcoord = texcoord;
        gl_Position = vec4(position, 0.0, 1.0);
    }
)glsl";

const GLchar* fragmentSource = R"glsl(
    #version 150 core

    in vec3 Color;
    in vec2 Texcoord;

    out vec4 outColor;

    uniform sampler2D texKitten;

    void main()
    {
        outColor = texture(texKitten, Texcoord);
    }
)glsl";