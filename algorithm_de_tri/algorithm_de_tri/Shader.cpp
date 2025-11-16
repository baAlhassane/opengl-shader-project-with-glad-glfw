#include "Shader.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
Shader::~Shader() {
    if (program) glDeleteProgram(program);
}

bool Shader::compileShader(GLuint shader) {
    GLint ok = 0;
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint maxLen = 0; glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLen);
        std::vector<char> buf(maxLen); glGetShaderInfoLog(shader, maxLen, nullptr, buf.data());
        std::cerr << "Shader compile error: " << buf.data() << std::endl;
        return false;
    }
    return true;
}

bool Shader::compileFromSources(const std::string& vsSrc, const std::string& fsSrc, std::string* outLog) {
    if (program) { glDeleteProgram(program); program = 0; }

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    const char* vSrc = vsSrc.c_str();
    glShaderSource(vs, 1, &vSrc, nullptr);
    if (!compileShader(vs)) { glDeleteShader(vs); return false; }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fSrc = fsSrc.c_str();
    glShaderSource(fs, 1, &fSrc, nullptr);
    if (!compileShader(fs)) { glDeleteShader(vs); glDeleteShader(fs); return false; }

    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    GLint ok = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint maxLen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLen);
        std::vector<char> buf(maxLen);
        glGetProgramInfoLog(program, maxLen, nullptr, buf.data());
        if (outLog) *outLog = std::string(buf.data());
        std::cerr << "Program link error: " << buf.data() << std::endl;
        glDeleteShader(vs);
        glDeleteShader(fs);
        return false;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return true;
}


//bool Shader::compileFromSources(const char* vsSrc, const char* fsSrc, std::string* outLog) {
//    if (program) { glDeleteProgram(program); program = 0; }
//
//    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
//    glShaderSource(vs, 1, &vsSrc, nullptr);
//    if (!compileShader(vs)) { glDeleteShader(vs); return false; }
//
//    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(fs, 1, &fsSrc, nullptr);
//    if (!compileShader(fs)) { glDeleteShader(vs); glDeleteShader(fs); return false; }
//
//    program = glCreateProgram();
//    glAttachShader(program, vs);
//    glAttachShader(program, fs);
//    glLinkProgram(program);
//
//    GLint ok = 0; glGetProgramiv(program, GL_LINK_STATUS, &ok);
//    if (!ok) {
//        GLint maxLen = 0; glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLen);
//        std::vector<char> buf(maxLen); glGetProgramInfoLog(program, maxLen, nullptr, buf.data());
//        if (outLog) *outLog = std::string(buf.data());
//        std::cerr << "Program link error: " << buf.data() << std::endl;
//        glDeleteShader(vs); glDeleteShader(fs);
//        return false;
//    }
//
//    glDeleteShader(vs);
//    glDeleteShader(fs);
//    return true;
//}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

std::string Shader::readFile(const char* path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << path << std::endl;
        exit(EXIT_FAILURE); // quitte si le fichier n'existe pas
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}


// Shader.h
Shader::Shader(const char* vsSrc, const char* fsSrc) {
    compileFromSources(vsSrc, fsSrc);
}
