#version 410 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertex_position;

// Values that stay constant for the whole mesh.
uniform mat4 lightMatrix;

// model, view and projection transform
uniform mat4 m;

void main() {
    gl_Position =  lightMatrix * m * vertex_position;
}