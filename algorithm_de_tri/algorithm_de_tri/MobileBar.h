#pragma once
#include <glad/glad.h>
#include <vector>
#include"Shader.h"

class MobileBar {
public:
    MobileBar() = default;
    ~MobileBar();

    void init(float barWidthLocal, float initialHeight);
    void updateHeight(float h);
    void draw(Shader& shader, float width); // prend Shader et width

private:
    GLuint vao = 0;
    GLuint vbo = 0;
    float width = 0.0f;
   
};
