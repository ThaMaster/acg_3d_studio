#version 410 core

in vec4 FragPos;

uniform vec4 lightPos;
uniform float far_plane;

void main()
{
    if(lightPos.w == 0.0) {
        gl_FragDepth = gl_FragCoord.z;
    } else {
        gl_FragDepth = length(FragPos.xyz - lightPos.xyz) / far_plane;
    }
}