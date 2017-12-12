#pragma once

#include <GL/glew.h>

// Shader sources
const GLchar* vertexSource = R"glsl(
    #version 150 core

    in vec2 position;
    in vec2 texcoord;

    out vec2 Texcoord;

    void main()
    {
        Texcoord = texcoord;
        gl_Position = vec4(position, 0.0, 1.0);
    }
)glsl";

const GLchar* fragmentSource = R"glsl(
    #version 150 core

    in vec2 Texcoord;

    out vec4 outColor;

    uniform sampler2D texMap;

    void main()
    {
        outColor = texture(texMap, Texcoord);
    }
)glsl";
