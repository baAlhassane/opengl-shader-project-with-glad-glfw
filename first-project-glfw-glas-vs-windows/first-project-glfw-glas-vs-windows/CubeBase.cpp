#include "CubeBase.h"
// Cube.cpp
#include "Cube.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

CubeBase::CubeBase() :
    renderingProgram(0), vao(0), vbo(0),
    cameraX(0.0f), cameraY(0.0f), cameraZ(8.0f),
    cubeLocX(0.0f), cubeLocY(-2.0f), cubeLocZ(0.0f) {
}

CubeBase::~CubeBase() {
    glDeleteProgram(renderingProgram);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

void CubeBase::init(GLFWwindow* window) {
    renderingProgram = createShaderProgram("../shaders/cubeBaseVertexShader.glsl", "../shaders/cubebaseFragmentShader.glsl");
    setupVertices();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}

void CubeBase::setupVertices() {
    float vertexPositions[108] = {
        -1.0f, 1.0f, -1.0f,  -1.0f,-1.0f,-1.0f,  1.0f,-1.0f,-1.0f,
         1.0f,-1.0f,-1.0f,   1.0f, 1.0f,-1.0f, -1.0f, 1.0f,-1.0f,
         1.0f,-1.0f,-1.0f,   1.0f,-1.0f, 1.0f,  1.0f, 1.0f,-1.0f,
         1.0f,-1.0f, 1.0f,   1.0f, 1.0f, 1.0f,  1.0f, 1.0f,-1.0f,
         1.0f,-1.0f, 1.0f,  -1.0f,-1.0f, 1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,  -1.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,  -1.0f,-1.0f,-1.0f, -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,  -1.0f, 1.0f,-1.0f, -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,   1.0f,-1.0f, 1.0f,  1.0f,-1.0f,-1.0f,
         1.0f,-1.0f,-1.0f,  -1.0f,-1.0f,-1.0f, -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,   1.0f, 1.0f,-1.0f,  1.0f, 1.0f, 1.0f,
         1.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 1.0f, -1.0f, 1.0f,-1.0f
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Définir la correspondance entre les attributs du shader et le buffer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
}

void CubeBase::display(GLFWwindow* window) {
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(renderingProgram);

    mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
    projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float aspect = (float)width / (float)height;

    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
    mMat = glm::translate(glm::mat4(1.0f), glm::vec3(cubeLocX, cubeLocY, cubeLocZ));
    mvMat = vMat * mMat;

    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_TRIANGLES, 0, 36);
    std::cout << " cubeBase ! " << std::endl;
}

GLuint CubeBase::createShaderProgram(const char* vertFile, const char* fragFile) {
    auto loadShader = [](const char* filename) -> std::string {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "❌ Impossible d'ouvrir : " << filename << std::endl;
            return "";
        }
        std::stringstream ss; ss << file.rdbuf();
        return ss.str();
        };

    std::string vertCode = loadShader(vertFile);
    std::string fragCode = loadShader(fragFile);

    const char* vSrc = vertCode.c_str();
    const char* fSrc = fragCode.c_str();

    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vSrc, nullptr);
    glCompileShader(vShader);

    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fSrc, nullptr);
    glCompileShader(fShader);

    GLuint program = glCreateProgram();
    glAttachShader(program, vShader);
    glAttachShader(program, fShader);
    glLinkProgram(program);

    glDeleteShader(vShader);
    glDeleteShader(fShader);
    return program;
}

void CubeBase::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
