#include "Point.h"
#include <iostream>


Point::Point() : renderingProgram(0) {
    vao[0] = 0;
}

Point::~Point() {
    // Nettoyage des ressources GPU
    glDeleteProgram(renderingProgram);
    glDeleteVertexArrays(1, vao);
}

GLuint Point::createShaderProgram() {
    const char* vshaderSource =
        "#version 430 \n"
        "void main(void) \n"
        "{ gl_Position = vec4(0.0, 0.0, 0.0, 1.0); }";

    const char* fshaderSource =
        "#version 430 \n"
        "out vec4 color; \n"
        "void main(void) \n"
        "{ color = vec4(0.0, 0.0, 1.0, 1.0); }";

    // Création et compilation des shaders
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vShader, 1, &vshaderSource, nullptr);
    glShaderSource(fShader, 1, &fshaderSource, nullptr);
    glCompileShader(vShader);
    glCompileShader(fShader);

    // Création du programme
    GLuint vfProgram = glCreateProgram();
    glAttachShader(vfProgram, vShader);
    glAttachShader(vfProgram, fShader);
    glLinkProgram(vfProgram);

    // Nettoyage des shaders individuels (non nécessaires après linkage)
    glDeleteShader(vShader);
    glDeleteShader(fShader);

    return vfProgram;
}

void Point::init(GLFWwindow* window) {
    renderingProgram = createShaderProgram();

    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);
}

void Point::display(GLFWwindow* window, double currentTime) {
    glUseProgram(renderingProgram);
    glPointSize(30.0f);
    glDrawArrays(GL_POINTS, 0, 1);
}


/*
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Point.h"

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(600, 600, "Program 2.2 - Point", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // ?? Initialiser GLAD ici
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Erreur : GLAD non chargé !" << std::endl;
        return -1;
    }

    Point point;
    point.init(window);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        double currentTime = glfwGetTime();
        point.display(window, currentTime);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

*/
