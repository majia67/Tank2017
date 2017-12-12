#version 330 core

in vec2 Texcoord;
out vec4 outColor;
uniform sampler2D texMap;

void main()
{
    outColor = texture(texMap, Texcoord);
}
