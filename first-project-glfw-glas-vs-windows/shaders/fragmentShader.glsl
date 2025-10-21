#version 330 core

in vec3 vertexColor;      // venant du vertex shader
out vec4 fragColor;       // couleur finale du pixel

void main(void)
{
    fragColor = vec4(vertexColor, 1.0); // on ajoute le canal alpha
}
