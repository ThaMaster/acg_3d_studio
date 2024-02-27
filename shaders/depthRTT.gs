#version 410 core

layout(triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 lightMatrices[6];
uniform vec4 lightPos;

out vec4 FragPos;

void main()
{
    if(lightPos.w == 0.0) {

        for(int i = 0; i < gl_in.length(); i++) {
            gl_Position = gl_in[i].gl_Position;
            EmitVertex(); // Emit the vertex
        }
        EndPrimitive();

    } else {
        for(int face = 0; face < 6; ++face)
        {
            gl_Layer = face; // built-in variable that specifies to which face we render.
            for(int i = 0; i < 3; ++i) // for each triangle vertex
            {
                FragPos = gl_in[i].gl_Position;
                gl_Position = lightMatrices[face] * FragPos;
                EmitVertex();
            }    
            EndPrimitive();
        }
    }
} 