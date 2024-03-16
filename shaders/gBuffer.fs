#version 410 core

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedo;
layout(location = 3) out vec4 gAmbientShininess;

// From vertex shader
in vec4 position;  // position of the vertex (and fragment) in eye space
in vec3 normal ;  // surface normal vector in eye space
in vec2 texCoord; // Texture coordinate
in mat3 TBN;

uniform mat4 m, v, p;
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

// The front surface material
uniform Material material;
uniform Texture fragTexture;

void main()
{
    vec3 fNormal = normalize(normal);
    // textures[0] represent the normal map! Other textures should not use this position!
    if(material.activeTextures[0]) {
        vec3 normalMap = texture2D(material.textures[0], texCoord).rgb;
        normalMap = normalMap * 2.0 - 1.0;
        fNormal = normalize(TBN * normalMap);
    }

    gNormal.xyz = fNormal;
    gPosition = position;

    vec4 diffuseColor = material.diffuse;
    if(material.activeTextures[1]) 
    {
        diffuseColor = texture2D(material.textures[1], texCoord);
    }

    gAlbedo.rgb = diffuseColor.rgb;

    vec4 specularColor = material.specular;
    if(material.activeTextures[2]) 
    {
        specularColor = texture2D(material.textures[2], texCoord);
    }

    gAlbedo.a = specularColor.r;

    gAmbientShininess.rgb = material.ambient.rgb;
    gAmbientShininess.a = material.shininess;

    // Iterate over other potentially set material textures.
    // for(int i = 5; i < MAX_TEXTURES; i++)
    // {
    //     if (material.activeTextures[i])
    //     {
    //         vec3 matTexColor = texture2D(material.textures[i], texCoord).rgb;
    //         totalLighting.rgb *= matTexColor;
    //     }
    // }

    // // Iterate over each texture
    // for (int i = 0; i < MAX_TEXTURES; i++)
    // {
    //     if (fragTexture.activeTextures[i])
    //     {
    //         vec4 textureColor = texture2D(fragTexture.textures[i], texCoord);
    //         totalLighting = mix(totalLighting, textureColor, textureColor.a);
    //     }
    // }
}
