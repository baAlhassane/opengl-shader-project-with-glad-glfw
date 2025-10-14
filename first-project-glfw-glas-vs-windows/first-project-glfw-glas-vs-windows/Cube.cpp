#include "Cube.h"
//#include "Utils.h"       // pour la fonction de création du shader
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include<vector>S


Cube::Cube() : renderingProgram(0), vao(0), vbo(0),
cameraX(0.0f), cameraY(0.0f), cameraZ(8.0f),
cubeLocX(0.0f), cubeLocY(-2.0f), cubeLocZ(0.0f) {
}

Cube::~Cube() {
    glDeleteProgram(renderingProgram);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

void Cube::setupVertices() {
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

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
}

void Cube::init(GLFWwindow* window) {
    renderingProgram = createShaderProgram("../shaders/vertexShader.glsl", "../shaders/fragmentShader.glsl");
    setupVertices();

    // ===== Activer le test de profondeur =====
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}

void Cube::display(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(renderingProgram);

    mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
    projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float aspect = static_cast<float>(width) / static_cast<float>(height);

    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
    mMat = glm::translate(glm::mat4(1.0f), glm::vec3(cubeLocX, cubeLocY, cubeLocZ));
    mvMat = vMat * mMat;

    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glDrawArrays(GL_TRIANGLES, 0, 36);
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

GLuint Cube::createShaderProgram(const char* vertFile, const char* fragFile) {
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
