#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

out vec4 varyingColor;

void main(void)
{
    gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);

    // Génère une couleur pseudo-dépendante de la position du sommet
    varyingColor = vec4(position * 0.5 + 0.5, 1.0);
}
