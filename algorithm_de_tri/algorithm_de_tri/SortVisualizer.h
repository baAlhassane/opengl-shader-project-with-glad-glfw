#pragma once
#include <vector>
#include "Bars.h"
#include "Shader.h"
#include "MobileBar.h"
#include <GLFW/glfw3.h>

class SortingVisualizer {
public:


    void selectionSortStep();
    void run();
    unsigned int VAO = 0, VBO = 0;            // pour toutes les barres statiques
    unsigned int mobileVAO = 0, mobileVBO = 0; // pour la barre mobile (rouge)
    unsigned int WIDTH = 1600, HEIGHT = 800;           // pour toutes les barres statiques
    int numBars = 0;
    void setWindowSize(int w, int h);
    void setupBuffers();
    void updateBarVertices(int emptyIndex);
    void setupShaders();
    void initGL();
    void render();
    void update();
    void insertionSortStep();
    void swapBars(int i, int j);
    void drawBar(float x, float height, bool highlighted = false);
    GLFWwindow* window = nullptr;

    //
    SortingVisualizer(int size);
    int minIndex = -1;    // index du minimum en cours
    int scanIndex = -1;   // index pour parcourir j
    bool swapping = false; // indique qu'on est en train d'animer l'échange

private:
    Shader shader;        // shader pour dessiner les barres
    MobileBar mobileBar;  // la barre à afficher
    float barWidth;       // largeur de la barre rouge
    
    std::vector<float> values;
    bool sorting;
    int currentIndex = 1;      // i
    int compareIndex = -1;     // j
    bool sortingDone = false;
    bool isComparing = false;
    float currentValue = 0.0f; // vraie valeur de l'élément i

    // animation mobile
    float mobileOffset = 0.0f; // offset courant (clip-space) appliqué à la barre mobile
};

