#version 330 core

in vec2 pos;
in vec2 texc;

out vec2 vTexc;

void main()
{
    gl_Position = vec4(pos, 0.0, 1.0);
    vTexc = texc;
}
