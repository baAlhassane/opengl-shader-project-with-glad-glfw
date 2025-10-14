#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
class Point
{
public:
    Point();                                  // constructeur
    ~Point();                                 // destructeur

    void init(GLFWwindow* window);            // initialisation OpenGL
    void display(GLFWwindow* window, double currentTime); // rendu

private:
    GLuint createShaderProgram();             // création du shader
    GLuint renderingProgram;                  // programme shader
    GLuint vao[1];
};

