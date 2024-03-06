#version 410 core

in vec2 texCoord;

layout(location = 0) out vec4 color;

uniform sampler2D quadTexture;
uniform int numTexVals;
void main()
{
    if(numTexVals == 4) {
        color = texture(quadTexture, texCoord);
    } else if(numTexVals == 3)  {
        color = vec4(texture(quadTexture, texCoord).rgb, 1.0);
    } else {
        float singleColor = texture(quadTexture, texCoord).a;
        color = vec4(singleColor, singleColor, singleColor, 1.0);
    }
    
}