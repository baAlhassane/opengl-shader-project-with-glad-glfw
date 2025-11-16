#pragma once
#include <string>
#include <glad/glad.h>


class Shader {
public:
    Shader() = default;
    ~Shader();

    //bool compileFromSources(const char* vsSrc, const char* fsSrc, std::string* outLog = nullptr);
    bool  compileFromSources(const std::string& vsSrc, const std::string& fsSrc, std::string* outLog = nullptr);
    void use() const { glUseProgram(program); }
    GLuint id() const { return program; }
    void setFloat(const std::string& name, float value) const;
    std::string readFile(const char* path);
    Shader(const char*, const char*);
private:
    bool compileShader(GLuint shader);
    GLuint program = 0;
    unsigned int ID;
    
};
