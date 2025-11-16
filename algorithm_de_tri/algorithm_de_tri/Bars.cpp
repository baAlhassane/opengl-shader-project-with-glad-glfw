#include "Bars.h"
#include <iostream>

BarSet::~BarSet() {
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);
}

void BarSet::init(const std::vector<float>& values, float spacingFactor) {
    spacing = spacingFactor;
    n = (int)values.size();
    barWidth = 2.0f / (n * spacing);

    // build initial vertex buffer (same code que tu avais)
    std::vector<float> vertices;
    vertices.reserve(n * 6 * 2);

    for (int i = 0; i < n; ++i) {
        float h = values[i];
        float left = -1.0f + i * barWidth * spacing;
        float right = left + barWidth;
        vertices.insert(vertices.end(), {
            left, -0.9f,  right, -0.9f,  right, -0.9f + h * 1.6f,
            left, -0.9f,  right, -0.9f + h * 1.6f,  left, -0.9f + h * 1.6f
            });
    }

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    // attribut position location=0 (2 floats)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void BarSet::update(const std::vector<float>& values, int emptyIndex) {
    if ((int)values.size() != n) return; // sécurité
    std::vector<float> vertices;
    vertices.reserve(n * 6 * 2);

    for (int i = 0; i < n; ++i) {
        float h = (i == emptyIndex) ? 0.0f : values[i];
        if (h < 0.0f) h = 0.0f; if (h > 1.0f) h = 1.0f;
        float left = -1.0f + i * barWidth * spacing;
        float right = left + barWidth;
        vertices.insert(vertices.end(), {
            left, -0.9f,  right, -0.9f,  right, -0.9f + h * 1.6f,
            left, -0.9f,  right, -0.9f + h * 1.6f,  left, -0.9f + h * 1.6f
            });
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BarSet::drawBar(int i) const {
    if (i < 0 || i >= n) return;
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, i * 6, 6);
    glBindVertexArray(0);
}

void BarSet::drawAll() const {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, n * 6);
    glBindVertexArray(0);
}
