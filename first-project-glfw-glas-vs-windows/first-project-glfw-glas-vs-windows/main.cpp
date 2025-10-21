#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "PyramidTexture.h"  // ta classe qui gère le cube texturé
#include <iostream>
#include <filesystem>
// Taille de la fenêtre
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

int main() {
    // Initialisation de GLFW
    std::cout << " Dossier courant : " << std::filesystem::current_path() << std::endl;

    if (!glfwInit()) {
        std::cerr << "Erreur : GLFW init failed" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Pyramid Texturée", nullptr, nullptr);
    if (!window) {
        std::cerr << "Erreur : création fenêtre échouée" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Erreur : GLAD init failed" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Callback pour redimensionnement
    glfwSetFramebufferSizeCallback(window, PyramidTexture::framebuffer_size_callback);

    // Création et initialisation de l'objet
    PyramidTexture* pyramid = new PyramidTexture();
    pyramid->init(window);  // charge shaders, textures, VAO/VBO etc.

    // Boucle principale
    while (!glfwWindowShouldClose(window)) {
        pyramid->display(window);  // dessine la pyramide texturée
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete pyramid;
    glfwTerminate();
    return 0;
}


