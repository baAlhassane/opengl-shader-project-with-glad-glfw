#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Triangle.h"
#include <iostream>


int main() {
    // init GLFW
    if (!glfwInit()) {
        std::cerr << "Erreur init GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Triangle", NULL, NULL);
    if (!window) {
        std::cerr << "Erreur création fenêtre\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Erreur init GLAD\n";
        return -1;
    }


    {   // <- début du bloc
        Triangle triangle;

        while (!glfwWindowShouldClose(window)) {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            triangle.draw();

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }   // <- triangle est détruit ici, AVANT glfwTerminate()

    glfwTerminate();
    return 0;
}
