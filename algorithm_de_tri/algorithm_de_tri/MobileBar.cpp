#include "MobileBar.h"
#include <algorithm>

MobileBar::~MobileBar() {
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);
}

void MobileBar::init(float barWidthLocal, float initialHeight) {
    width = barWidthLocal;
    std::vector<float> mobileVerts = {
        0.0f, -0.9f,
        width, -0.9f,
        width, -0.9f + initialHeight * 1.6f,
        0.0f, -0.9f,
        width, -0.9f + initialHeight * 1.6f,
        0.0f, -0.9f + initialHeight * 1.6f
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, mobileVerts.size() * sizeof(float), mobileVerts.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void MobileBar::updateHeight(float h) {
    if (!vbo) return;
    float hh = std::clamp(h, 0.0f, 1.0f);

    std::vector<float> mobileVerts = {
        0.0f, -0.9f,
        width, -0.9f,
        width, -0.9f + hh * 1.6f,
        0.0f, -0.9f,
        width, -0.9f + hh * 1.6f,
        0.0f, -0.9f + hh * 1.6f
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, mobileVerts.size() * sizeof(float), mobileVerts.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void MobileBar::draw(Shader& shader, float offsetX) {
    shader.use();

    // Couleur rouge (barre mobile)
    GLint colorLoc = glGetUniformLocation(shader.id(), "uColor");
    glUniform4f(colorLoc, 1.0f, 0.0f, 0.0f, 1.0f);

    // Position horizontale
    GLint offsetLoc = glGetUniformLocation(shader.id(), "uOffsetX");
    glUniform1f(offsetLoc, offsetX);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

