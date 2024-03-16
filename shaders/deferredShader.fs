#version 410 core

const int MaxNumberOfLights = 10;

in vec2 texCoord;
in vec4 position;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

// The end result of this shader
layout(location = 0) out vec4 color;

uniform mat4 m, v, p;
uniform mat4 v_inv;
uniform int numberOfLights;
uniform vec3 viewPos;
//uniform float far_plane;

// Definition of a light source structure
struct LightSource
{
  bool enabled;

  float constant;
  float linear;
  float quadratic;

  vec4 ambient;
  vec4 diffuse;
  vec4 specular;

  vec4 position;

  mat4 lightMatrices[6];
};

// This is the uniforms that our program communicates with
uniform LightSource lights[MaxNumberOfLights];

//uniform bool useShadowMap;
//uniform sampler2D shadowMaps[MaxNumberOfLights];
//uniform samplerCube shadowCubeMaps[MaxNumberOfLights];

// float directionalShadow(int lightIndex)
// {
//     // perform perspective divide
//     vec3 projCoords = fragSpacePos[lightIndex].xyz / fragSpacePos[lightIndex].w;
//     // transform to [0,1] range
//     projCoords = projCoords * 0.5 + 0.5;

//     // get depth of current fragment from light's perspective
//     float currentDepth = projCoords.z;

//     vec3 lightDir = normalize(vec3(lights[lightIndex].position));

//     float bias = max(0.001 * (1.0 - dot(normal, lightDir)), 0.0001);
//     //float bias = 0.05;
//     // check whether current frag pos is in shadow
//     float shadow = 0.0;
//     vec2 texelSize = 1.0 / textureSize(shadowMaps[lightIndex], 0);
//     const int halfkernelWidth = 2;
//     for(int x = -halfkernelWidth; x <= halfkernelWidth; ++x)
//     {
//         for(int y = -halfkernelWidth; y <= halfkernelWidth; ++y)
//         {
//             float pcfDepth = texture(shadowMaps[lightIndex], projCoords.xy + vec2(x, y) * texelSize).r;
//             shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
//         }
//     }
//     shadow /= ((halfkernelWidth*2+1)*(halfkernelWidth*2+1));
    
//     // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
//     if(projCoords.z > 1.0)
//         shadow = 0.0;

//     return shadow;
// }

// float pointShadow(int lightIndex)
// {
//     vec3 fragToLight = vec3(fragSpacePos[lightIndex].xyz - lights[lightIndex].position.xyz);
    
//     float currentDepth = length(fragToLight);

//     float shadow  = 0.0;
//     float bias    = 0.05; 
//     float samples = 4.0;
//     float offset  = 0.1;
//     for(float x = -offset; x < offset; x += offset / (samples * 0.5))
//     {
//         for(float y = -offset; y < offset; y += offset / (samples * 0.5))
//         {
//             for(float z = -offset; z < offset; z += offset / (samples * 0.5))
//             {
//                 float closestDepth = texture(shadowCubeMaps[lightIndex], fragToLight + vec3(x, y, z)).r; 
//                 closestDepth *= far_plane;   // undo mapping [0;1]
//                 if(currentDepth - bias > closestDepth)
//                     shadow += 1.0;
//             }
//         }
//     }
//     shadow /= (samples * samples * samples);
//     return shadow;
// }

// vec4 visualizeDepthCubeMap(int lightIndex)
// {
//     vec3 fragToLight = vec3(fragSpacePos[lightIndex] - lights[lightIndex].position);
    
//     float closestDepth = texture(shadowCubeMaps[lightIndex], fragToLight).r;
//     closestDepth *= far_plane;
    
//     float currentDepth = length(fragToLight);

//     float bias = 0.005;
//     float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0; 
    
//     return vec4(vec3(closestDepth / far_plane), 1.0);
// }

void main()
{
    vec4 position = texture(gPosition, texCoord);
    vec3 normalDirection = texture(gNormal, texCoord).rgb;
    vec4 diffuseColor = vec4(texture(gAlbedoSpec, texCoord).rgb, 1.0);
    float specularColor = texture(gAlbedoSpec, texCoord).a;
    
    vec3 viewDirection = normalize(viewPos - position.xyz);
    vec3 lightDirection;
    float attenuation;

    // initialize total lighting with ambient lighting
    vec4 totalLighting = lights[0].ambient;

    // for all light sources
    for (int index = 0; index < numberOfLights; index++) 
    {
        float shadow = 0.0;
        LightSource light = lights[index];
        if (0.0 == light.position.w) // directional light?
        {
            // if(useShadowMap) {
            //     shadow = directionalShadow(index);
            // }   
            attenuation = 1.0; // no attenuation
            lightDirection = normalize(vec3(light.position));
        }
        else // point light or spotlight (or other kind of light) 
        {
            // if(useShadowMap) {
            //     shadow = pointShadow(index);
            // }
            vec3 positionToLightSource = vec3(light.position.xyz - position.xyz);
            float distance = length(positionToLightSource);
            lightDirection = normalize(positionToLightSource);
            attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
        }

        vec4 diffuseReflection = attenuation
        * light.diffuse * diffuseColor
        * max(0.0, dot(normalDirection, lightDirection));

        vec4 specularReflection;
        if (dot(normalDirection, lightDirection) < 0.0) // light source on the wrong side?
        {
            specularReflection = vec4(0.0, 0.0, 0.0, 0.0); // no specular reflection
        }
        else // light source on the right side
        {
            specularReflection = attenuation * light.specular * specularColor
            * pow(max(0.0, dot(reflect(-lightDirection, normalDirection), viewDirection)), 10);
            // * pow(max(0.0, dot(reflect(-lightDirection, normalDirection), viewDirection)), material.shininess);
        }

        totalLighting += diffuseReflection + specularReflection;

        // if(material.shininess != 0) {
        //     totalLighting = totalLighting + (1.0 - shadow) * (diffuseReflection + specularReflection);
        // } else {
        //     totalLighting = totalLighting + (1.0 - shadow) * diffuseReflection;
        // }
    }

    color = totalLighting;
}
