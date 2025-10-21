#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

class PyramidTexture {
public:
    PyramidTexture();
    ~PyramidTexture();

    void init(GLFWwindow* window);
    void display(GLFWwindow* window);

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

private:
    GLuint renderingProgram;
    GLuint vao;
    GLuint vbo[2];
    GLuint brickTexture;

    float cameraX, cameraY, cameraZ;
    float objLocX, objLocY, objLocZ;

    glm::mat4 pMat, vMat, mMat, mvMat;
    GLint mvLoc, projLoc;

    GLuint createShaderProgram(const char* vertFile, const char* fragFile);
    GLuint loadTexture(const std::string& filename);
    void setupVertices();
};




