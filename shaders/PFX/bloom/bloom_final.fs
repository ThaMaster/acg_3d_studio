#version 410 core

layout (location = 0) out vec4 color;

in vec2 texCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;

uniform bool bloom;

void main()
{
    vec3 sceneColor = texture(scene, texCoords).rgb;      
    vec3 bloomColor = texture(bloomBlur, texCoords).rgb;
    if(bloom)
        sceneColor += bloomColor; // additive blending

    color = vec4(sceneColor, 1.0);
}