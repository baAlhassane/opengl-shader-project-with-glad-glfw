#pragma once
#include <vector>
#include <glad/glad.h>

class BarSet {
public:
    BarSet() = default;
    ~BarSet();

    void init(const std::vector<float>& values, float spacingFactor = 1.3f);
    void update(const std::vector<float>& values, int emptyIndex = -1);

    void bind() const { glBindVertexArray(vao); }
    void unbind() const { glBindVertexArray(0); }
    void drawBar(int i) const;
    void drawAll() const;

    int numBars() const { return n; }
    float getBarWidth() const { return barWidth; }
    float getSpacingFactor() const { return spacing; }

private:
    GLuint vao = 0, vbo = 0;
    int n = 0;
    float spacing = 1.3f;
    float barWidth = 0.0f;
};
