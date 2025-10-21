#pragma once
#include "CubeBase.h"
class CubeRotation : public CubeBase{
    public:
    CubeRotation();
    ~CubeRotation() override;
    void setupVertices() override;
    void display(GLFWwindow* window) override;
    void init(GLFWwindow* window) override;
};

