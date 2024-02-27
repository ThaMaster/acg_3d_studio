#version 410 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertex_position;

// Values that stay constant for the whole mesh.
uniform mat4 lightMatrices[6];

// model, view and projection transform
uniform mat4 m;
uniform vec4 lightPos;

void main() 
{
    if(lightPos.w == 0.0) {
        gl_Position = lightMatrices[0] * m * vertex_position;
    } else {
        gl_Position = m * vertex_position;
    }
}