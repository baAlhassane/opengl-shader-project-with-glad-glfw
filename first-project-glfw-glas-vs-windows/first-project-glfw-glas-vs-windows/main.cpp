#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Cube.h"
#include <filesystem>

int main() {
    if (!glfwInit()) return -1;
    std::cout << " Dossier courant : " << std::filesystem::current_path() << std::endl;

    GLFWwindow* window = glfwCreateWindow(800, 600, "Cube OpenGL", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Erreur : GLAD non chargé !" << std::endl;
        return -1;
    }

    Cube cube;
    cube.init(window);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // gris foncé
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        cube.display(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
