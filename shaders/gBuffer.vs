#version 410 core

layout(location = 0) in vec4 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_texCoord;
layout(location = 3) in vec3 vertex_tangent;

out vec4 position;  // position of the vertex (and fragment) in world space
out vec3 normal;  // surface normal vector in world space
out vec2 texCoord; 
out mat3 TBN; // The TBN matrix;

// model, view and projection transform
uniform mat4 m, v, p;

// Inverse transpose of model matrix for transforming normals
uniform mat3 m_3x3_inv_transp;

void main()
{
  mat4 mv = v * m;
  texCoord = vertex_texCoord;

  position = mv * vertex_position;

  normal = normalize(m_3x3_inv_transp * vertex_normal);

  vec3 T = normalize(vec3(m * vec4(vertex_tangent, 0.0)));
  vec3 N = normalize(vec3(m * vec4(vertex_normal, 0.0)));
  // Re-orthogonalize with the Gram-Schmidt process
  T = normalize(T - dot(T,N) * N);
  vec3 B = cross(N, T);

  TBN = mat3(T, B, N);

  gl_Position = p * position;


}
