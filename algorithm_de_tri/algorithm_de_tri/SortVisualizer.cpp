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
class SortingVisualizer {
public:
    SortingVisualizer(int size);
    ~SortingVisualizer();
    unsigned int WIDTH = 1600;
    unsigned int HEIGHT = 800;
    void setWindowSize(int w, int h);
    void run();
    void initGL();
    void setupShaders();
    void updateBarVertices(int emptyIndex = -1);

private:
  
    void setupBuffers();
    // mise à jour VBO: emptyIndex indique l'indice à laisser vide (hauteur = 0)
  
    void render();
    void insertionSortStep();

    Shader shader;
    MobileBar mobileBar;
    GLFWwindow* window = nullptr;
    std::vector<float> values;
    int currentIndex = 1;      // i
    int compareIndex = -1;     // j
    bool sortingDone = false;
    bool isComparing = false;
    float currentValue = 0.0f; // vraie valeur de l'élément i

    // animation mobile
    float mobileOffset = 0.0f; // offset courant (clip-space) appliqué à la barre mobile



    unsigned int shaderProgram = 0;
    unsigned int VAO = 0, VBO = 0;            // pour toutes les barres statiques
    unsigned int mobileVAO = 0, mobileVBO = 0; // pour la barre mobile (rouge)
    int numBars = 0;

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    SortingVisualizer* visualizer = static_cast<SortingVisualizer*>(glfwGetWindowUserPointer(window));
    if (visualizer) {
        // glViewport(0, 0, width, height);
        visualizer->setWindowSize(width, height);
    }
}

SortingVisualizer::SortingVisualizer(int size)
    : numBars(size), window(nullptr), sortingDone(false),
    currentIndex(1), isComparing(false), mobileOffset(0.0f)
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

// -------------------------
//SortingVisualizer::SortingVisualizer(int size) : numBars(size) {
//    values.resize(size);
//    //shader = Shader("vertex_shader.glsl", "fragment_shader.glsl"); // tes fichiers shader
//    /*std::string vsSrc = shader.readFile("../shaders/vertex_shader_tri_insertion.glsl");
//    std::string fsSrc = shader.readFile("../shaders/fragment_shader_tri_insertion.glsl");
//    */
//
//    std::string vsSrc = shader.readFile("../shaders/vertex_shader_tri_insertion.glsl");
//    std::string fsSrc = shader.readFile("../shaders/fragment_shader_tri_insertion.glsl");
//    if (vsSrc.empty() || fsSrc.empty()) {
//        std::cerr << "Erreur : un des shaders est vide !" << std::endl;
//        exit(EXIT_FAILURE);
//    }
//
//    if (!shader.compileFromSources(vsSrc, fsSrc)) {
//        std::cerr << "Erreur de compilation du shader  !" << std::endl;
//        exit(EXIT_FAILURE);
//    }
//    
//    mobileBar.init(0.0f, 0.0f); // position initiale de la barre
//    std::srand((unsigned)std::time(nullptr));
//    for (int i = 0; i < size; ++i) values[i] = (i + 1) / float(size);
//    for (int i = size - 1; i > 0; --i) std::swap(values[i], values[std::rand() % (i + 1)]);
//}

void SortingVisualizer::setupShaders() {
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

SortingVisualizer::~SortingVisualizer() {
    if (VBO) glDeleteBuffers(1, &VBO);
    if (VAO) glDeleteVertexArrays(1, &VAO);
}

void SortingVisualizer::setWindowSize(int w, int h) { WIDTH = w; HEIGHT = h; }

void SortingVisualizer::initGL() {
    if (!glfwInit()) { std::cerr << "GLFW init failed\n"; exit(-1); }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Tri par insertion - Visualisation", nullptr, nullptr);
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

    // --- Chargement et compilation des shaders ---
    std::string vsSrc = shader.readFile("../shaders/vertex_shader_tri_insertion.glsl");
    std::string fsSrc = shader.readFile("../shaders/fragment_shader_tri_insertion.glsl");
    if (vsSrc.empty() || fsSrc.empty()) {
        std::cerr << "Erreur : un des shaders est vide !" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (!shader.compileFromSources(vsSrc, fsSrc)) {
        std::cerr << "Erreur de compilation du shader !" << std::endl;
        exit(EXIT_FAILURE);
    }

    // --- Initialisation OpenGL après contexte ---

    // ---- IMGUI INIT ----
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark(); // ou ImGui::StyleColorsLight()

    // Ces deux lignes sont CRUCIALES :
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");  // <--- c’est ça qui manquait !
    mobileBar.init(0.0f, 0.0f);
    setupShaders();
    setupBuffers();
}




// --- setup buffers ---
void SortingVisualizer::setupBuffers() {
    std::vector<float> vertices;
    float spacing = 1.3f;
    float barWidth = 2.0f / (numBars * spacing);
    for (int i = 0; i < numBars; ++i) {
        float h = values[i];
        float left = -1.0f + i * barWidth * spacing;
        float right = left + barWidth;
        vertices.insert(vertices.end(), {
            left,-0.9f, right,-0.9f, right,-0.9f + h * 1.6f,
            left,-0.9f, right,-0.9f + h * 1.6f, left,-0.9f + h * 1.6f
            });
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    mobileBar.init(barWidth, values[0]);
}

void SortingVisualizer::updateBarVertices(int emptyIndex) {
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



void SortingVisualizer::render() {
    glClear(GL_COLOR_BUFFER_BIT);
    int dw, dh;
    glfwGetFramebufferSize(window, &dw, &dh);
    glViewport(0, 0, dw, dh);
    shader.use();

    float spacing = 1.3f;
    float barWidth = 2.0f / (numBars * spacing);
    glBindVertexArray(VAO);
    float step = barWidth;
   int  display_h = dh;


    //    glClear(GL_COLOR_BUFFER_BIT);
    //    int display_w, display_h;
    //    glfwGetFramebufferSize(window, &display_w, &display_h);
    //    glViewport(0, 0, display_w, display_h);
    //    glUseProgram(shaderProgram);
    //    float spacingFactor = 1.3f;
    //    float barWidth = 2.0f / float(numBars*spacingFactor);
    //    float step = barWidth;
    //    //float spacingFactor = 1.05f; // au lieu de 1.3f
    //   // float spacingFactor = 1.3f;

    // --- Dessin des barres fixes ---
    for (int i = 0; i < numBars; ++i) {
        float r = 1, g = 1, b = 1, a = 1;
        if (i == currentIndex) { r = 1; g = 0.2f; b = 0.2f; a = 0.35f; } // barre active (rose clair)
        else if (i < currentIndex) { r = 0.22f; g = 0.78f; b = 0.22f; a = 1.0f; } // barres triées (vertes)
        glUniform4f(glGetUniformLocation(shader.id(), "uColor"), r, g, b, a);
        glUniform1f(glGetUniformLocation(shader.id(), "uOffsetX"), 0.0f);
        glDrawArrays(GL_TRIANGLES, i * 6, 6);
    }

    glBindVertexArray(0);

    // --- Barre rouge mobile ---
    if (isComparing) {
        float leftCurr = -1.0f + currentIndex * barWidth * spacing;
        float offsetX = leftCurr + mobileOffset;
        mobileBar.draw(shader, offsetX);
    }



    // ---------- IMGUI ----------
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // legend window bigger and fixed once
    ImGui::SetNextWindowSize(ImVec2(520, 260), ImGuiCond_Once);

    ImGui::Begin("Legende et informations", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Legende :");

    //  Barre rouge : élément courant à insérer
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
        "Rouge : element a inserer (i = %d, val = %.2f)", currentIndex, currentValue);

    // Barre verte : élément en comparaison
    if (compareIndex >= 0 && compareIndex < values.size()) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f),
            "Vert : element compare (j = %d, val = %.2f)", compareIndex, values[compareIndex]);
    }
    else {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f),
            "Vert : element compare (aucune comparaison en cours)");
    }

    // Vert clair : éléments déjà triés
    ImGui::TextColored(ImVec4(0.22f, 0.78f, 0.22f, 1.0f),
        "Vert clair : elements deja tries (indices  j  = %d < i = %d)" , compareIndex,currentIndex);
    // Barre rouge demon– emplacement temporaire
    ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 0.35f),
        "Rouge demon : repere la pos i  de elt a inserer i = %d) ",currentIndex);

    // ⚪ Blanc : éléments non encore triés
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
        "Blanc : elements non encore tries (indices,i >   %d)", currentIndex);


    ImGui::End();



    ImDrawList* draw_list = ImGui::GetForegroundDrawList();
    for (int i = 0; i < numBars; ++i) {
        float leftClip = -1.0f + i * step*spacing;
       // float barWidth = 2.0f / (numBars * spacingFactor);
        float centerClip = leftClip + (barWidth/2.0f);
        float xPixel = (centerClip * 0.5f + 0.5f) * float(WIDTH);
        float yPixel = float(display_h) - 18.0f;

       // float yPixel = float(HEIGHT) - 18.0f;
        draw_list->AddText(ImVec2(xPixel - 6, yPixel), IM_COL32(255, 255, 255, 255), std::to_string(i).c_str());
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


    glfwSwapBuffers(window);
}



void SortingVisualizer::insertionSortStep() {
    if (sortingDone) return;
    if (currentIndex >= numBars) { sortingDone = true; isComparing = false; return; }

    currentValue = values[currentIndex]; // élément rouge
    float key = currentValue;
    int j = currentIndex - 1;
    mobileOffset = 0.0f;
    float spacingFactor = 1.3f;
    float barWidth = 2.0f / float(numBars*spacingFactor);
    float step = barWidth * 1.3f;

    isComparing = false;
    compareIndex = -1+1;
    updateBarVertices();
    render();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    int emptyIndex = currentIndex; // position du trou initial (au début le trou est là où est l'élément rouge)

    while (j >= 0 && values[j] > key) {
        compareIndex = j;
        isComparing = true;

        // on veut que la case laissée vide soit la position j (la valeur de j est déplacée vers j+1)
        emptyIndex = j;

        // Animation du déplacement de la barre rouge
        float targetOffset = (compareIndex - currentIndex) * step;
        const int stepsAnim = 20;
        const int ms_per_step = 150;
        float startOffset = mobileOffset;

        for (int s = 1; s <= stepsAnim; ++s) {
            float t = float(s) / float(stepsAnim);
            mobileOffset = startOffset + (targetOffset - startOffset) * t;
            updateBarVertices(emptyIndex+1); // on indique la position vide
            render();
            std::this_thread::sleep_for(std::chrono::milliseconds(ms_per_step));
        }

        // Décalage des valeurs : la valeur de j est copiée vers j+1
        values[j + 1] = values[j];

        // update en conservant la case j comme vide
        updateBarVertices(emptyIndex+1);
        render();
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        j--;
    }
    if (currentIndex >= numBars) {
        sortingDone = true;
        isComparing = false;
        mobileOffset = 0.0f;
        updateBarVertices();
        render();

        // Attendre un petit moment avant de fermer
        std::this_thread::sleep_for(std::chrono::seconds(2));
        glfwSetWindowShouldClose(window, true);
        return;
    }


    // placer la clé dans le trou final (emptyIndex contient l'indice du trou)
    values[emptyIndex] = key;

    // Animation finale d’insertion (mobile se déplace vers le trou final)
    float finalTargetOffset = ((emptyIndex)-currentIndex) * step;
    {
        const int stepsAnim2 = 20;
        const int ms_per_step2 = 100;
        float startOffset = mobileOffset;
        for (int s = 1; s <= stepsAnim2; ++s) {
            float t = float(s) / float(stepsAnim2);
            mobileOffset = startOffset + (finalTargetOffset - startOffset) * t;
            updateBarVertices(emptyIndex);
            render();
            std::this_thread::sleep_for(std::chrono::milliseconds(ms_per_step2));
        }
    }

    isComparing = false;
    mobileOffset = 0.0f;
    updateBarVertices(); // plus de trou visible
    render();
    std::this_thread::sleep_for(std::chrono::seconds(1));

    currentIndex++;
}

void SortingVisualizer::run() {
    initGL();
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (!sortingDone) insertionSortStep();
        else {
            render(); 
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); }
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}


//void SortingVisualizer::run() {
//    initGL();
//    while (!glfwWindowShouldClose(window)) {
//        glfwPollEvents();
//
//
//        if (!sortingDone) {
//            insertionSortStep();  // Effectue un pas de tri
//        }
//        else {
//            // Tri terminé : affiche une dernière fois, sans animation
//           // render();
//            //std::this_thread::sleep_for(std::chrono::milliseconds(100));
//            glClear(GL_COLOR_BUFFER_BIT);
//            render();
//            glfwPollEvents();
//            continue;  // <-- évite le swap et les doubles appels
//        }
//
//        glfwSwapBuffers(window);
//    }
//
//
//    // Nettoyage
//    ImGui_ImplOpenGL3_Shutdown();
//    ImGui_ImplGlfw_Shutdown();
//    ImGui::DestroyContext();
//    glfwDestroyWindow(window);
//    glfwTerminate();
//}

// =============================================================
// MAIN
// =============================================================
//int main() {
//    SortingVisualizer vis(20);
//    vis.run();
//    return 0;
//}






//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//code qui marche parfaitement. Avec la visualisation des 20 bares et reglage de la dimension de l'ecrant
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


//// main.cpp
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//
//#include <vector>
//#include <iostream>
//#include <thread>
//#include <chrono>
//#include <cstdlib>
//#include <string>
//#include <ctime>
//
//#include "imgui.h"
//#include "imgui_impl_glfw.h"
//#include "imgui_impl_opengl3.h"
//
//
//
//class SortingVisualizer {
//public:
//    SortingVisualizer(int size);
//    ~SortingVisualizer();
//    unsigned int WIDTH = 1600;
//    unsigned int HEIGHT = 800;
//    void setWindowSize(int w, int h);
//    void run();
//
//private:
//    void initGL();
//    void setupShaders();
//    void setupBuffers();
//    // mise à jour VBO: emptyIndex indique l'indice à laisser vide (hauteur = 0)
//    void updateBarVertices(int emptyIndex = -1);
//    void render();
//    void insertionSortStep();
//
//    GLFWwindow* window = nullptr;
//    std::vector<float> values;
//    int currentIndex = 1;      // i
//    int compareIndex = -1;     // j
//    bool sortingDone = false;
//    bool isComparing = false;
//    float currentValue = 0.0f; // vraie valeur de l'élément i
//
//    // animation mobile
//    float mobileOffset = 0.0f; // offset courant (clip-space) appliqué à la barre mobile
//
//    
//
//    unsigned int shaderProgram = 0;
//    unsigned int VAO = 0, VBO = 0;            // pour toutes les barres statiques
//    unsigned int mobileVAO = 0, mobileVBO = 0; // pour la barre mobile (rouge)
//    int numBars = 0;
//
//    // Shaders
//    const char* vertexShaderSource = R"(
//        #version 330 core
//        layout(location = 0) in vec2 aPos;
//        uniform float uOffsetX;
//        void main() {
//            vec2 pos = aPos;
//            pos.x += uOffsetX;
//            gl_Position = vec4(pos, 0.0, 1.0);
//        }
//    )";
//
//    const char* fragmentShaderSource = R"(
//        #version 330 core
//        out vec4 FragColor;
//        uniform vec4 uColor;
//        void main() {
//            FragColor = uColor;
//        }
//    )";
//};
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
//    SortingVisualizer* visualizer = static_cast<SortingVisualizer*>(glfwGetWindowUserPointer(window));
//    if (visualizer) {
//       // glViewport(0, 0, width, height);
//        visualizer->setWindowSize(width, height);
//    }
//}
//
//
//// ---------------- constructor / destructor ----------------
//
//SortingVisualizer::SortingVisualizer(int size) : numBars(size) {
//    values.resize(size);
//    std::srand((unsigned) std::time(nullptr));
//    for (int i = 0; i < size; ++i) values[i] = (i + 1) / float(size);
//
//    // shuffle
//    for (int i = size - 1; i > 0; --i) {
//        int j = std::rand() % (i + 1);
//        std::swap(values[i], values[j]);
//    }
//
//    std::cout << "Tableau initial (pourcentage) : ";
//    for (float v : values) std::cout << int(v * 100) << " ";
//    std::cout << std::endl;
//}
//
//SortingVisualizer::~SortingVisualizer() {
//    if (shaderProgram) glDeleteProgram(shaderProgram);
//    if (VBO) glDeleteBuffers(1, &VBO);
//    if (VAO) glDeleteVertexArrays(1, &VAO);
//    if (mobileVBO) glDeleteBuffers(1, &mobileVBO);
//    if (mobileVAO) glDeleteVertexArrays(1, &mobileVAO);
//}
//void SortingVisualizer::setWindowSize(int w, int h) {
//    WIDTH = w;
//    HEIGHT = h;
//}
//
//void SortingVisualizer::initGL() {
//    if (!glfwInit()) { std::cerr << "GLFW init failed\n"; exit(-1); }
//
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//    window = glfwCreateWindow(WIDTH, HEIGHT, "Tri par insertion - Visualisation", nullptr, nullptr);
//    if (!window) { std::cerr << "Failed to create GLFW window\n"; glfwTerminate(); exit(-1); }
//
//    glfwMakeContextCurrent(window);
//
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
//        std::cerr << "Failed to initialize GLAD\n";
//        exit(-1);
//    }
//
//    glfwSetWindowUserPointer(window, this);                // ✅ associe ton objet C++ à la fenêtre
//    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  // ✅ callback sur resize
//
//    glClearColor(0.1f, 0.12f, 0.14f, 1.0f);
//
//    // --- ImGui init ---
//    IMGUI_CHECKVERSION();
//    ImGui::CreateContext();
//    ImGuiIO& io = ImGui::GetIO(); (void)io;
//    ImGui::StyleColorsDark();
//    ImGui_ImplGlfw_InitForOpenGL(window, true);
//    ImGui_ImplOpenGL3_Init("#version 330");
//
//    setupShaders();
//    setupBuffers();
//
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//}
//
//
//void SortingVisualizer::setupShaders() {
//    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
//    glShaderSource(vs, 1, &vertexShaderSource, nullptr);
//    glCompileShader(vs);
//    GLint ok = 0;
//    glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
//    if (!ok) {
//        char buf[1024]; glGetShaderInfoLog(vs, 1024, nullptr, buf);
//        std::cerr << "VS: " << buf << std::endl;
//    }
//
//    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(fs, 1, &fragmentShaderSource, nullptr);
//    glCompileShader(fs);
//    glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
//    if (!ok) { char buf[1024]; glGetShaderInfoLog(fs, 1024, nullptr, buf); std::cerr << "FS: " << buf << std::endl; }
//
//    shaderProgram = glCreateProgram();
//    glAttachShader(shaderProgram, vs);
//    glAttachShader(shaderProgram, fs);
//    glLinkProgram(shaderProgram);
//    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &ok);
//    if (!ok) { char buf[1024]; glGetProgramInfoLog(shaderProgram, 1024, nullptr, buf); std::cerr << "LINK: " << buf << std::endl; }
//
//    glDeleteShader(vs);
//    glDeleteShader(fs);
//}
//
//void SortingVisualizer::setupBuffers() {
//    // Build vertex data for all bars (VBO) - each bar: 6 vertices * 2 floats
//    std::vector<float> vertices;
//    vertices.reserve(numBars * 6 * 2);
//   // float barWidth = 2.0f / float(numBars);
//
//    float spacingFactor = 1.3f; // comme tu voulais : espace entre barres
//    float barWidth = 2.0f / ((numBars) * spacingFactor);
//
//    for (int i = 0; i < numBars; ++i) {
//        float h = values[i];
//        float left = -1.0f + i * barWidth * spacingFactor;
//        float right = left + barWidth;
//
//        vertices.insert(vertices.end(), {
//            left, -0.9f,  right, -0.9f,  right, -0.9f + h * 1.6f,
//            left, -0.9f,  right, -0.9f + h * 1.6f,  left, -0.9f + h * 1.6f 
//            });
//    }
//
//    // main VAO/VBO for static bars
//    glGenVertexArrays(1, &VAO);
//    glGenBuffers(1, &VBO);
//    glBindVertexArray(VAO);
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    // Reserve full size once (we will update with glBufferSubData)
//    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
//    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
//    glEnableVertexAttribArray(0);
//    glBindVertexArray(0);
//
//    // mobile VAO/VBO: create once, we'll update its data each frame
//    // initialize with the same shape as a single bar (placeholder)
//    float initialH = values[0];
//    std::vector<float> mobileVerts = {
//        0.0f, -0.9f,  barWidth, -0.9f,  barWidth, -0.9f + initialH * 1.6f,
//        0.0f, -0.9f,  barWidth, -0.9f + initialH * 1.6f,  0.0f, -0.9f + initialH * 1.6f
//    };
//    glGenVertexArrays(1, &mobileVAO);
//    glGenBuffers(1, &mobileVBO);
//    glBindVertexArray(mobileVAO);
//    glBindBuffer(GL_ARRAY_BUFFER, mobileVBO);
//    glBufferData(GL_ARRAY_BUFFER, mobileVerts.size() * sizeof(float), mobileVerts.data(), GL_DYNAMIC_DRAW);
//    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
//    glEnableVertexAttribArray(0);
//    glBindVertexArray(0);
//}
//
//// ==============================
//// Nouvelle fonction corrigée
//// ==============================
//void SortingVisualizer::updateBarVertices(int emptyIndex /*= -1*/) {
//    // On reconstruit le tableau vertices en respectant toujours la même taille
//    // (une entrée par barre) ; pour emptyIndex on écrit une barre de hauteur 0.
//    std::vector<float> vertices;
//    vertices.reserve(numBars * 6 * 2);
//
//   // float barWidth = 2.0f / float(numBars);
//
//   // float spacingFactor = 1.05f; // au lieu de 1.3f
//
//    float spacingFactor = 1.3f;
//    float barWidth = 2.0f / (numBars*spacingFactor);
//
//    for (int i = 0; i < numBars; ++i) {
//        float h;
//        if (i == emptyIndex) {
//            // trou visuel : hauteur 0 (ne casse pas l'indexation des barres)
//            h = 0.0f;
//          // h = currentValue;
//
//        }
//        else {
//            h = values[i];
//            // sécurité : si values contient -1 ou autre sentinel, clamp à 0..1
//            if (h < 0.0f) h = 0.0f;
//            if (h > 1.0f) h = 1.0f;
//        }
//
//        float left = -1.0f + i * barWidth * spacingFactor;
//        float right = left + barWidth;
//
//        // 2 triangles -> 6 sommets (x,y)
//        vertices.insert(vertices.end(), {
//            left, -0.9f,
//            right, -0.9f,
//            right, -0.9f + h * 1.6f,
//            left, -0.9f,
//            right, -0.9f + h * 1.6f,
//            left, -0.9f + h * 1.6f
//            });
//    }
//
//    // Mise à jour du VBO principal (taille identique à l'initialisation)
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//    // Mettre à jour le mobile bar VBO (hauteur = currentValue)
//    float barWidthLocal = 2.0f / float(numBars*spacingFactor);
//    float hmobile = currentValue;
//    if (hmobile < 0.0f) hmobile = 0.0f;
//    if (hmobile > 1.0f) hmobile = 1.0f;
//    std::vector<float> mobileVerts = {
//        0.0f, -0.9f,
//        barWidthLocal, -0.9f,
//        barWidthLocal, -0.9f + hmobile * 1.6f,
//        0.0f, -0.9f,
//        barWidthLocal, -0.9f + hmobile * 1.6f,
//        0.0f, -0.9f + hmobile * 1.6f
//    };
//    glBindBuffer(GL_ARRAY_BUFFER, mobileVBO);
//    glBufferSubData(GL_ARRAY_BUFFER, 0, mobileVerts.size() * sizeof(float), mobileVerts.data());
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//}
//
//// ---------------- rendering ----------------
//
//void SortingVisualizer::render() {
//
//  
//
//    glClear(GL_COLOR_BUFFER_BIT);
//    int display_w, display_h;
//    glfwGetFramebufferSize(window, &display_w, &display_h);
//    glViewport(0, 0, display_w, display_h);
//    glUseProgram(shaderProgram);
//    float spacingFactor = 1.3f;
//    float barWidth = 2.0f / float(numBars*spacingFactor);
//    float step = barWidth;
//    //float spacingFactor = 1.05f; // au lieu de 1.3f
//   // float spacingFactor = 1.3f;
//
//    // 1) draw base bars (VAO)
//    glBindVertexArray(VAO);
//    for (int i = 0; i < numBars; ++i) {
//        float r = 1.f, g = 1.f, b = 1.f, a = 1.f;
//       if (i == currentIndex) { r = 1.f; g = 0.2f; b = 0.2f; a = 0.35f; } // ghost red (semi)
//        //if (i == currentIndex) { r = 1.f; g = 0.2f; b = 0.2f; a = 1.0f;} // même rouge que la barre mobile}
//        else if (i < currentIndex) { r = 0.22f; g = 0.78f; b = 0.22f; a = 1.0f; } // sorted-green
//        else { r = 1.f; g = 1.f; b = 1.f; a = 1.0f; } // unsorted-white
//
//        glUniform4f(glGetUniformLocation(shaderProgram, "uColor"), r, g, b, a);
//        glUniform1f(glGetUniformLocation(shaderProgram, "uOffsetX"), 0.0f);
//        glDrawArrays(GL_TRIANGLES, i * 6, 6);
//    }
//    glBindVertexArray(0);
//
//    // 2) draw mobile bar on top (mobileVAO). mobileVBO already has height=currentValue
//    if (isComparing) {
//        glBindVertexArray(mobileVAO);
//        // find left base position of currentIndex to compute offset in clip-space:
//        float leftCurr = -1.0f + currentIndex * barWidth * spacingFactor;
//        // mobileVerts were built at x=[0..barWidthLocal], so we need to offset to leftCurr + mobileOffset
//        // compute offset relative to clip-space: leftCurr + mobileOffset
//        float totalOffset = leftCurr + mobileOffset;
//        glUniform4f(glGetUniformLocation(shaderProgram, "uColor"), 1.0f, 0.2f, 0.2f, 1.0f); // opaque red
//        glUniform1f(glGetUniformLocation(shaderProgram, "uOffsetX"), totalOffset);
//        glDrawArrays(GL_TRIANGLES, 0, 6);
//        glBindVertexArray(0);
//    }
//
//    // ---------- IMGUI ----------
//    //ImGui_ImplOpenGL3_NewFrame();
//    //ImGui_ImplGlfw_NewFrame();
//    //ImGui::NewFrame();
//
//    //// legend window bigger and fixed once
//    //ImGui::SetNextWindowSize(ImVec2(520, 260), ImGuiCond_Once);
//
//   /* ImGui::Begin("Legende et informations", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
//    ImGui::Text("Legende :");
//    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
//        "Rouge : element e inserer (i = %d, val = %.2f)", currentIndex, currentValue);
//    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f),
//        "Vert : element compare (j = %d, val = %.2f)", compareIndex, values[compareIndex]);
//    ImGui::End();*/
//
//    // ---------- IMGUI ----------
//    ImGui_ImplOpenGL3_NewFrame();
//    ImGui_ImplGlfw_NewFrame();
//    ImGui::NewFrame();
//
//    // legend window bigger and fixed once
//    ImGui::SetNextWindowSize(ImVec2(520, 260), ImGuiCond_Once);
//
//    ImGui::Begin("Legende et informations", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
//    ImGui::Text("Legende :");
//
//    //  Barre rouge : élément courant à insérer
//    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
//        "Rouge : element a inserer (i = %d, val = %.2f)", currentIndex, currentValue);
//
//    // Barre verte : élément en comparaison
//    if (compareIndex >= 0 && compareIndex < values.size()) {
//        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f),
//            "Vert : element compare (j = %d, val = %.2f)", compareIndex, values[compareIndex]);
//    }
//    else {
//        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f),
//            "Vert : element compare (aucune comparaison en cours)");
//    }
//
//    // Vert clair : éléments déjà triés
//    ImGui::TextColored(ImVec4(0.22f, 0.78f, 0.22f, 1.0f),
//        "Vert clair : elements deja tries (indices  j  = %d < i = %d)" , compareIndex,currentIndex);
//    // Barre rouge demon– emplacement temporaire
//    ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 0.35f),
//        "Rouge demon : repere la pos i  de elt a inserer i = %d) ",currentIndex);
//
//    // ⚪ Blanc : éléments non encore triés
//    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
//        "Blanc : elements non encore tries (indices,i >   %d)", currentIndex);
//
//
//    ImGui::End();
//
//
//
//    ImDrawList* draw_list = ImGui::GetForegroundDrawList();
//    for (int i = 0; i < numBars; ++i) {
//        float leftClip = -1.0f + i * step*spacingFactor;
//       // float barWidth = 2.0f / (numBars * spacingFactor);
//        float centerClip = leftClip + (barWidth/2.0f);
//        float xPixel = (centerClip * 0.5f + 0.5f) * float(WIDTH);
//        float yPixel = float(display_h) - 18.0f;
//
//       // float yPixel = float(HEIGHT) - 18.0f;
//        draw_list->AddText(ImVec2(xPixel - 6, yPixel), IM_COL32(255, 255, 255, 255), std::to_string(i).c_str());
//    }
//
//    ImGui::Render();
//    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//
//    glfwSwapBuffers(window);
//}
//
//void SortingVisualizer::insertionSortStep() {
//    if (sortingDone) return;
//    if (currentIndex >= numBars) { sortingDone = true; isComparing = false; return; }
//
//    currentValue = values[currentIndex]; // élément rouge
//    float key = currentValue;
//    int j = currentIndex - 1;
//    mobileOffset = 0.0f;
//    float spacingFactor = 1.3f;
//    float barWidth = 2.0f / float(numBars*spacingFactor);
//    float step = barWidth * 1.3f;
//
//    isComparing = false;
//    compareIndex = -1+1;
//    updateBarVertices();
//    render();
//    std::this_thread::sleep_for(std::chrono::milliseconds(500));
//
//    int emptyIndex = currentIndex; // position du trou initial (au début le trou est là où est l'élément rouge)
//
//    while (j >= 0 && values[j] > key) {
//        compareIndex = j;
//        isComparing = true;
//
//        // on veut que la case laissée vide soit la position j (la valeur de j est déplacée vers j+1)
//        emptyIndex = j;
//
//        // Animation du déplacement de la barre rouge
//        float targetOffset = (compareIndex - currentIndex) * step;
//        const int stepsAnim = 20;
//        const int ms_per_step = 150;
//        float startOffset = mobileOffset;
//
//        for (int s = 1; s <= stepsAnim; ++s) {
//            float t = float(s) / float(stepsAnim);
//            mobileOffset = startOffset + (targetOffset - startOffset) * t;
//            updateBarVertices(emptyIndex+1); // on indique la position vide
//            render();
//            std::this_thread::sleep_for(std::chrono::milliseconds(ms_per_step));
//        }
//
//        // Décalage des valeurs : la valeur de j est copiée vers j+1
//        values[j + 1] = values[j];
//
//        // update en conservant la case j comme vide
//        updateBarVertices(emptyIndex+1);
//        render();
//        std::this_thread::sleep_for(std::chrono::milliseconds(300));
//
//        j--;
//    }
//
//    // placer la clé dans le trou final (emptyIndex contient l'indice du trou)
//    values[emptyIndex] = key;
//
//    // Animation finale d’insertion (mobile se déplace vers le trou final)
//    float finalTargetOffset = ((emptyIndex)-currentIndex) * step;
//    {
//        const int stepsAnim2 = 20;
//        const int ms_per_step2 = 100;
//        float startOffset = mobileOffset;
//        for (int s = 1; s <= stepsAnim2; ++s) {
//            float t = float(s) / float(stepsAnim2);
//            mobileOffset = startOffset + (finalTargetOffset - startOffset) * t;
//            updateBarVertices(emptyIndex);
//            render();
//            std::this_thread::sleep_for(std::chrono::milliseconds(ms_per_step2));
//        }
//    }
//
//    isComparing = false;
//    mobileOffset = 0.0f;
//    updateBarVertices(); // plus de trou visible
//    render();
//    std::this_thread::sleep_for(std::chrono::seconds(1));
//
//    currentIndex++;
//}
//
//// ---------------- run ----------------
//
//void SortingVisualizer::run() {
//    initGL();
//    while (!glfwWindowShouldClose(window)) {
//        glfwPollEvents();
//        if (!sortingDone) insertionSortStep();
//        else { render(); std::this_thread::sleep_for(std::chrono::milliseconds(100)); }
//    }
//
//    ImGui_ImplOpenGL3_Shutdown();
//    ImGui_ImplGlfw_Shutdown();
//    ImGui::DestroyContext();
//    glfwDestroyWindow(window);
//    glfwTerminate();
//}
//
//// ---------------- main ----------------
//
//int main() {
//    SortingVisualizer vis(20);
//    vis.run();
//    return 0;
//}
//
//

