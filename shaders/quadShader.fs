#version 410 core

in vec2 texCoord;

layout(location = 0) out vec4 color;

uniform sampler2D gPosition;

uniform sampler2D quadTexture;
uniform int numTexVals;
uniform vec4 valPos;

uniform bool useDepthCubeMap;
uniform samplerCube quadCubeMap;
uniform vec4 lPosition;

vec4 visualizeDepthCubeMap(vec4 fragSpacePos)
{
    vec3 fragToLight = vec3(fragSpacePos - lPosition);
    float closestDepth = texture(quadCubeMap, fragToLight).r;    
    return vec4(vec3(closestDepth), 1.0);
}

void main()
{
    if(useDepthCubeMap) {
        vec4 fragPosition = texture(gPosition, texCoord);
        color = visualizeDepthCubeMap(fragPosition);
    } else {
        if(numTexVals == 4) {
            color = texture(quadTexture, texCoord);
        } else if(numTexVals == 3)  {
            color = vec4(texture(quadTexture, texCoord).rgb, 1.0);
        } else {
            if(valPos.r == 1) {
                float singleColor = texture(quadTexture, texCoord).r;
                color = vec4(vec3(singleColor), 1.0);
            } else if(valPos.g == 1) {
                float singleColor = texture(quadTexture, texCoord).g;
                color = vec4(vec3(singleColor), 1.0);
            } else if(valPos.b == 1) {
                float singleColor = texture(quadTexture, texCoord).b;
                color = vec4(vec3(singleColor), 1.0);
            } else if(valPos.a == 1) {
                float singleColor = texture(quadTexture, texCoord).a;
                color = vec4(vec3(singleColor), 1.0);
            }
        }
    }
    
}