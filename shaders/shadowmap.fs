#version 410 core

// From vertex shader
in vec4 position;  // position of the vertex (and fragment) in eye space
in vec3 normal ;  // surface normal vector in eye space
in vec2 texCoord; // Texture coordinate

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
  vec4 position;
  vec4 diffuse;
  vec4 specular;
};

const int MaxNumberOfLights = 10;

// This is the uniforms that our program communicates with
uniform LightSource lights[MaxNumberOfLights];

// The front surface material
uniform Material material;
uniform Texture texture;

void main()
{
    vec3 normalDirection = normalize(normal);
    vec3 viewDirection = normalize(vec3(v_inv * vec4(0.0, 0.0, 0.0, 1.0) - position));
    vec3 lightDirection;
    float attenuation;

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
        * light.diffuse * material.diffuse
        * max(0.0, dot(normalDirection, lightDirection));

        vec4 specularReflection;
        if (dot(normalDirection, lightDirection) < 0.0) // light source on the wrong side?
        {
            specularReflection = vec4(0.0, 0.0, 0.0, 0.0); // no specular reflection
        }
        else // light source on the right side
        {
            specularReflection = attenuation * light.specular * material.specular
            * pow(max(0.0, dot(reflect(-lightDirection, normalDirection), viewDirection)), material.shininess);
        }

        totalLighting = totalLighting + diffuseReflection + specularReflection;
    }

    // Iterate over each texture
    for (int i = 0; i < MAX_TEXTURES; i++)
    {
        if (material.activeTextures[i])
        {
            vec4 matTexColor = texture2D(material.textures[i], texCoord);
            totalLighting *= matTexColor;
        }
        
        if (texture.activeTextures[i])
        {
            vec4 textureColor = texture2D(texture.textures[i], texCoord);
            totalLighting = mix(totalLighting, totalLighting*textureColor, textureColor.a);
        }
    }

    color = totalLighting;
}