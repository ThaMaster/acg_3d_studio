#version 410 core

// From vertex shader
in vec4 position;  // position of the vertex (and fragment) in eye space
in vec3 normal ;  // surface normal vector in eye space
in vec2 texCoord; // Texture coordinate
in mat3 TBN;

// The end result of this shader
out vec4 color;

uniform mat4 m, v, p;
uniform mat4 v_inv;
uniform int numberOfLights;

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
};

const int MaxNumberOfLights = 10;

// This is the uniforms that our program communicates with
uniform LightSource lights[MaxNumberOfLights];

// The front surface material
uniform Material material;
uniform Texture texture;

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
        LightSource light = lights[index];
        if (0.0 == light.position.w) // directional light?
        {
            attenuation = 1.0; // no attenuation
            lightDirection = normalize(vec3(light.position));
        }
        else // point light or spotlight (or other kind of light) 
        {
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
            totalLighting = totalLighting + diffuseReflection + specularReflection;
        } else {
            totalLighting = totalLighting + diffuseReflection;
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
        if (texture.activeTextures[i])
        {
            vec4 textureColor = texture2D(texture.textures[i], texCoord);
            totalLighting = mix(totalLighting, textureColor, textureColor.a);
        }
    }
    color = vec4(totalLighting.rgb, totalLighting.a * material.opacity);
}
