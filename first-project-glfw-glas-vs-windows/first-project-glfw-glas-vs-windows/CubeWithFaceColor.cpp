#include "CubeWithFaceColor.h"
//#include "Utils.h"       // pour la fonction de création du shader
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include<vector>


CubeWithFaceColor::CubeWithFaceColor() : renderingProgram(0), vao(0), vbo(0),
cameraX(0.0f), cameraY(0.0f), cameraZ(8.0f),
cubeLocX(0.0f), cubeLocY(-2.0f), cubeLocZ(0.0f) {
}

CubeWithFaceColor::~CubeWithFaceColor() {
    glDeleteProgram(renderingProgram);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

void CubeWithFaceColor  ::init(GLFWwindow* window) {
    renderingProgram = createShaderProgram("../shaders/vertexShader.glsl", "../shaders/fragmentShader.glsl");
    setupVertices();

    // ===== Activer le test de profondeur =====
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}

void CubeWithFaceColor::setupVertices() {
    float vertexData[216] = {
        // Face avant (rouge)
        -1.0f,  1.0f, -1.0f,   1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, -1.0f,   1.0f, 0.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,   1.0f, 0.0f, 0.0f,

        // Face droite (vert)
         1.0f, -1.0f, -1.0f,   0.0f, 1.0f, 0.0f,
         1.0f, -1.0f,  1.0f,   0.0f, 1.0f, 0.0f,
         1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 0.0f,
         1.0f, -1.0f,  1.0f,   0.0f, 1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,   0.0f, 1.0f, 0.0f,
         1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 0.0f,

         // Face arrière (bleu)
          1.0f, -1.0f,  1.0f,   0.0f, 0.0f, 1.0f,
         -1.0f, -1.0f,  1.0f,   0.0f, 0.0f, 1.0f,
          1.0f,  1.0f,  1.0f,   0.0f, 0.0f, 1.0f,
         -1.0f, -1.0f,  1.0f,   0.0f, 0.0f, 1.0f,
         -1.0f,  1.0f,  1.0f,   0.0f, 0.0f, 1.0f,
          1.0f,  1.0f,  1.0f,   0.0f, 0.0f, 1.0f,

          // Face gauche (jaune)
          -1.0f, -1.0f,  1.0f,   1.0f, 1.0f, 0.0f,
          -1.0f, -1.0f, -1.0f,   1.0f, 1.0f, 0.0f,
          -1.0f,  1.0f,  1.0f,   1.0f, 1.0f, 0.0f,
          -1.0f, -1.0f, -1.0f,   1.0f, 1.0f, 0.0f,
          -1.0f,  1.0f, -1.0f,   1.0f, 1.0f, 0.0f,
          -1.0f,  1.0f,  1.0f,   1.0f, 1.0f, 0.0f,

          // Face dessous (cyan)
          -1.0f, -1.0f,  1.0f,   0.0f, 1.0f, 1.0f,
           1.0f, -1.0f,  1.0f,   0.0f, 1.0f, 1.0f,
          -1.0f, -1.0f, -1.0f,   0.0f, 1.0f, 1.0f,
           1.0f, -1.0f,  1.0f,   0.0f, 1.0f, 1.0f,
           1.0f, -1.0f, -1.0f,   0.0f, 1.0f, 1.0f,
          -1.0f, -1.0f, -1.0f,   0.0f, 1.0f, 1.0f,

          // Face dessus (magenta)
          -1.0f,  1.0f, -1.0f,   1.0f, 0.0f, 1.0f,
          -1.0f,  1.0f,  1.0f,   1.0f, 0.0f, 1.0f,
           1.0f,  1.0f, -1.0f,   1.0f, 0.0f, 1.0f,
          -1.0f,  1.0f,  1.0f,   1.0f, 0.0f, 1.0f,
           1.0f,  1.0f,  1.0f,   1.0f, 0.0f, 1.0f,
           1.0f,  1.0f, -1.0f,   1.0f, 0.0f, 1.0f
    };

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
}

//GLuint Cube::createShaderProgram(const char* vertFile, const char* fragFile) {
//    auto loadShader = [](const char* filename) -> std::string {
//        std::ifstream file(filename);
//        std::stringstream ss;
//        ss << file.rdbuf();
//        return ss.str();
//        };
//
//    std::string vertCode = loadShader(vertFile);
//    std::string fragCode = loadShader(fragFile);
//    const char* vSrc = vertCode.c_str();
//    const char* fSrc = fragCode.c_str();
//
//    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
//    glShaderSource(vShader, 1, &vSrc, nullptr);
//    glCompileShader(vShader);
//
//    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(fShader, 1, &fSrc, nullptr);
//    glCompileShader(fShader);
//
//    GLuint program = glCreateProgram();
//    glAttachShader(program, vShader);
//    glAttachShader(program, fShader);
//    glLinkProgram(program);

   /* int success;
    char infoLog[512];

    glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vShader, 512, NULL, infoLog);
        std::cerr << "Erreur vertex shader : " << infoLog << std::endl;
    }

    glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fShader, 512, NULL, infoLog);
        std::cerr << "Erreur fragment shader : " << infoLog << std::endl;
    }*/

    //    GLint success = 0;
    //    glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
    //    if (success == GL_FALSE) {
    //        GLint maxLength = 0;
    //        glGetShaderiv(vShader, GL_INFO_LOG_LENGTH, &maxLength);
    //        std::vector<GLchar> infoLog(maxLength);
    //        glGetShaderInfoLog(vShader, maxLength, &maxLength, &infoLog[0]);
    //        std::cerr << "Shader compile error: " << std::endl << &infoLog[0] << std::endl;
    //    }
    //
    //
    //
    //
    //    glDeleteShader(vShader);
    //    glDeleteShader(fShader);
    //
    //    return program;
    //}


void CubeWithFaceColor::display(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(renderingProgram);

    mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
    projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float aspect = static_cast<float>(width) / static_cast<float>(height);



 /*   pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
    mMat = glm::translate(glm::mat4(1.0f), glm::vec3(cubeLocX, cubeLocY, cubeLocZ));
    mvMat = vMat * mMat;
 */

    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
    float time = glfwGetTime();
    mMat = glm::rotate(glm::mat4(1.0f), time * glm::radians(50.0f), glm::vec3(1.0f, 1.0f, 0.0f));
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));

    mvMat = vMat * mMat;


    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

     // Ce code est un code valid pour un cube sans attribut color. 
    //glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //glEnableVertexAttribArray(0);// ici 0= location 0;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Attribut position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Attribut couleur
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glDrawArrays(GL_TRIANGLES, 0, 36);
}


GLuint CubeWithFaceColor::createShaderProgram(const char* vertFile, const char* fragFile) {
    auto loadShader = [](const char* filename) -> std::string {
        std::ifstream file(filename);

        // Vérifie si le fichier s'est bien ouvert
        if (!file.is_open()) {
            std::cerr << "❌ Impossible d'ouvrir le fichier shader : " << filename << std::endl;
            return "";
        }

        // Debug : afficher le contenu du shader
        std::string line;
        std::cout << "📄 Contenu de " << filename << " :" << std::endl;
        while (std::getline(file, line)) {
            std::cout << line << std::endl;
        }
        file.clear();
        file.seekg(0, std::ios::beg); // On remet le curseur au début pour relire

        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
        };
    // Draslom3@lhasba
    // Charger les deux shaders
    std::string vertCode = loadShader(vertFile);
    std::string fragCode = loadShader(fragFile);

    if (vertCode.empty() || fragCode.empty()) {
        std::cerr << "❌ Erreur : l'un des shaders est vide, compilation annulée." << std::endl;
        return 0;
    }

    const char* vSrc = vertCode.c_str();
    const char* fSrc = fragCode.c_str();

    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vSrc, nullptr);
    glCompileShader(vShader);

    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fSrc, nullptr);
    glCompileShader(fShader);

    // Vérification compilation vertex shader
    GLint success = 0;
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(vShader, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(vShader, maxLength, &maxLength, &infoLog[0]);
        std::cerr << "❌ Erreur compilation vertex shader :" << std::endl << &infoLog[0] << std::endl;
    }

    // Vérification compilation fragment shader
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(fShader, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(fShader, maxLength, &maxLength, &infoLog[0]);
        std::cerr << "❌ Erreur compilation fragment shader :" << std::endl << &infoLog[0] << std::endl;
    }

    // Création du programme
    GLuint program = glCreateProgram();
    glAttachShader(program, vShader);
    glAttachShader(program, fShader);
    glLinkProgram(program);

    // Vérification linkage
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
        std::cerr << "❌ Erreur linkage du programme :" << std::endl << &infoLog[0] << std::endl;
    }

    glDeleteShader(vShader);
    glDeleteShader(fShader);

    return program;
}

void CubeWithFaceColor::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    float aspect = static_cast<float>(width) / static_cast<float>(height);

    // Récupère le pointeur vers ton objet Cube associé à cette fenêtre
    CubeWithFaceColor* cube = static_cast<CubeWithFaceColor*>(glfwGetWindowUserPointer(window));
    if (cube) {
        cube->pMat = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);
    }
}



/*
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "CubeWithFaceColor.h"
#include <filesystem>
#include <glm/ext/matrix_clip_space.hpp>

//glm::mat4 proj_matrix;
//// 👇 Fonction de rappel appelée par GLFW quand la fenêtre change de taille
//void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
//    glViewport(0, 0, width, height); // met à jour la zone d’affichage OpenGL
//    float aspect = static_cast<float>(width) / static_cast<float>(height);
//    proj_matrix = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);
//}

int main() {
    if (!glfwInit()) return -1;
    std::cout << " Dossier courant : " << std::filesystem::current_path() << std::endl;

    GLFWwindow* window = glfwCreateWindow(800, 600, "Cube OpenGL", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    // 👇 ICI : on enregistre la fonction de rappel
   // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Erreur : GLAD non chargé !" << std::endl;
        return -1;
    }

    CubeWithFaceColor cube;
    cube.init(window);
    // Associe le cube à la fenêtre
    glfwSetWindowUserPointer(window, &cube);
    // Enregistre la méthode statique comme callback
    glfwSetFramebufferSizeCallback(window, CubeWithFaceColor::framebuffer_size_callback);

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


*/