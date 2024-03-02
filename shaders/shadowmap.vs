#version 410 core

layout(location = 0) in vec4 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_texCoord;
layout(location = 3) in vec3 vertex_tangent;
layout(location = 4) in vec3 vertex_bitangent;

const int MaxNumberOfLights = 10;

out vec4 fragSpacePos[MaxNumberOfLights];
out vec4 position;  // position of the vertex (and fragment) in world space
out vec3 normal;  // surface normal vector in world space
out vec2 texCoord; 
out mat3 TBN; // The TBN matrix;

// model, view and projection transform
uniform mat4 m, v, p;

// Inverse transpose of model matrix for transforming normals
uniform mat3 m_3x3_inv_transp;

uniform bool useShadowMap;

uniform int numberOfLights;
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

void main()
{
  mat4 mv = v * m;
  texCoord = vertex_texCoord;

  position = mv * vertex_position;
  
  if(useShadowMap) {
    for(int i = 0; i < numberOfLights; i++) {
      if(lights[i].position.w == 0.0) {
        fragSpacePos[i] = lights[i].lightMatrices[0] * m * vertex_position;
      } else {
        fragSpacePos[i] = m * vertex_position;
      }
    }
  }

  normal = normalize(m_3x3_inv_transp * vertex_normal);

  vec3 T = normalize(vec3(m * vec4(vertex_tangent, 0.0)));
  vec3 N = normalize(vec3(m * vec4(vertex_normal, 0.0)));
  // Re-orthogonalize with the Gram-Schmidt process
  T = normalize(T - dot(T,N) * N);
  vec3 B = cross(N, T);

  TBN = mat3(T, B, N);

  gl_Position = p * position;


}
