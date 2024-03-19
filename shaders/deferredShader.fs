#version 410 core

const int MaxNumberOfLights = 10;

in vec2 texCoords;
in vec4 position;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gAmbientShininess;
uniform sampler2D gTextureColor;

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 bloomColor;
layout (location = 2) out vec4 cleanColor;

uniform mat4 m, v, p;
uniform int numberOfLights;
uniform vec3 viewPos;
uniform float far_plane;

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

uniform bool useShadowMap;
uniform sampler2D shadowMaps[MaxNumberOfLights];
uniform samplerCube shadowCubeMaps[MaxNumberOfLights];

float directionalShadow(int lightIndex, vec4 fragSpacePos, vec3 normal)
{
    // perform perspective divide
    vec3 projCoords = fragSpacePos.xyz / fragSpacePos.w;
    
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    vec3 lightDir = normalize(vec3(lights[lightIndex].position));

    float bias = max(0.001 * (1.0 - dot(normal, lightDir)), 0.0001);

    // check whether current frag pos is in shadow
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMaps[lightIndex], 0);
    const int halfkernelWidth = 2;
    for(int x = -halfkernelWidth; x <= halfkernelWidth; ++x)
    {
        for(int y = -halfkernelWidth; y <= halfkernelWidth; ++y)
        {
            float pcfDepth = texture(shadowMaps[lightIndex], projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= ((halfkernelWidth*2+1)*(halfkernelWidth*2+1));
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

float pointShadow(int lightIndex, vec4 fragSpacePos)
{
    vec3 fragToLight = vec3(fragSpacePos.xyz - lights[lightIndex].position.xyz);
    
    float currentDepth = length(fragToLight);

    float shadow  = 0.0;
    float bias    = 0.05; 
    float samples = 4.0;
    float offset  = 0.1;
    for(float x = -offset; x < offset; x += offset / (samples * 0.5))
    {
        for(float y = -offset; y < offset; y += offset / (samples * 0.5))
        {
            for(float z = -offset; z < offset; z += offset / (samples * 0.5))
            {
                float closestDepth = texture(shadowCubeMaps[lightIndex], fragToLight + vec3(x, y, z)).r; 
                closestDepth *= far_plane;   // undo mapping [0;1]
                if(currentDepth - bias > closestDepth)
                    shadow += 1.0;
            }
        }
    }
    shadow /= (samples * samples * samples);
    return shadow;
}

void main()
{
    // Get all the attributes from the geometry pass.
    vec4 position = texture(gPosition, texCoords);
    vec3 normalDirection = texture(gNormal, texCoords).rgb;
    vec4 diffuseColor = vec4(texture(gAlbedoSpec, texCoords).rgb, 1.0);
    float specularColor = texture(gAlbedoSpec, texCoords).a;
    vec4 matAmbient = vec4(texture(gAmbientShininess, texCoords).rgb, 1.0);
    float shininess = texture(gAmbientShininess, texCoords).a;
    vec4 texColor = texture2D(gTextureColor, texCoords);
    
    vec3 viewDirection = normalize(viewPos - position.xyz);
    vec3 lightDirection;
    float attenuation;

    // initialize total lighting with ambient lighting
    vec4 totalLighting = (lights[0].ambient * matAmbient);

    // for all light sources
    for (int index = 0; index < numberOfLights; index++) 
    {
        float shadow = 0.0;
        LightSource light = lights[index];
        if(light.enabled) {
            if (0.0 == light.position.w) // directional light?
            {
                if(useShadowMap) {
                    vec4 fragSpacePos = light.lightMatrices[0] * position;
                    shadow = directionalShadow(index, fragSpacePos, normalDirection);
                }   
                attenuation = 1.0; // no attenuation
                lightDirection = normalize(light.position.xyz);
            }
            else // point light or spotlight (or other kind of light) 
            {
                if(useShadowMap) {
                    shadow = pointShadow(index, position);
                }
                vec3 positionToLightSource = vec3(light.position.xyz - position.xyz);
                float distance = length(positionToLightSource);
                lightDirection = normalize(positionToLightSource);
                attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
            }

            vec4 diffuseReflection = attenuation
            * light.diffuse * diffuseColor
            * max(0.0, dot(normalDirection, lightDirection));

            vec4 specularReflection;
            if (dot(normalDirection, lightDirection) < 0.0 || shininess == 0) // light source on the wrong side?
            {
                specularReflection = vec4(0.0, 0.0, 0.0, 0.0); // no specular reflection
            }
            else // light source on the right side
            {
                specularReflection = attenuation * light.specular * specularColor
                * pow(max(0.0, dot(reflect(-lightDirection, normalDirection), viewDirection)), shininess);
            }
            
            totalLighting += (1.0 - shadow) * (diffuseReflection + specularReflection);
        }
    }
    
    if(texColor != vec4(0.0)) {
        // Add additional texture colors if they are stated.
        totalLighting *= texColor;
    }

    // Exctract the fragments that exceed a certain brightness.
    float brightness = dot(vec3(totalLighting), vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0) {
        bloomColor = totalLighting;
    } else {
        bloomColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    color = totalLighting;

    // Exctract a copy of the scene for the depth of field effect.
    cleanColor = totalLighting;
}
