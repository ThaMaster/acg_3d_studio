#version 410 core

const int MaxNumberOfLights = 10;
// From vertex shader
in vec4 position;  // position of the vertex (and fragment) in eye space
in vec3 normal ;  // surface normal vector in eye space
in vec2 texCoord; // Texture coordinate
in mat3 TBN;
in vec4 fragSpacePos[MaxNumberOfLights];

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

// The end result of this shader
layout(location = 0) out vec4 color;

uniform mat4 m, v, p;
uniform mat4 v_inv;
uniform int numberOfLights;
uniform float far_plane;
const int MAX_TEXTURES=10;

// declaration of a Material structure
struct Material
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    float opacity;
    float shininess;
    bool activeTextures[MAX_TEXTURES];
    sampler2D textures[MAX_TEXTURES];
};

struct Texture
{
    bool activeTextures[MAX_TEXTURES];
    sampler2D textures[MAX_TEXTURES];
};

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

// The front surface material
uniform Material material;
uniform Texture fragTexture;

uniform bool useShadowMap;
uniform sampler2D shadowMaps[MaxNumberOfLights];
uniform samplerCube shadowCubeMaps[MaxNumberOfLights];

float directionalShadow(int lightIndex)
{
    // perform perspective divide
    vec3 projCoords = fragSpacePos[lightIndex].xyz / fragSpacePos[lightIndex].w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    vec3 lightDir = normalize(vec3(lights[lightIndex].position));

    float bias = max(0.001 * (1.0 - dot(normal, lightDir)), 0.0001);
    //float bias = 0.05;
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

float pointShadow(int lightIndex)
{
    vec3 fragToLight = vec3(fragSpacePos[lightIndex].xyz - lights[lightIndex].position.xyz);
    
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

vec4 visualizeDepthCubeMap(int lightIndex)
{
    vec3 fragToLight = vec3(fragSpacePos[lightIndex] - lights[lightIndex].position);
    
    float closestDepth = texture(shadowCubeMaps[lightIndex], fragToLight).r;
    closestDepth *= far_plane;
    
    float currentDepth = length(fragToLight);

    float bias = 0.005;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0; 
    
    return vec4(vec3(closestDepth / far_plane), 1.0);
}


void main()
{
    vec3 fNormal = normalize(normal);
    // textures[0] represent the normal map! Other textures should not use this position!
    if(material.activeTextures[0]) {
        vec3 normalMap = texture2D(material.textures[0], texCoord).rgb;
        normalMap = normalMap * 2.0 - 1.0;
        fNormal = normalize(TBN * normalMap);
    }
    
    vec3 normalDirection = fNormal;
    vec3 viewDirection = normalize(vec3(v_inv * vec4(0.0, 0.0, 0.0, 1.0) - position));
    vec3 lightDirection;
    float attenuation;

    vec4 diffuseColor = material.diffuse;
    if(material.activeTextures[1]) 
    {
        diffuseColor = texture2D(material.textures[1], texCoord);
    }

    vec4 specularColor = material.specular;
    if(material.activeTextures[2]) 
    {
        specularColor = texture2D(material.textures[2], texCoord);
    }

    // initialize total lighting with ambient lighting
    vec4 totalLighting = lights[0].ambient * material.ambient;

    // for all light sources
    for (int index = 0; index < numberOfLights; index++) 
    {
        float shadow = 0.0;
        LightSource light = lights[index];
        if (0.0 == light.position.w) // directional light?
        {
            if(useShadowMap) {
                shadow = directionalShadow(index);
            }   
            attenuation = 1.0; // no attenuation
            lightDirection = normalize(vec3(light.position));
        }
        else // point light or spotlight (or other kind of light) 
        {
            if(useShadowMap) {
                shadow = pointShadow(index);
            }
            vec4 positionWorld = v_inv * position;
            vec3 positionToLightSource = vec3(light.position.xyz - positionWorld.xyz);
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
            * pow(max(0.0, dot(reflect(-lightDirection, normalDirection), viewDirection)), material.shininess);
        }

        if(material.shininess != 0) {
            totalLighting = totalLighting + (1.0 - shadow) * (diffuseReflection + specularReflection);
        } else {
            totalLighting = totalLighting + (1.0 - shadow) * diffuseReflection;
        }
    }
    // Iterate over other potentially set material textures.
    for(int i = 5; i < MAX_TEXTURES; i++)
    {
        if (material.activeTextures[i])
        {
            vec3 matTexColor = texture2D(material.textures[i], texCoord).rgb;
            totalLighting.rgb *= matTexColor;
        }
    }
    // Iterate over each texture
    for (int i = 0; i < MAX_TEXTURES; i++)
    {
        if (fragTexture.activeTextures[i])
        {
            vec4 textureColor = texture2D(fragTexture.textures[i], texCoord);
            totalLighting = mix(totalLighting, textureColor, textureColor.a);
        }
    }

    color = vec4(totalLighting.rgb, totalLighting.a * material.opacity);
}
