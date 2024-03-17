#version 410 core

layout (location = 0) out vec4 color;

in vec2 texCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;

uniform bool bloom;
uniform float exposure;

void main()
{
    const float gamma = 2.2;
    vec3 sceneColor = texture(scene, texCoords).rgb;      
    vec3 bloomColor = texture(bloomBlur, texCoords).rgb;
    if(bloom)
        sceneColor += bloomColor; // additive blending
    // // tone mapping
    vec3 result = vec3(1.0) - exp(-sceneColor * exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));
    color = vec4(result, 1.0);
}