#include "CubeRotation.h"


// CubeWithFaceColor.cpp
#include "CubeWithFaceColor.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

CubeRotation::CubeRotation() : CubeBase() {}
CubeRotation::~CubeRotation() {}

void CubeRotation::setupVertices() {
    float vertexData[] = {
        // Face avant (rouge)
        -1,1,-1, 1,0,0,  -1,-1,-1, 1,0,0,  1,-1,-1, 1,0,0,
         1,-1,-1, 1,0,0,  1,1,-1, 1,0,0,  -1,1,-1, 1,0,0,
         // Face droite (vert)
          1,-1,-1, 0,1,0,  1,-1,1, 0,1,0,  1,1,-1, 0,1,0,
          1,-1,1, 0,1,0,   1,1,1, 0,1,0,   1,1,-1, 0,1,0,
          // Face arrière (bleu)
           1,-1,1, 0,0,1,  -1,-1,1, 0,0,1,  1,1,1, 0,0,1,
          -1,-1,1, 0,0,1,  -1,1,1, 0,0,1,   1,1,1, 0,0,1,
          // Face gauche (jaune)
          -1,-1,1, 1,1,0,  -1,-1,-1, 1,1,0,  -1,1,1, 1,1,0,
          -1,-1,-1, 1,1,0, -1,1,-1, 1,1,0,   -1,1,1, 1,1,0,
          // Dessous (cyan)
          -1,-1,1, 0,1,1,  1,-1,1, 0,1,1,  -1,-1,-1, 0,1,1,
           1,-1,1, 0,1,1,   1,-1,-1, 0,1,1,  -1,-1,-1, 0,1,1,
           // Dessus (magenta)
           -1,1,-1, 1,0,1,  -1,1,1, 1,0,1,  1,1,-1, 1,0,1,
           -1,1,1, 1,0,1,   1,1,1, 1,0,1,    1,1,-1, 1,0,1
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // position (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // couleur (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
}
void CubeRotation::init(GLFWwindow* window) {
    renderingProgram = createShaderProgram("../shaders/vertexShader.glsl", "../shaders/fragmentShader.glsl");
    setupVertices();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}
void CubeRotation::display(GLFWwindow* window) {
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    glUseProgram(renderingProgram);

    mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
    projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float aspect = (float)width / (float)height;

    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
    float time = glfwGetTime();
    mMat = glm::rotate(glm::mat4(1.0f), time * glm::radians(50.0f), glm::vec3(1, 1, 0));
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5));
    mvMat = vMat * mMat;

    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, 36);
    std::cout << " cubeRotation time " <<time<< std::endl;
}








/*
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "CubeBase.h"
#include "CubeRotation.h"
#include <iostream>

// Taille de la fenêtre
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

// Pointeur générique sur le cube courant
CubeBase* cubeRotation = nullptr;
CubeBase* cube = nullptr;


int main() {
    // Initialisation GLFW
    if (!glfwInit()) {
        std::cerr << "Erreur : GLFW init failed" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Création de la fenêtre
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Cube Rotation extends Cubase ", nullptr, nullptr);
    if (!window) {
        std::cerr << "Erreur : création fenêtre échouée" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Chargement de GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Erreur : GLAD init failed" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Callback de redimensionnement
    glfwSetFramebufferSizeCallback(window, CubeBase::framebuffer_size_callback);

    // 🧱 Choisir quel cube utiliser :
    // cube = new Cube(); // cube sans couleur
    cubeRotation = new CubeRotation(); // cube avec couleurs par face
    cube = new CubeBase();
    cubeRotation->init(window);
    cube->init(window);

    // Positionner les cubes différemment
    cube->cubeLocX = -4.0f;  // cube statique à gauche
    cubeRotation->cubeLocX = 6.0f; // cube coloré à droite

    // Boucle principale
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        cube->display(window);
       cubeRotation->display(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete cube;
    delete cubeRotation;
    glfwTerminate();
    return 0;
}



*/

