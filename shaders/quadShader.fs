#version 410 core

in vec2 texCoord;

layout(location = 0) out vec4 color;

uniform sampler2D quadTexture;

void main()
{
    color = vec4(texture(quadTexture, texCoord).rgb, 1.0);
}