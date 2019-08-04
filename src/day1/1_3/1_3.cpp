//
//  main.cpp
//  1_2
//
//  Created by Takashi Oya on 2019/04/13.
//  Copyright © 2019年 Takashi Oya. All rights reserved.
//

#include <cstdio>
#include <GLFW/glfw3.h>

static const int WIN_WIDTH   = 500;                 // ウィンドウの幅
static const int WIN_HEIGHT  = 500;                 // ウィンドウの高さ
static const char *WIN_TITLE = "OpenGL Course";     // ウィンドウのタイトル

// ユーザ定義のOpenGLの初期化
void initializeGL() {
    // 背景色の設定
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
}

// ユーザ定義のOpenGL描画
void paintGLEven() {
    // 背景色の描画
    glDrawBuffer(GL_FRONT);
    glClear(GL_COLOR_BUFFER_BIT);
}

void paintGLOdd() {
    // 背景色の描画
    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT);
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
    
    // 初期化
    initializeGL();
    int i=0;
    // メインループ
    while (glfwWindowShouldClose(window) == GL_FALSE) {
        
        if(i%2==0){
            // 描画
            paintGLEven();
            
            // 描画用バッファの切り替え
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        else{
            paintGLOdd();
            
            // 描画用バッファの切り替え
            glfwSwapBuffers(window);
            glfwPollEvents();
            
        }
        ++i;
        
    }
}
