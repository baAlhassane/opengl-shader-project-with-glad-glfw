#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
class Cube {
public:
    Cube();
    ~Cube();

    void init(GLFWwindow* window);          // initialise shaders, vertices
    void display(GLFWwindow* window);   


private:
    void setupVertices();                   // configure VAO/VBO
    GLuint createShaderProgram(const char* vertFile, const char* fragFile);

    GLuint renderingProgram;
    GLuint vao;
    GLuint vbo;

    // matrices et uniforms
    GLuint mvLoc, projLoc;
    glm::mat4 pMat, vMat, mMat, mvMat;

    // position caméra et cube
    float cameraX, cameraY, cameraZ;
    float cubeLocX, cubeLocY, cubeLocZ;
};
