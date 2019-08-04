
#include <cstdio>
#include <cmath>
#include <string>

#define GLFW_INCLUDE_GLU  // GLUライブラリを使用するのに必要
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "common.h"

static int WIN_WIDTH   = 500;                 // ウィンドウの幅
static int WIN_HEIGHT  = 500;                 // ウィンドウの高さ
static const char *WIN_TITLE = "OpenGL Course";     // ウィンドウのタイトル

static const double PI = 4.0 * atan(1.0);           // 円周率の定義

static float theta = 0.0f;

static const std::string TEX_FILE = std::string(DATA_DIRECTORY) + "dice.png";
static GLuint textureId = 0u;
static bool enableMipmap = true;

static const float positions[8][3] = {
    { -1.0f, -1.0f, -1.0f },
    {  1.0f, -1.0f, -1.0f },
    { -1.0f,  1.0f, -1.0f },
    { -1.0f, -1.0f,  1.0f },
    {  1.0f,  1.0f, -1.0f },
    { -1.0f,  1.0f,  1.0f },
    {  1.0f, -1.0f,  1.0f },
    {  1.0f,  1.0f,  1.0f }
};

static const unsigned int indices[12][3] = {
    { 1, 6, 7 }, { 1, 7, 4 },
    { 2, 5, 7 }, { 2, 7, 4 },
    { 3, 5, 7 }, { 3, 7, 6 },
    { 0, 1, 4 }, { 0, 4, 2 },
    { 0, 1, 6 }, { 0, 6, 3 },
    { 0, 2, 5 }, { 0, 5, 3 }
};

static const unsigned int indices_texcoords[12][3] = {
    { 12,13,11}, {12,11,10},
    { 7, 8, 11 }, { 7, 11, 10 },
    { 4, 8, 9 }, { 4, 9, 5 },
    { 3, 2, 6 }, { 3, 6, 7 },
    { 3, 0, 1 }, { 3, 1, 4 },
    { 3, 7, 8 }, { 3, 8, 4 }
};


static const float texcoords[14][2] = {
    {  0.0f,  1/3.0f},
    {  0.0f,  2/3.0f},
    
    {  0.25f,  0.0f },
    {  0.25f,  1/3.0f},
    {  0.25f,  2/3.0f },
    { 0.25f, 1.0f},
    
    {  0.5f,  0.0f },
    {  0.5f,  1/3.0f},
    {  0.5f,  2/3.0f },
    { 0.5f, 1.0f},
    
    {  0.75f,  1/3.0f},
    {  0.75f,  2/3.0f},

    {  1.0f,  1/3.0f},
    {  1.0f,  2/3.0f},
};

// OpenGLの初期化関数
void initializeGL() {
    // 背景色の設定 (黒)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    // 深度テストの有効化
    glEnable(GL_DEPTH_TEST);
    
    // テクスチャの有効化
    glEnable(GL_TEXTURE_2D);
    
    // テクスチャの設定
    int texWidth, texHeight, channels;
    unsigned char *bytes = stbi_load(TEX_FILE.c_str(), &texWidth, &texHeight, &channels, STBI_rgb_alpha);
    if (!bytes) {
        fprintf(stderr, "Failed to load image file: %s\n", TEX_FILE.c_str());
        exit(1);
    }
    
    // テクスチャの生成と有効化
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    
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
    // 背景色と深度値のクリア
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
    
    // 立方体の描画
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    glBegin(GL_TRIANGLES);
    
    for (int face = 0; face < 6; face++) {
        
        for (int i = 0; i < 3; i++) {
        glTexCoord2fv(texcoords[indices_texcoords[face*2+0][i]]);
            glVertex3fv(positions[indices[face*2+0][i]]);
        }
        for (int i = 0; i < 3; i++) {
            glTexCoord2fv(texcoords[indices_texcoords[face*2+1][i]]);
            glVertex3fv(positions[indices[face*2+1][i]]);
        }
    }
    
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
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
    
    // ウィンドウのリサイズを扱う関数の登録
    glfwSetWindowSizeCallback(window, resizeGL);
    
    // OpenGLを初期化
    initializeGL();
    
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
