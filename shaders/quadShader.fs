#version 410 core

out vec4 FragColor;

in vec2 texCoord;

layout(location = 0) out vec4 color;

const int MaxNumberOfLights = 10;
uniform sampler2D shadowMaps[MaxNumberOfLights];

void main()
{
    float depth = texture(shadowMaps[0], texCoord).r;
    color = vec4(vec3(depth), 1.0);
}