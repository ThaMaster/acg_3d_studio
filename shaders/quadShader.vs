#version 410 core

layout(location = 0) in vec4 vertex_position;
layout(location = 1) in vec2 vertex_texCoord;

out vec2 texCoord;

uniform mat4 transform;

void main()
{
    texCoord = vertex_texCoord;
    gl_Position = transform * vertex_position;
}