#pragma once
#include <glad/glad.h>
#include <string>
class Triangle
{
public:
    Triangle();              // constructeur (init VBO/VAO/shader)
    ~Triangle();             // destructeur (libère ressources)
    void draw();             // affiche le triangle

private:
    unsigned int VAO, VBO;
    unsigned int shaderProgram;

    unsigned int compileShader(unsigned int type, const char* source);
    unsigned int createShaderProgram(const char* vertexSrc, const char* fragmentSrc);
};

