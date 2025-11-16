#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "MobileBar.h"
#include "Shader.h"
#include "Bars.h"
//#include "SortVisualizer.h"



#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <string>
#include<algorithm>




// =============================================================
//  CLASSE MOBILEBAR


// =============================================================
//  CLASSE SORTINGVISUALIZER
// =============================================================
class SelectionSortVisualiser {
public:
    SelectionSortVisualiser(int size);
    ~SelectionSortVisualiser();
    unsigned int WIDTH = 1600;
    unsigned int HEIGHT = 800;
    void setWindowSize(int w, int h);
    void run();
    void initGL();
    void setupShaders();
    void updateBarVertices(int emptyIndex = -1);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    void selectionSortStep();

    int minIndex = -1;    // index du minimum en cours
    int scanIndex = -1;   // index pour parcourir j
    bool swapping = false; // indique qu'on est en train d'animer l'échange
    bool stepSelectionSort();
    static void windowCloseCallback(GLFWwindow* win);
    



private:

    void setupBuffers();
    // mise à jour VBO: emptyIndex indique l'indice à laisser vide (hauteur = 0)

    void render();
    //void insertionSortStep();

    Shader shader;
    MobileBar mobileBar;
    GLFWwindow* window = nullptr;
    std::vector<float> values;
    int currentIndex = 1;      // i
    int compareIndex = -1;     // j
    bool sortingDone = false;
    bool isComparing = false;
    float currentValue = 0.0f; // vraie valeur de l'élément i

    //int currentIndex = 0;     // i
   // int compareIndex = -1;    // j
    //int minIndex = -1;        // minIndex
    int sortedBoundary = 0;   // elements deja tries


    // animation mobile
    float mobileOffset = 0.0f; // offset courant (clip-space) appliqué à la barre mobile



    unsigned int shaderProgram = 0;
    unsigned int VAO = 0, VBO = 0;            // pour toutes les barres statiques
    unsigned int mobileVAO = 0, mobileVBO = 0; // pour la barre mobile (rouge)
    int numBars = 0;

    bool forceStop = false;
  


    // Shaders
    const char* vertexShaderSource = R"(
       #version 330 core
       layout(location = 0) in vec2 aPos;
       uniform float uOffsetX;
       void main() {
           vec2 pos = aPos;
           pos.x += uOffsetX;
           gl_Position = vec4(pos, 0.0, 1.0);
       }
   )";

    const char* fragmentShaderSource = R"(
       #version 330 core
       out vec4 FragColor;
       uniform vec4 uColor;
       void main() {
           FragColor = uColor;
       }
   )";
};


void SelectionSortVisualiser::windowCloseCallback(GLFWwindow* win) {
    SelectionSortVisualiser* self =
        static_cast<SelectionSortVisualiser*>(glfwGetWindowUserPointer(win));

    if (self) {
        self->forceStop = true;  // On arrête l'animation immédiatement
        self->sortingDone = true;
    }

    // Et on autorise GLFW à fermer la fenêtre
    glfwSetWindowShouldClose(win, GLFW_TRUE);
}


void SelectionSortVisualiser::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    SelectionSortVisualiser* visualizer = static_cast<SelectionSortVisualiser*>(glfwGetWindowUserPointer(window));
    if (visualizer) {
        // glViewport(0, 0, width, height);
        visualizer->setWindowSize(width, height);
    }
}

SelectionSortVisualiser::SelectionSortVisualiser(int size)
    : numBars(size), window(nullptr), sortingDone(false),
    currentIndex(0), isComparing(false), mobileOffset(0.0f)
{
    // Génération des valeurs du tableau (aucun OpenGL ici)
    values.resize(size);
    std::srand((unsigned)std::time(nullptr));
    for (int i = 0; i < size; ++i)
        values[i] = (i + 1) / float(size);

    // Mélange aléatoire
    for (int i = size - 1; i > 0; --i)
        std::swap(values[i], values[std::rand() % (i + 1)]);
}



void SelectionSortVisualiser::setupShaders() {
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShaderSource, nullptr);
    glCompileShader(vs);
    GLint ok = 0;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char buf[1024]; glGetShaderInfoLog(vs, 1024, nullptr, buf);
        std::cerr << "VS: " << buf << std::endl;
    }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
    if (!ok) { char buf[1024]; glGetShaderInfoLog(fs, 1024, nullptr, buf); std::cerr << "FS: " << buf << std::endl; }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &ok);
    if (!ok) { char buf[1024]; glGetProgramInfoLog(shaderProgram, 1024, nullptr, buf); std::cerr << "LINK: " << buf << std::endl; }

    glDeleteShader(vs);
    glDeleteShader(fs);
}

SelectionSortVisualiser::~SelectionSortVisualiser() {
    if (VBO) glDeleteBuffers(1, &VBO);
    if (VAO) glDeleteVertexArrays(1, &VAO);
}

void SelectionSortVisualiser::setWindowSize(int w, int h) { WIDTH = w; HEIGHT = h; }


void SelectionSortVisualiser::initGL() {

    if (!glfwInit()) { std::cerr << "GLFW init failed\n"; exit(-1); }

    // ⛔ Masquer la fenêtre au début !
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Tri par selection - Visualisation", nullptr, nullptr);
    if (!window) { std::cerr << "Failed to create GLFW window\n"; glfwTerminate(); exit(-1); }

    glfwMakeContextCurrent(window);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        exit(-1);
    }

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glClearColor(0.1f, 0.12f, 0.14f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // --- SHADERS ---
    std::string vsSrc = shader.readFile("../shaders/vertex_shader_tri_insertion.glsl");
    std::string fsSrc = shader.readFile("../shaders/fragment_shader_tri_insertion.glsl");
    if (vsSrc.empty() || fsSrc.empty()) { std::cerr << "Erreur : shader vide !"; exit(-1); }
    if (!shader.compileFromSources(vsSrc, fsSrc)) { std::cerr << "Erreur shader !"; exit(-1); }

    // --- IMGUI INIT ---
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // --- BUFFERS + GEOMETRY ---
    mobileBar.init(0.0f, 0.0f);
    setupShaders();
    setupBuffers();

    // 🟢 Maintenant que TOUT est prêt : montrer la fenêtre
    glfwShowWindow(window);

    // 🟢 Faire un premier rendu pour éviter tout flash
    render();
    glfwSwapBuffers(window);
}



// --- setup buffers ---
void SelectionSortVisualiser::setupBuffers() {
    if (VAO == 0) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // On réserve la mémoire une seule fois
        glBufferData(GL_ARRAY_BUFFER,
            numBars * 12 * sizeof(float),
            nullptr,
            GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
        glEnableVertexAttribArray(0);
    }
    else {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
    }

    // Maintenant on génère les vertices
    std::vector<float> vertices;
    vertices.reserve(numBars * 12);

    //float spacing = 1.3f;
   // float barWidth = 2;

    /*std::vector<float> vertices;
    vertices.reserve(numBars * 12); */// 6 triangles * 2 coords

    float spacing = 1.3f;
    float barWidth = 2.0f / (numBars * spacing);

    for (int i = 0; i < numBars; ++i) {
        float h = values[i];
        float left = -1.0f + i * barWidth * spacing;
        float right = left + barWidth;
        float top = -0.9f + h * 1.6f;

        vertices.insert(vertices.end(), {
            left, -0.9f,   right, -0.9f,   right, top,
            left, -0.9f,   right, top,     left,  top
            });
    }



    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // ⚠️ IMPORTANT : comme la taille reste constante, glBufferSubData suffit
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());

    glBindBuffer(GL_ARRAY_BUFFER, 0);


}

void SelectionSortVisualiser::updateBarVertices(int emptyIndex) {
    std::vector<float> vertices;
    float spacing = 1.3f;
    float barWidth = 2.0f / (numBars * spacing);
    for (int i = 0; i < numBars; ++i) {
        float h = (i == emptyIndex) ? 0.0f : std::clamp(values[i], 0.0f, 1.0f);
        float left = -1.0f + i * barWidth * spacing;
        float right = left + barWidth;
        vertices.insert(vertices.end(), {
            left,-0.9f, right,-0.9f, right,-0.9f + h * 1.6f,
            left,-0.9f, right,-0.9f + h * 1.6f, left,-0.9f + h * 1.6f
            });
    }
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    mobileBar.updateHeight(currentValue);
    if (sortingDone) {
        compareIndex = -1;
        mobileOffset = 0.0f;
    }

}

bool SelectionSortVisualiser::stepSelectionSort() {
    if (currentIndex >= numBars) return false;

    if (compareIndex == -1) {
        minIndex = currentIndex;
        compareIndex = currentIndex + 1;
        return true;
    }

    if (compareIndex < numBars) {
        if (values[compareIndex] < values[minIndex]) {
            minIndex = compareIndex;
        }
        compareIndex++;
        return true;
    }

    // swap après avoir fini la boucle
    std::swap(values[currentIndex], values[minIndex]);

    // 🔵 Reset pour préparer le prochain i
    scanIndex = -1;
    minIndex = -1;
    currentIndex++;
    compareIndex = -1;

    setupBuffers();


    return true;
}


void SelectionSortVisualiser::render() {
    glClear(GL_COLOR_BUFFER_BIT);
    int dw, dh;
    glfwGetFramebufferSize(window, &dw, &dh);
    glViewport(0, 0, dw, dh);
    shader.use();

    float spacing = 1.3f;
    float barWidth = 2.0f / (numBars * spacing);

    glBindVertexArray(VAO);

    // --- Dessin des barres ---
    for (int i = 0; i < numBars; ++i) {

        float r = 1, g = 1, b = 1, a = 1;

        if (i < sortedBoundary) {
            // ✓ éléments déjà triés
            r = 0.22f; g = 0.78f; b = 0.22f; a = 1.0f;
        }
        else if (i == currentIndex) {
            // 🔴 index i en cours de sélection
            r = 1.0f; g = 0.0f; b = 0.0f; a = 0.8f;
        }
        else if (i == minIndex) {
            // 🟡 minimum courant
            r = 1.0f; g = 1.0f; b = 0.0f; a = 1.0f;
        }
        else if (i == compareIndex) {
            // 🔵 élément en comparaison
            r = 0.2f; g = 0.4f; b = 1.0f; a = 1.0f;
        }

        glUniform4f(glGetUniformLocation(shader.id(), "uColor"), r, g, b, a);
        glUniform1f(glGetUniformLocation(shader.id(), "uOffsetX"), 0.0f);

        glDrawArrays(GL_TRIANGLES, i * 6, 6);
    }

    glBindVertexArray(0);

    // ---------- IMGUI ----------
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //  Barre rouge : élément courant à insérer
   /* ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
        "Rouge : element a inserer (i = %d, val = %.2f)", currentIndex, currentValue);*/

    ImGui::Begin("Legende Selection Sort", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::TextColored(ImVec4(1, 0, 0, 0.8f), "Rouge : index i = %d  (position du prochain minimum)", currentIndex);
    ImGui::TextColored(ImVec4(0.2f, 0.4f, 1, 1), "Bleu : comparaison index j= %d" , compareIndex);
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Jaune : minimum index courant minIndex= %d",minIndex);
    ImGui::TextColored(ImVec4(0.22f, 0.78f, 0.22f, 1), "Vert : elements deja tries");

    ImGui::Text("i = %d, j = %d, minIndex = %d", currentIndex, compareIndex, minIndex);

    ImGui::End();

    // dessin indices
    ImDrawList* draw_list = ImGui::GetForegroundDrawList();
    for (int i = 0; i < numBars; ++i) {
        float left = -1.0f + i * barWidth * spacing;
        float center = left + barWidth / 2.0f;
        float x = (center * 0.5f + 0.5f) * float(WIDTH);
        float y = float(dh) - 18.0f;

        draw_list->AddText(ImVec2(x - 6, y), IM_COL32(255, 255, 255, 255),
            std::to_string(i).c_str());
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    //glfwSwapBuffers(window);
}


void SelectionSortVisualiser::selectionSortStep() {
    if (sortingDone) return;


    // Début d’un cycle i
    if (scanIndex == -1) {
        scanIndex = currentIndex + 1;
        minIndex = currentIndex;
    }

    // Fin du scan j : on swap
    if (scanIndex >= numBars) {

        // animation du swap
        float spacingFactor = 1.3f;
        float barWidth = 2.0f / (numBars * spacingFactor);
        float step = barWidth * spacingFactor;

        swapping = true;

        float posA = -1.0f + currentIndex * step;
        float posB = -1.0f + minIndex * step;

        const int animSteps = 20;
        for (int s = 0; s <= animSteps; s++) {
            float t = float(s) / animSteps;
            mobileOffset = t * (posA - posB); // déplace la barre mobile visuellement

            updateBarVertices();
            render();

            glfwSwapBuffers(window);    // présenter l'étape
            glfwPollEvents();           // traiter les évènements (important pour la fermeture)

            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }

        std::swap(values[currentIndex], values[minIndex]);

        swapping = false;

        // --- mettre à jour le boundary des éléments triés ---
        sortedBoundary = currentIndex + 1;   // 🟢 Toutes les barres <= currentIndex deviennent vertes
        // reset pour prochain i
        scanIndex = -1;
        minIndex = -1;
        currentIndex++;

        if (currentIndex >= numBars - 1) {
            sortingDone = true;

            // Fix : aucune barre ne doit rester colorée rouge ou jaune
            currentIndex = -1;
            compareIndex = -1;
            minIndex = -1;

            sortedBoundary = numBars;   // toutes les barres deviennent vertes
        }


        updateBarVertices();
        render();

        return;
    }

    // Étape j : comparaison
    compareIndex = scanIndex;

    if (values[scanIndex] < values[minIndex]) {
        minIndex = scanIndex;
    }

    scanIndex++;

    updateBarVertices();
    render();
    std::this_thread::sleep_for(std::chrono::milliseconds(800));
}

void SelectionSortVisualiser::run() {
    initGL();


    // ⚠️ IMPORTANT : juste après initGL(), la fenêtre existe.
    glfwSetWindowCloseCallback(window, windowCloseCallback);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (!sortingDone && !forceStop) {   // <--- on ajoute forceStop
            selectionSortStep();
        }
        else {
            // Tri terminé : dernière frame sans animation
            render();
            glfwSwapBuffers(window);   // swap une seule fois ici
            std::this_thread::sleep_for(std::chrono::milliseconds(400));
        }

        // seul swap par frame : ici
        glfwSwapBuffers(window);
    }

    // Nettoyage
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}





// =============================================================
// MAIN
// =============================================================
int main() {
    SelectionSortVisualiser vis(20);
    vis.run();
    return 0;
}


