#version 410 core

in vec4 FragPos;

uniform vec4 lightPos;
uniform float far_plane;

void main()
{
    if(lightPos.w == 0.0) {
        gl_FragDepth = gl_FragCoord.z;
    } else {
        float lightDistance = length(FragPos.xyz - lightPos.xyz);
        // map to [0;1] range by dividing by far_plane
        lightDistance = lightDistance / far_plane;
        // write this as modified depth
        gl_FragDepth = lightDistance;
    }
}