#pragma once
// Cube.h
#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class CubeBase {
protected:
    GLuint renderingProgram, vao, vbo;
    GLuint mvLoc, projLoc;
    glm::mat4 pMat, vMat, mMat, mvMat;
    float cameraX, cameraY, cameraZ;
   

public:
    CubeBase();
    virtual ~CubeBase();
    float cubeLocX, cubeLocY, cubeLocZ;

    virtual void init(GLFWwindow* window);
    virtual void setupVertices();
    virtual void display(GLFWwindow* window);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

protected:
    GLuint createShaderProgram(const char* vertFile, const char* fragFile);
};

