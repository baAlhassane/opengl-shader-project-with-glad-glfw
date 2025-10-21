#include "PyramidTexture.h"


#include "PyramidTexture.h"
#include <SOIL2/soil2.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

PyramidTexture::PyramidTexture()
    : renderingProgram(0), vao(0), brickTexture(0),
    cameraX(0.0f), cameraY(1.5f), cameraZ(5.0f),
    objLocX(0.0f), objLocY(0.0f), objLocZ(0.0f)
{
}

PyramidTexture::~PyramidTexture() {
    glDeleteProgram(renderingProgram);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(2, vbo);
    glDeleteTextures(1, &brickTexture);
}

void PyramidTexture::init(GLFWwindow* window) {
    renderingProgram = createShaderProgram(
        "../shaders/pyramidTextureVertex.glsl",
        "../shaders/pyramidTextureFragment.glsl"
    );

    setupVertices();
    std::string texturePath = "../textures/brick_red.png";
    if (!std::filesystem::exists(texturePath)) {
        std::cerr << "❌ Fichier introuvable : " << texturePath << std::endl;
    }
    else {
        std::cout << " Fichier trouvé : " << texturePath << std::endl;
    }
    brickTexture = loadTexture(texturePath.c_str());

    //brickTexture = loadTexture("textures/brick_red.jpg");
    brickTexture= loadTexture("../textures/brick_red.png");

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}

void PyramidTexture::setupVertices() {
    float pyramidPositions[54] = {
        // base (2 triangles)
        -1.0f, 0.0f, -1.0f,
         1.0f, 0.0f, -1.0f,
         1.0f, 0.0f,  1.0f,
        -1.0f, 0.0f, -1.0f,
         1.0f, 0.0f,  1.0f,
        -1.0f, 0.0f,  1.0f,
        // faces
        -1.0f, 0.0f, -1.0f,
         1.0f, 0.0f, -1.0f,
         0.0f, 1.0f,  0.0f,

         1.0f, 0.0f, -1.0f,
         1.0f, 0.0f,  1.0f,
         0.0f, 1.0f,  0.0f,

         1.0f, 0.0f,  1.0f,
        -1.0f, 0.0f,  1.0f,
         0.0f, 1.0f,  0.0f,

        -1.0f, 0.0f,  1.0f,
        -1.0f, 0.0f, -1.0f,
         0.0f, 1.0f,  0.0f
    };

    float pyrTexCoords[36] = {
        // coordonnées pour les 6 triangles de la base
        0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
        0.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f,
        // faces latérales
        0.0f, 0.0f,  1.0f, 0.0f,  0.5f, 1.0f,
        0.0f, 0.0f,  1.0f, 0.0f,  0.5f, 1.0f,
        0.0f, 0.0f,  1.0f, 0.0f,  0.5f, 1.0f,
        0.0f, 0.0f,  1.0f, 0.0f,  0.5f, 1.0f
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(2, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPositions), pyramidPositions, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyrTexCoords), pyrTexCoords, GL_STATIC_DRAW);
}

void PyramidTexture::display(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(renderingProgram);

    mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
    projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float aspect = (float)width / (float)height;

    /*pMat = glm::perspective(1.0472f, aspect, 0.1f, 100.0f);
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
    mMat = glm::translate(glm::mat4(1.0f), glm::vec3(objLocX, objLocY, objLocZ));
    mvMat = vMat * mMat;*/
    cameraX = -0.3f;
    cameraY = -0.3f;
    cameraZ = 6.0f;
    pMat = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));

    mMat = glm::translate(glm::mat4(1.0f), glm::vec3(objLocX, objLocY, objLocZ));
   // mMat = glm::rotate(mMat, glm::radians(-20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    mMat = glm::rotate(mMat, glm::radians(40.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    mMat = glm::scale(mMat, glm::vec3(2.5f, 2.5f, 2.5f));

    mvMat = vMat * mMat;

 

    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

    // Attributs vertex
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Coordonnées texture
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, brickTexture);

    glDrawArrays(GL_TRIANGLES, 0, 18);
}

GLuint PyramidTexture::createShaderProgram(const char* vertFile, const char* fragFile) {
    auto loadShader = [](const char* filename) -> std::string {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "❌ Impossible d’ouvrir : " << filename << std::endl;
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

GLuint PyramidTexture::loadTexture(const std::string& filename) {
    GLuint textureID;
    textureID = SOIL_load_OGL_texture(
        filename.c_str(),// "../textures/brick_red.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
    );

    if (textureID == 0) {
        std::cerr << "❌ SOIL2 erreur : " << SOIL_last_result() << std::endl;
        std::cerr << "Erreur chargement texture : " << filename << std::endl;

    }
    else {
        std::cout << "✅ Texture chargée avec succès (ID = " << textureID << ")" << std::endl;

    } 
    return textureID;
}

void PyramidTexture::framebuffer_size_callback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}


/**
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


*/