#version 330 core

layout(location = 0) in vec3 position; // attribut position
layout(location = 1) in vec3 color;    // attribut couleur

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

out vec3 vertexColor; // variable transmise au fragment shader

void main(void)
{
    gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
    vertexColor = color; // on envoie la couleur au fragment shader
}
