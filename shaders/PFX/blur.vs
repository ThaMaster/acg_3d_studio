#version 410 core

layout (location = 0) in vec4 vertex_position;
layout (location = 2) in vec2 vertex_texCoord;

out vec2 texCoords;

void main()
{
    texCoords = vertex_texCoord;
    gl_Position = vertex_position;
}
