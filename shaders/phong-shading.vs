#version 410 core

layout(location = 0) in vec4 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_texCoord;


out vec4 position;  // position of the vertex (and fragment) in world space
out vec3 normal;  // surface normal vector in world space
out vec2 texCoord; 

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

  gl_Position = p * position;
}
