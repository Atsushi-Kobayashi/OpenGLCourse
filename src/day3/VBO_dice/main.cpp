
#include <cstdio>
#include <cmath>
#include <vector>
#include <string>

#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "common.h"

static int WIN_WIDTH   = 500;                       // ウィンドウの幅
static int WIN_HEIGHT  = 500;                       // ウィンドウの高さ
static const char *WIN_TITLE = "OpenGL Course";     // ウィンドウのタイトル

static const double PI = 4.0 * atan(1.0);           // 円周率の定義

static float theta = 0.0f;

static const std::string TEX_FILE = std::string(DATA_DIRECTORY) + "dice.png";
static GLuint textureId = 0u;
static bool enableMipmap = true;

// 頂点オブジェクト
struct Vertex {
    Vertex(const glm::vec3 &position_, const glm::vec2 &tex_)
        : position(position_)
        , tex(tex_) {
    }

    glm::vec3 position;
    glm::vec2 tex;  //texture上のuv座標
};

static const glm::vec3 positions[8] = {
    glm::vec3{ -1.0f, -1.0f, -1.0f },
    glm::vec3{  1.0f, -1.0f, -1.0f },
    glm::vec3{ -1.0f,  1.0f, -1.0f },
    glm::vec3{ -1.0f, -1.0f,  1.0f },
    glm::vec3{  1.0f,  1.0f, -1.0f },
    glm::vec3{ -1.0f,  1.0f,  1.0f },
    glm::vec3{  1.0f, -1.0f,  1.0f },
    glm::vec3{  1.0f,  1.0f,  1.0f }
};

static const glm::vec2 texcoords[14] = {
    glm::vec2(0.0f,  1/3.0f),
    glm::vec2(0.0f,  2/3.0f),

    glm::vec2(0.25f,  0.0f),
    glm::vec2(0.25f,  1/3.0f),
    glm::vec2(0.25f,  2/3.0f),
    glm::vec2(0.25f, 1.0f),

    glm::vec2(0.5f,  0.0f),
    glm::vec2(0.5f,  1/3.0f),
    glm::vec2(0.5f,  2/3.0f),
    glm::vec2(0.5f, 1.0f),

    glm::vec2(0.75f,  1/3.0f),
    glm::vec2(0.75f,  2/3.0f),

    glm::vec2(1.0f,  1/3.0f),
    glm::vec2(1.0f,  2/3.0f)
};
static const unsigned int faces[12][3] = {
    { 1, 6, 7 }, { 1, 7, 4 },
    { 2, 5, 7 }, { 2, 7, 4 },
    { 3, 5, 7 }, { 3, 7, 6 },
    { 0, 1, 4 }, { 0, 4, 2 },
    { 0, 1, 6 }, { 0, 6, 3 },
    { 0, 2, 5 }, { 0, 5, 3 }
};

static const unsigned int indices_tex[12][3] = {
    { 12, 13, 11 }, { 12, 11, 10 },
    {  7, 8, 11 }, { 7, 11, 10 },
    {  4, 8, 9 }, { 4, 9, 5 },
    { 3, 2, 6 }, { 3, 6, 7 },
    { 3, 0, 1 }, { 3, 1, 4 },
    { 3, 7, 8 }, { 3, 8, 4 }
};

// バッファを参照する番号
GLuint vertexBufferId;
GLuint indexBufferId;

// OpenGLの初期化関数
void initializeGL() {
    // 深度テストの有効化
    glEnable(GL_DEPTH_TEST);

    // 背景色の設定 (黒)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // テクスチャの有効化
    glEnable(GL_TEXTURE_2D);
    
    // テクスチャの設定
    int texWidth, texHeight, channels;
    unsigned char *bytes = stbi_load(TEX_FILE.c_str(), &texWidth, &texHeight, &channels, STBI_rgb_alpha);
    if (!bytes) {
        fprintf(stderr, "Failed to load image file: %s\n", TEX_FILE.c_str());
        exit(1);
    }


    // 頂点配列の作成
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    int idx = 0;
    for (int face = 0; face < 6; face++) {
        for (int i = 0; i < 3; i++) {
            vertices.push_back(Vertex(positions[faces[face * 2 + 0][i]], texcoords[indices_tex[face*2+0][i]]));
            indices.push_back(idx++);
        }

        for (int i = 0; i < 3; i++) {
            vertices.push_back(Vertex(positions[faces[face * 2 + 1][i]], texcoords[indices_tex[face*2+1][i]]));
            indices.push_back(idx++);
        }
    }

    // 頂点バッファの作成
    glGenBuffers(1, &vertexBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(),
                 vertices.data(), GL_STATIC_DRAW);

    // 頂点番号バッファの作成
    glGenBuffers(1, &indexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
                 indices.data(), GL_STATIC_DRAW);
    
    // 単純なテクスチャの転送
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texWidth, texHeight,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
    
    // テクスチャの画素値参照方法の設定
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    // テクスチャ境界の折り返し設定
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // テクスチャの無効化
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // ロードした画素情報の破棄
    stbi_image_free(bytes);
}

// OpenGLの描画関数
void paintGL() {
    // 背景色の描画
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 座標の変換
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 1000.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(3.0f, 4.0f, 5.0f,     // 視点の位置
              0.0f, 0.0f, 0.0f,     // 見ている先
              0.0f, 1.0f, 0.0f);    // 視界の上方向

    
    glRotatef(theta, 1.0f, -1.0f, 1.0f);  // thetaだけ回転

    // 頂点バッファの有効化
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, position));

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, tex));

    // 三角形の描画
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // 頂点バッファの無効化
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void resizeGL(GLFWwindow *window, int width, int height) {
    // ユーザ管理のウィンドウサイズを変更
    WIN_WIDTH = width;
    WIN_HEIGHT = height;
    
    // GLFW管理のウィンドウサイズを変更
    glfwSetWindowSize(window, WIN_WIDTH, WIN_HEIGHT);

    // 実際のウィンドウサイズ (ピクセル数) を取得
    int renderBufferWidth, renderBufferHeight;
    glfwGetFramebufferSize(window, &renderBufferWidth, &renderBufferHeight);

    // ビューポート変換の更新
    glViewport(0, 0, renderBufferWidth, renderBufferHeight);
}


// アニメーションのためのアップデート
void animate() {
    theta += 1.5f;  // 1.5度だけ回転
}


int main(int argc, char **argv) {
    // OpenGLを初期化する
    if (glfwInit() == GL_FALSE) {
        fprintf(stderr, "Initialization failed!\n");
        return 1;
    }

    // Windowの作成
    GLFWwindow *window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE,
                                          NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Window creation failed!");
        glfwTerminate();
        return 1;
    }

    // OpenGLの描画対象にWindowを追加
    glfwMakeContextCurrent(window);

    // OpenGL 3.x/4.xの関数をロードする (glfwMakeContextCurrentの後でないといけない)
    const int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        fprintf(stderr, "Failed to load OpenGL 3.x/4.x libraries!\n");
        return 1;
    }

    // バージョンを出力する
    printf("Load OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

    // OpenGLを初期化
    initializeGL();

    // ウィンドウのリサイズを扱う関数の登録
    glfwSetWindowSizeCallback(window, resizeGL);

    // メインループ
    while (glfwWindowShouldClose(window) == GL_FALSE) {
        // 描画
        paintGL();

        // アニメーション
        animate();

        // 描画用バッファの切り替え
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
