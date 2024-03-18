#version 410 core

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedo;
layout(location = 3) out vec4 gAmbientShininess;
layout(location = 4) out vec4 gTextureColor;

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

    float shininess;
    bool activeTextures[MAX_TEXTURES];
    sampler2D textures[MAX_TEXTURES];
};

struct Texture
{
    bool activeTextures[MAX_TEXTURES];
    bool isProcedural[MAX_TEXTURES];
    sampler2D textures[MAX_TEXTURES];
};

// PROCEDURAL TEXTURE

// Hash function
vec2 hash(vec2 p) {
    vec2 p2 = fract(vec2(p.xyx) * vec2(5.3983, 5.4427));
    p2 += dot(p2, p2.yx + 19.19);
    return fract(vec2((p2.x + p2.y) * p2.x, (p2.x + p2.y) * p2.y));
}
float noise(vec2 P) {
    vec2 Pi = floor(P);
    vec2 Pf = P - Pi;
    vec2 u = Pf * Pf * (3.0 - 2.0 * Pf);

    float a = dot(hash(Pi), Pf);
    float b = dot(hash(Pi + vec2(1.0, 0.0)), Pf - vec2(1.0, 0.0));
    float c = dot(hash(Pi + vec2(0.0, 1.0)), Pf - vec2(0.0, 1.0));
    float d = dot(hash(Pi + vec2(1.0, 1.0)), Pf - vec2(1.0, 1.0));

    return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}


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

    vec4 texColor = vec4(0.0, 0.0, 0.0, 0.0);
    // Iterate over each texture
    for (int i = 0; i < MAX_TEXTURES; i++)
    {
        if (fragTexture.activeTextures[i])
        {
            if(fragTexture.isProcedural[i]) {
                float n = noise(texCoord * 50);
                if (n < 0.1)
                    texColor = vec4(0.0, 0.0, 0.5, 1.0);
                else if (n < 0.2)
                    texColor = vec4(0.0, 0.2, 0.8, 1.0);
                else 
                    texColor = vec4(0.0, 0.5, 1.0, 1.0);
            } else {
                vec4 fragTexColor = texture2D(fragTexture.textures[i], texCoord);
                texColor = mix(texColor, fragTexColor, fragTexColor.a);
            }
        }
    }

    gTextureColor = texColor;
}
