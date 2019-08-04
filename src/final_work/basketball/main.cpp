#include <cmath>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

// ディレクトリの設定ファイル
#include "common.h"

int game_step = 0;
int shoot_flag = 0;
int goal_flag = 0;
int shooting_count = 0;
int goal_count = 0;
int camera_flag = 0; // 1になった時刻以降は視点位置を固定
int ball_bound_count = 0;
int click_wait = 0;
int cheat_flag = 0; // デモ用のチートモードのための変数, シュート時右クリックすることでチートモード有効化

static int WIN_WIDTH = 1200; // ウィンドウの幅
static int WIN_HEIGHT = 800; // ウィンドウの高さ
static char *WIN_TITLE = "Basketball";

static const double PI = 4.0 * std::atan(1.0);

// シェーダファイル
static const std::string VERT_SHADER_FILE = std::string(SHADER_DIRECTORY) + "obj.vert";
static const std::string FRAG_SHADER_FILE = std::string(SHADER_DIRECTORY) + "obj.frag";

static const std::string VERT_SHADER_FILE_PO = std::string(SHADER_DIRECTORY) + "plain.vert";
static const std::string FRAG_SHADER_FILE_PO = std::string(SHADER_DIRECTORY) + "plain.frag";

// モデルのファイル
static const std::string OBJECT_FILE_b = std::string(DATA_DIRECTORY) + "/NBA_BASKETBALL/NBA_BASKETBALL.obj";
static std::string TEX_FILE_b = std::string(DATA_DIRECTORY) + "/NBA_BASKETBALL/NBA_BASKETBALL_DIFFUSE.tga";

static const std::string OBJECT_FILE_g = std::string(DATA_DIRECTORY) + "/basketball_goal/10488_basketball_goal_L3.obj";
static std::string TEX_FILE_g = std::string(DATA_DIRECTORY) + "/basketball_goal/10488_basketball_goal_diffuse_v1.jpg";

static const std::string OBJECT_FILE_c = std::string(DATA_DIRECTORY) + "/basketball_court/basketball_court.obj";
static std::string TEX_FILE_c = std::string(DATA_DIRECTORY) + "/basketball_court/basketball_court.jpg";

static const std::string OBJECT_FILE_bg = std::string(DATA_DIRECTORY) + "/background/blue_sky.obj";
static std::string TEX_FILE_bg = std::string(DATA_DIRECTORY) + "/background/blue_sky.jpg";

static const std::string OBJECT_FILE_w = std::string(DATA_DIRECTORY) + "/wall/gym_wall.obj";
static std::string TEX_FILE_w = std::string(DATA_DIRECTORY) + "/wall/wall_05_m.png";

static std::string TEX_FILE_t = std::string(DATA_DIRECTORY) + "/moji/title.png";
static std::string TEX_FILE_cts = std::string(DATA_DIRECTORY) + "/moji/cts.png";
static std::string TEX_FILE_gl = std::string(DATA_DIRECTORY) + "/moji/goal_02.png";

static std::string TEX_FILE_go = std::string(DATA_DIRECTORY) + "/moji/GO_01.png";
static std::string TEX_FILE_gc = std::string(DATA_DIRECTORY) + "/moji/game_clear.png";
static std::string TEX_FILE_sc = std::string(DATA_DIRECTORY) + "/moji/SC_01.png";
static std::string TEX_FILE_s1 = std::string(DATA_DIRECTORY) + "/moji/stage_01_img.png";
static std::string TEX_FILE_s2 = std::string(DATA_DIRECTORY) + "/moji/stage_02_img.png";
static std::string TEX_FILE_s3 = std::string(DATA_DIRECTORY) + "/moji/stage_03_img.png";

// シェーディングのための情報
static const glm::vec3 lightPos = glm::vec3(0.0f, 7.0f, 0.0f);
// static const glm::vec3 diffColor = glm::vec3(0.75164f, 0.60648f, 0.22648f);
static const glm::vec3 specColor = glm::vec3(0.393548, 0.271906, 0.166721);
static const glm::vec3 ambiColor = glm::vec3(0.2125, 0.1275, 0.054);
static const float roughness = 0.09f;

double current_time = 0.0;
double shooting_time = 0.0;
double shooting_total_time = 0.0;
float grav_a = -9.8f;
const float rst_e = 0.8f;
const float ball_r = 0.12f;
// ボールの初期位置
glm::vec3 front_3p_pos = glm::vec3(4.5f, 2.5f, 0.037f);
glm::vec3 free_throw_pos = glm::vec3(6.8f, 2.5f, 0.037f);
glm::vec3 center_line_pos = glm::vec3(0.0f, 2.5f, 0.037f);

glm::vec3 shoot_vel = glm::vec3(0.0f, 0.0f, 0.0f);

const float ring_r = 0.2f;
const double ring_kindness = 0.02;
const glm::vec3 ring_c_pos = glm::vec3(11.851f, 3.13f, 0.037f);

// ボードの位置に関するもの
const glm::vec3 board_LL_p = glm::vec3(12.2f, 3.01f, -0.683f);
const glm::vec3 board_size = glm::vec3(0.1f, 1.04f, 1.43f);

glm::mat4 viewMat = glm::lookAt(glm::vec3(2.5f, 2.0f, 0.001f), // 視点の位置
                                glm::vec3(11.0f, 3.0f, 0.0f),  // 見ている先
                                glm::vec3(0.0f, 1.0f, 0.0f));  // 視界の上方向

// free throw
const glm::mat4 viewMat_01 = glm::lookAt(glm::vec3(5.5f, 2.1f, 0.001f), // 視点の位置
                                         glm::vec3(11.8f, 3.0f, 0.0f),  // 見ている先
                                         glm::vec3(0.0f, 1.0f, 0.0f));  // 視界の上方向
// 3P
const glm::mat4 viewMat_02 = glm::lookAt(glm::vec3(3.5f, 2.1f, 0.001f), // 視点の位置
                                         glm::vec3(11.8f, 3.0f, 0.0f),  // 見ている先
                                         glm::vec3(0.0f, 1.0f, 0.0f));  // 視界の上方向
// center line
const glm::mat4 viewMat_03 = glm::lookAt(glm::vec3(-1.5f, 2.1f, 0.001f), // 視点の位置
                                         glm::vec3(11.8f, 3.0f, 0.0f),   // 見ている先
                                         glm::vec3(0.0f, 1.0f, 0.0f));   // 視界の上方向

// 以下マウス操作のための変数定義
// Arcballコントロールのための変数
bool isDragging = false;

enum ArcballMode
{
    ARCBALL_MODE_NONE = 0x00,
    ARCBALL_MODE_TRANSLATE = 0x01,
    ARCBALL_MODE_ROTATE = 0x02,
    ARCBALL_MODE_SCALE = 0x04
};

int arcballMode = ARCBALL_MODE_NONE;

glm::vec3 gravity;

glm::ivec2 oldPos;
glm::ivec2 newPos;

struct Vertex
{
    Vertex(const glm::vec3 &position_, const glm::vec3 &normal_, const glm::vec2 &tex_)
        : position(position_), normal(normal_), tex(tex_)
    {
    }

    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex;
};

GLuint compileShader(const std::string &filename, GLuint type)
{
    // シェーダの作成
    GLuint shaderId = glCreateShader(type);

    // ファイルの読み込み
    std::ifstream reader;
    size_t codeSize;
    std::string code;

    // ファイルを開く
    reader.open(filename.c_str(), std::ios::in);
    if (!reader.is_open())
    {
        // ファイルを開けなかったらエラーを出して終了
        fprintf(stderr, "Failed to load a shader: %s\n", VERT_SHADER_FILE.c_str());
        exit(1);
    }

    // ファイルをすべて読んで変数に格納 (やや難)
    reader.seekg(0, std::ios::end);  // ファイル読み取り位置を終端に移動
    codeSize = reader.tellg();       // 現在の箇所(=終端)の位置がファイルサイズ
    code.resize(codeSize);           // コードを格納する変数の大きさを設定
    reader.seekg(0);                 // ファイルの読み取り位置を先頭に移動
    reader.read(&code[0], codeSize); // 先頭からファイルサイズ分を読んでコードの変数に格納

    // ファイルを閉じる
    reader.close();

    // コードのコンパイル
    const char *codeChars = code.c_str();
    glShaderSource(shaderId, 1, &codeChars, NULL);
    glCompileShader(shaderId);

    // コンパイルの成否を判定する
    GLint compileStatus;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE)
    {
        // コンパイルが失敗したらエラーメッセージとソースコードを表示して終了
        fprintf(stderr, "Failed to compile a shader!\n");

        // エラーメッセージの長さを取得する
        GLint logLength;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0)
        {
            // エラーメッセージを取得する
            GLsizei length;
            std::string errMsg;
            errMsg.resize(logLength);
            glGetShaderInfoLog(shaderId, logLength, &length, &errMsg[0]);

            // エラーメッセージとソースコードの出力
            fprintf(stderr, "[ ERROR ] %s\n", errMsg.c_str());
            fprintf(stderr, "%s\n", code.c_str());
        }
        exit(1);
    }

    return shaderId;
}

GLuint buildShaderProgram(const std::string &vShaderFile, const std::string &fShaderFile)
{
    // シェーダの作成
    GLuint vertShaderId = compileShader(vShaderFile, GL_VERTEX_SHADER);
    GLuint fragShaderId = compileShader(fShaderFile, GL_FRAGMENT_SHADER);

    // シェーダプログラムのリンク
    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertShaderId);
    glAttachShader(programId, fragShaderId);
    glLinkProgram(programId);

    // リンクの成否を判定する
    GLint linkState;
    glGetProgramiv(programId, GL_LINK_STATUS, &linkState);
    if (linkState == GL_FALSE)
    {
        // リンクに失敗したらエラーメッセージを表示して終了
        fprintf(stderr, "Failed to link shaders!\n");

        // エラーメッセージの長さを取得する
        GLint logLength;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0)
        {
            // エラーメッセージを取得する
            GLsizei length;
            std::string errMsg;
            errMsg.resize(logLength);
            glGetProgramInfoLog(programId, logLength, &length, &errMsg[0]);

            // エラーメッセージを出力する
            fprintf(stderr, "[ ERROR ] %s\n", errMsg.c_str());
        }
        exit(1);
    }

    // シェーダを無効化した後にIDを返す
    glUseProgram(0);
    return programId;
}

void resizeGL(GLFWwindow *window, int width, int height)
{
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

struct VertexArrayObject
{
    VertexArrayObject(const std::string &filename, const std::string &tex_filename, const double obj_scale = 1.0)
        : filename(filename), tex_filename(tex_filename), obj_scale(obj_scale) {}

    const std::string filename;
    const std::string tex_filename;

    // インデックスバッファのサイズ (glDrawElementsで使用)
    size_t indexBufferSize;

    // バッファを参照する番号
    GLuint vaoId;
    GLuint vertexBufferId;
    GLuint indexBufferId;

    // シェーダを参照する番号
    GLuint programId;

    GLuint textureId;

    glm::mat4 modelMat = glm::mat4(1.0f);
    glm::mat4 projMat = glm::perspective(45.0f,
                                         (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 1000.0f);
    glm::vec3 vel = glm::vec3(0.0f, 0.0f, 0.0f);
    float theta = 0.0f;
    const double obj_scale;

    void initVao();
    void drawVao();
    void animate();
};

void VertexArrayObject::initVao()
{
    // 深度テストの有効化
    glEnable(GL_DEPTH_TEST);

    // 背景色の設定 (黒)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // モデルのロード
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;
    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str());
    if (!err.empty())
    {
        std::cerr << "[WARNING] " << err << std::endl;
    }

    if (!success)
    {
        std::cerr << "Failed to load OBJ file: " << filename << std::endl;
        exit(1);
    }

    // Vertex配列の作成
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    for (int s = 0; s < shapes.size(); s++)
    {
        const tinyobj::mesh_t &mesh = shapes[s].mesh;
        for (int i = 0; i < mesh.indices.size(); i++)
        {
            const tinyobj::index_t &index = mesh.indices[i];

            glm::vec3 position, normal;
            glm::vec2 texcoord;

            if (index.vertex_index >= 0)
            {
                position = glm::vec3(attrib.vertices[index.vertex_index * 3 + 0] * obj_scale,
                                     attrib.vertices[index.vertex_index * 3 + 1] * obj_scale,
                                     attrib.vertices[index.vertex_index * 3 + 2] * obj_scale);
            }

            if (index.normal_index >= 0)
            {
                normal = glm::vec3(attrib.normals[index.normal_index * 3 + 0],
                                   attrib.normals[index.normal_index * 3 + 1],
                                   attrib.normals[index.normal_index * 3 + 2]);
            }
            if (index.texcoord_index >= 0)
            {
                texcoord = glm::vec2(attrib.texcoords[index.texcoord_index * 2 + 0],
                                     1.0 - attrib.texcoords[index.texcoord_index * 2 + 1]); // 注意！！！！！
            }

            const unsigned int vertexIndex = vertices.size();
            vertices.push_back(Vertex(position, normal, texcoord));
            indices.push_back(vertexIndex);
        }
    }

    // VAOの作成
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);

    // 頂点バッファの作成
    glGenBuffers(1, &vertexBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    // 頂点バッファの有効化
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tex));

    // 頂点番号バッファの作成
    glGenBuffers(1, &indexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
                 indices.data(), GL_STATIC_DRAW);

    // 頂点バッファのサイズを変数に入れておく
    indexBufferSize = indices.size();

    // VAOをOFFにしておく
    glBindVertexArray(0);

    // initShaders
    programId = buildShaderProgram(VERT_SHADER_FILE, FRAG_SHADER_FILE);

    // initTexture(テクスチャの設定)
    int texWidth, texHeight, channels;
    unsigned char *bytes = stbi_load(tex_filename.c_str(), &texWidth, &texHeight, &channels, STBI_rgb_alpha);

    if (!bytes)
    {
        fprintf(stderr, "Failed to load image file: %s\n", tex_filename.c_str());
        exit(1);
    }

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(bytes);
}

void VertexArrayObject::animate()
{
    theta += 3.0f;

    // 以下平行移動
    // 前フレームからの経過時間を計算
    double tmp_time = glfwGetTime();     // 現在時刻(seconds)を取得
    double dt = tmp_time - current_time; // 前フレームからの経過時間を取得
    // 現在時刻の更新
    current_time = tmp_time;

    // 新しい速度を計算(重力加速度あり、空気抵抗なし)
    vel += glm::vec3(0.0f, grav_a * dt, 0.0f);

    // 速度による変位を計算
    glm::vec3 dsp = glm::vec3(vel.x * dt, vel.y * dt, vel.z * dt);

    // 壁などとの衝突判定のためにボール中心の現在位置を取得
    glm::vec4 c_pos = glm::translate(modelMat, dsp) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    if (c_pos.y - ball_r < 0.0f)
    {
        vel = glm::vec3(vel.x, abs(vel.y) * rst_e, vel.z);
        // めり込み対策に位置をずらしておく
        dsp.y -= c_pos.y - ball_r;
        ball_bound_count++;
    }
    if (c_pos.z + ball_r > 9.0f)
    {
        vel = glm::vec3(vel.x, vel.y, -abs(vel.z) * rst_e);
        // めり込み対策に位置をずらしておく
        dsp.z += 9.0f - (c_pos.z + ball_r);
        ball_bound_count += 1;
    }
    if (c_pos.z - ball_r < -9.0f)
    {
        vel = glm::vec3(vel.x, vel.y, abs(vel.z) * rst_e);
        // めり込み対策に位置をずらしておく
        dsp.z += -9.0f - (c_pos.z - ball_r);
        ball_bound_count += 1;
    }

    if (c_pos.x + ball_r > 15.0f)
    {
        vel = glm::vec3(-abs(vel.x) * rst_e, vel.y, vel.z);
        // めり込み対策に位置をずらしておく
        dsp.x += 15.0f - (c_pos.x + ball_r);
        ball_bound_count += 1;
    }
    if (c_pos.x - ball_r < -15.0f)
    {
        vel = glm::vec3(abs(vel.x) * rst_e, vel.y, vel.z);
        // めり込み対策に位置をずらしておく
        dsp.x += -15.0f - (c_pos.x - ball_r);
        ball_bound_count += 1;
    }

    // リングとの衝突判定
    // ボール中心とリング中心の距離
    float x_dst = c_pos.x - ring_c_pos.x; // dstとしてしまったが符号つき...
    float y_dst = c_pos.y - ring_c_pos.y;
    float z_dst = c_pos.z - ring_c_pos.z;
    float x_sq_dst = (c_pos.x - ring_c_pos.x) * (c_pos.x - ring_c_pos.x);
    float z_sq_dst = (c_pos.z - ring_c_pos.z) * (c_pos.z - ring_c_pos.z);
    float xz_sq_dst = x_sq_dst + z_sq_dst;

    // 最外の円柱に入っているか判定
    if (xz_sq_dst <= (ball_r + ring_r) * (ball_r + ring_r) && abs(y_dst) <= ball_r)
    {
        if (y_dst >= -0.0f && goal_flag == 0)
        {
            goal_flag = 1;
            printf("goal flag 1!\n");
        }

        // 救済措置
        dsp.x -= ring_kindness * x_dst;
        dsp.z -= ring_kindness * z_dst;

        // 最内の円柱にいないか判定(入ってたら当たってないのでスルー)
        if (xz_sq_dst >= (ring_r - ball_r) * (ring_r - ball_r))
        {
            //パターン1or2
            // すでにゴールに入った(goal flag==2)場合はもう上側へのバウンドは起こさない
            if (y_dst >= -0.5f && goal_flag != 2)
            {
                // パターン1(上外側)
                if (xz_sq_dst > ring_r * ring_r)
                {
                    vel = glm::vec3(abs(vel.x) * x_dst * 2.0f, abs(vel.y) * rst_e, abs(vel.z) * z_dst * 2.0f);
                    printf("Ling_1!!\n");
                    // めり込み対策に位置をずらしておく
                    if (x_dst >= 0)
                    {
                        dsp.x -= (x_dst - (ball_r + ring_r)) * x_dst;
                    }
                    else if (x_dst < 0)
                    {
                        dsp.x += (x_dst + (ball_r + ring_r)) * x_dst;
                    }
                    if (z_dst >= 0)
                    {
                        dsp.z -= (z_dst - (ball_r + ring_r)) * z_dst;
                    }
                    else if (z_dst < 0)
                    {
                        dsp.z += (z_dst + (ball_r + ring_r)) * z_dst;
                    }
                }
                // パターン2(上内側)
                else if (xz_sq_dst <= ring_r * ring_r)
                {
                    vel = glm::vec3(-abs(vel.x) * x_dst * 2.0f, abs(vel.y) * rst_e, -abs(vel.z) * z_dst * 2.0f);
                    printf("Ling_2!!\n");
                    // めり込み対策に位置をずらしておく
                    if (x_dst >= 0)
                    {
                        dsp.x -= (x_dst - (ring_r - ball_r)) * x_dst * 0.5f;
                    }
                    else if (x_dst < 0)
                    {
                        dsp.x += (x_dst + (ring_r - ball_r)) * x_dst * 0.5f;
                    }
                    if (z_dst >= 0)
                    {
                        dsp.z -= (z_dst - (ring_r - ball_r)) * z_dst * 0.5f;
                    }
                    else if (z_dst < 0)
                    {
                        dsp.z += (z_dst + (ring_r - ball_r)) * z_dst * 0.5f;
                    }
                }
                // めり込み対策に位置をずらしておく
                dsp.y += y_dst - ball_r;
            }
            //パターン3 or 4
            else if (y_dst < -0.5f)
            {
                // パターン4(下外側)
                if (xz_sq_dst > ring_r * ring_r)
                {
                    vel = glm::vec3(abs(vel.x) * x_dst * 2.0f, -abs(vel.y) * rst_e, abs(vel.z) * z_dst * 2.0f);
                    printf("Ling_4!!\n");
                    // めり込み対策に位置をずらしておく
                    if (x_dst >= 0)
                    {
                        dsp.x -= (x_dst - (ball_r + ring_r)) * x_dst;
                    }
                    else if (x_dst < 0)
                    {
                        dsp.x += (x_dst + (ball_r + ring_r)) * x_dst;
                    }
                    if (z_dst >= 0)
                    {
                        dsp.z -= (z_dst - (ball_r + ring_r)) * z_dst;
                    }
                    else if (z_dst < 0)
                    {
                        dsp.z += (z_dst + (ball_r + ring_r)) * z_dst;
                    }
                }
                // パターン3(下内側)
                else if (xz_sq_dst <= ring_r * ring_r)
                {
                    vel = glm::vec3(-abs(vel.x) * x_dst * 2.0f, -abs(vel.y) * rst_e, -abs(vel.z) * z_dst * 2.0f);
                    printf("Ling_3!!\n");
                    // めり込み対策に位置をずらしておく
                    if (x_dst >= 0)
                    {
                        dsp.x -= (x_dst - (ring_r - ball_r)) * x_dst;
                    }
                    else if (x_dst < 0)
                    {
                        dsp.x += (x_dst + (ring_r - ball_r)) * x_dst;
                    }
                    if (z_dst >= 0)
                    {
                        dsp.z -= (z_dst - (ring_r - ball_r)) * z_dst;
                    }
                    else if (z_dst < 0)
                    {
                        dsp.z += (z_dst + (ring_r - ball_r)) * z_dst;
                    }
                }
                // めり込み対策に位置をずらしておく
                dsp.y -= y_dst + ball_r;
                dsp.x += vel.x * dt * 0.5f;
                dsp.z += vel.z * dt * 0.5f;
            }
        }
        if (xz_sq_dst <= (ring_r) * (ring_r) && y_dst <= -0.05f && goal_flag == 1 && ball_bound_count == 0)
        {
            goal_flag = 2;
            printf("goal flag 2!\n");
        }
    }

    // 以下ボードとの衝突判定
    // (ボード、リングの衝突は同時に起こりうるので、
    // 先にリングを判定し、その結果の位置ずれも考慮してボードの判定を行った方がいい・・・？)
    if (board_LL_p.x - ball_r < c_pos.x && c_pos.x <= board_LL_p.x + board_size.x + ball_r &&
        // board_LL_p.y - ball_r < c_pos.y && c_pos.y <= board_LL_p.y + board_size.y + ball_r &&
        0.0f < c_pos.y && c_pos.y <= board_LL_p.y + board_size.y + ball_r && // ボードは地面からずっとある扱い
        board_LL_p.z - ball_r < c_pos.z && c_pos.z <= board_LL_p.z + board_size.z + ball_r)

    {
        vel = glm::vec3(-abs(vel.x) * rst_e, vel.y, vel.z);
        printf("board!! y:%f, z:%f\n", c_pos.y, c_pos.z);
        // めり込み対策に位置をずらしておく
        dsp.x -= board_LL_p.x - c_pos.x + ball_r;
    }

    modelMat = glm::translate(modelMat, dsp);

    // 視点位置をボールに追従させる
    glm::vec4 sight_pos = modelMat * glm::vec4(2.5f, 2.0f, 0.001f, 1.0f);
    if (sight_pos.x >= 11.0f)
    {
        camera_flag = 1;
    }
    if (not camera_flag)
    {
        viewMat = glm::lookAt(glm::vec3(fmin(11.0f, sight_pos.x - 3.0f), sight_pos.y - 2.0f, sight_pos.z), // 視点の位置
                              glm::vec3(11.8f, 3.0f, 0.0f),                                                // 見ている先
                              glm::vec3(0.0f, 1.0f, 0.0f));                                                // 視界の上方向
    }
}

void VertexArrayObject::drawVao()
{
    // 背景色の描画
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 rotateMat = glm::rotate(glm::radians(theta), glm::vec3(1.0f, 0.0f, 1.0f));
    glm::mat4 mvMat = viewMat * modelMat * rotateMat;
    glm::mat4 mvpMat = projMat * viewMat * modelMat * rotateMat;

    glm::mat4 normMat = glm::transpose(glm::inverse(mvMat));
    glm::mat4 lightMat = viewMat;

    // VAOの有効化
    glBindVertexArray(vaoId);

    // シェーダの有効化
    glUseProgram(programId);

    // Uniform変数の転送
    GLuint uid;
    uid = glGetUniformLocation(programId, "u_mvpMat");
    glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(mvpMat));
    uid = glGetUniformLocation(programId, "u_mvMat");
    glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(mvMat));
    uid = glGetUniformLocation(programId, "u_normMat");
    glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(normMat));
    uid = glGetUniformLocation(programId, "u_lightMat");
    glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(lightMat));

    uid = glGetUniformLocation(programId, "u_lightPos");
    glUniform3fv(uid, 1, glm::value_ptr(lightPos));
    // uid = glGetUniformLocation(programId, "u_diffColor");
    // glUniform3fv(uid, 1, glm::value_ptr(diffColor));
    uid = glGetUniformLocation(programId, "u_specColor");
    glUniform3fv(uid, 1, glm::value_ptr(specColor));
    uid = glGetUniformLocation(programId, "u_ambiColor");
    glUniform3fv(uid, 1, glm::value_ptr(ambiColor));
    uid = glGetUniformLocation(programId, "u_roughness");
    glUniform1f(uid, roughness);

    // テクスチャの有効化とシェーダへの転送
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    uid = glGetUniformLocation(programId, "u_texture");
    glUniform1i(uid, 0);

    // 三角形の描画
    glDrawElements(GL_TRIANGLES, indexBufferSize, GL_UNSIGNED_INT, 0);

    // VAOの無効化
    glBindVertexArray(0);

    // シェーダの無効化
    glUseProgram(0);

    // テクスチャの無効化
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

struct plainObject
{
    plainObject(const std::string &tex_name) : tex_name(tex_name) {}

    glm::vec3 positions[4] = {
        glm::vec3(-1.0f, -1.0f, 0.999f),
        glm::vec3(-1.0f, 1.0f, 0.999f),
        glm::vec3(1.0f, -1.0f, 0.999f),
        glm::vec3(1.0f, 1.0f, 0.999f)};

    glm::vec2 tex[4] = {
        glm::vec2(0.0f, 1.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(1.0f, 0.0f)};

    unsigned int p_indices[6][2] = {{0, 0}, {1, 1}, {2, 2}, {1, 1}, {2, 2}, {3, 3}};
    const std::string tex_name;

    GLuint vaoId;
    GLuint vertexBufferId;
    GLuint indexBufferId;
    GLuint programId;
    GLuint textureId;

    void initPo();
    void drawPo();
};

void plainObject::initPo()
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    int idx = 0;
    for (int i = 0; i < 6; i++)
    {
        Vertex v(positions[p_indices[i][0]], glm::vec3(1.0f, 0.0f, 0.0f), tex[p_indices[i][1]]);
        vertices.push_back(v);
        indices.push_back(idx++);

        // VAOの作成
        glGenVertexArrays(1, &vaoId);
        glBindVertexArray(vaoId);

        // 頂点バッファの作成
        glGenBuffers(1, &vertexBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        // 頂点バッファの有効化
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tex));

        // 頂点番号バッファの作成
        glGenBuffers(1, &indexBufferId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
                     indices.data(), GL_STATIC_DRAW);

        // VAOをOFFにしておく
        glBindVertexArray(0);
        programId = buildShaderProgram(VERT_SHADER_FILE_PO, FRAG_SHADER_FILE_PO);

        // initTexture(テクスチャの設定)
        int texWidth, texHeight, channels;
        unsigned char *bytes = stbi_load(tex_name.c_str(), &texWidth, &texHeight, &channels, STBI_rgb_alpha);

        if (!bytes)
        {
            fprintf(stderr, "Failed to load image file: %s\n", tex_name.c_str());
            exit(1);
        }

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(bytes);
    };
}

void plainObject::drawPo()
{
    // VAOの有効化
    glBindVertexArray(vaoId);

    // シェーダの有効化
    glUseProgram(programId);

    // Uniform変数の転送
    GLuint uid;
    // uid = glGetUniformLocation(programId, "u_mvpMat");
    // glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(mvpMat));

    // テクスチャの有効化とシェーダへの転送
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    uid = glGetUniformLocation(programId, "u_texture");
    glUniform1i(uid, 0);

    // 三角形の描画
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // VAOの無効化
    glBindVertexArray(0);

    // シェーダの無効化
    glUseProgram(0);

    // テクスチャの無効化
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// 以下マウス操作の関数
void mouseEvent(GLFWwindow *window, int button, int action, int mods)
{
    // クリックしたボタンで処理を切り替える
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        arcballMode = ARCBALL_MODE_ROTATE;
    }
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    {
        arcballMode = ARCBALL_MODE_SCALE;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        arcballMode = ARCBALL_MODE_TRANSLATE;
        printf("right!!\n");
        cheat_flag = 1;
    }

    // クリックされた位置を取得
    double px, py;
    glfwGetCursorPos(window, &px, &py);

    if (action == GLFW_PRESS)
    {
        if (game_step == 0)
        {
            game_step = 1;
        }
        if (click_wait == 0)
        {
            click_wait = 1;
        }
        if (!isDragging)
        {
            isDragging = true;
            oldPos = glm::ivec2(px, py);
            newPos = glm::ivec2(px, py);
            if (shoot_flag == 0)
            {
                shoot_flag = 1;
                shooting_time = glfwGetTime();
            }
        }
    }
    else
    {
        if (shoot_flag == 1)
        {
            shoot_flag = 2;
        }
        isDragging = false;
        oldPos = glm::ivec2(0, 0);
        newPos = glm::ivec2(0, 0);
        arcballMode = ARCBALL_MODE_NONE;
    }
}

void mouseMoveEvent(GLFWwindow *window, double xpos, double ypos)
{
    if (isDragging)
    {
        // マウスの現在位置を更新
        newPos = glm::ivec2(xpos, ypos);

        // マウスがあまり動いていない時は処理をしない
        const double dx = newPos.x - oldPos.x;
        const double dy = newPos.y - oldPos.y;
        const double length = dx * dx + dy * dy;
        if (length < 2.0f * 2.0f)
        {
            shoot_vel = glm::vec3(0.0f); //一旦ドラッグを止めてタイミング調整することを禁止する
            shooting_total_time = 0.0;
            shooting_time = glfwGetTime();
            return;
        }
        else
        {
            // updateMouse();
            if (shoot_flag == 1)
            {
                double tmp_shooting_dt = glfwGetTime() - shooting_time;
                shooting_total_time += tmp_shooting_dt;

                shoot_vel += glm::vec3(0.0f, -dy, dx);

                shooting_time = glfwGetTime();
            }
            oldPos = glm::ivec2(xpos, ypos);
        }
    }
}

void resetShootingVariables(int stage_num)
{
    if (stage_num <= 1)
    {
        // free throw
        viewMat = viewMat_01;
    }
    else if (stage_num == 2)
    {
        viewMat = viewMat_02;
    }
    else if (stage_num == 3)
    {
        viewMat = viewMat_03;
    }
    ball_bound_count = 0;
    shoot_flag = 0;
    goal_flag = 0;
    camera_flag = 0;
    shoot_vel = glm::vec3(0.0f);
    shooting_total_time = 0.0;
}

int main(int argc, char **argv)
{

    // OpenGLを初期化する
    if (glfwInit() == GL_FALSE)
    {
        fprintf(stderr, "Initialization failed!\n");
        return 1;
    }

    // OpenGLのバージョン設定 (Macの場合には必ず必要)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Windowの作成
    GLFWwindow *window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE,
                                          NULL, NULL);
    // WIN_TITLE = "stage";
    if (window == NULL)
    {
        fprintf(stderr, "Window creation failed!");
        glfwTerminate();
        return 1;
    }

    // OpenGLの描画対象にWindowを追加
    glfwMakeContextCurrent(window);

    // マウスのイベントを処理する関数を登録
    glfwSetMouseButtonCallback(window, mouseEvent);
    glfwSetCursorPosCallback(window, mouseMoveEvent);
    // glfwSetScrollCallback(window, wheelEvent);

    // OpenGL 3.x/4.xの関数をロードする (glfwMakeContextCurrentの後でないといけない)
    const int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        fprintf(stderr, "Failed to load OpenGL 3.x/4.x libraries!\n");
        return 1;
    }

    // バージョンを出力する
    printf("Load OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

    plainObject vao_s1(TEX_FILE_s1);
    vao_s1.initPo();
    plainObject vao_s2(TEX_FILE_s2);
    vao_s2.initPo();
    plainObject vao_s3(TEX_FILE_s3);
    vao_s3.initPo();

    plainObject vao_ttl(TEX_FILE_t);
    vao_ttl.positions[0] = glm::vec3(-2.0 / 3.0, -1.0f + 0.1f, 0.001f);
    vao_ttl.positions[1] = glm::vec3(-2.0 / 3.0, -2.0 / 3.0 + 0.1f, 0.001f);
    vao_ttl.positions[2] = glm::vec3(2.0 / 3.0, -1.0f + 0.1f, 0.001f);
    vao_ttl.positions[3] = glm::vec3(2.0 / 3.0, -2.0 / 3.0 + 0.1f, 0.001f);
    vao_ttl.initPo();

    plainObject vao_gl(TEX_FILE_gl);
    vao_gl.positions[0] = glm::vec3(0.55, 0.85 - 0.05, 0.001f);
    vao_gl.positions[1] = glm::vec3(0.55, 1.0 - 0.05, 0.001f);
    vao_gl.positions[2] = glm::vec3(1.0, 0.85 - 0.05, 0.001f);
    vao_gl.positions[3] = glm::vec3(1.0, 1.0 - 0.05, 0.001f);
    vao_gl.initPo();

    VertexArrayObject vao_w(OBJECT_FILE_w, TEX_FILE_w);
    vao_w.initVao();

    // ウィンドウのリサイズを扱う関数の登録
    glfwSetWindowSizeCallback(window, resizeGL);

    // ゲーム開始
    while (glfwWindowShouldClose(window) == GL_FALSE)
    {
        glfwSetWindowTitle(window, WIN_TITLE);
        // スタート画面
        VertexArrayObject vao_b_start(OBJECT_FILE_b, TEX_FILE_b, 0.06);
        vao_b_start.initVao();
        grav_a = 0.0f;
        viewMat = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),   // 視点の位置
                              glm::vec3(0.3f, -0.17f, 0.3f), // 見ている先
                              glm::vec3(0.0f, 1.0f, 0.0f));  // 視界の上方向

        while (glfwWindowShouldClose(window) == GL_FALSE && game_step == 0)
        { // 背景色の描画
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            vao_b_start.drawVao();
            vao_ttl.drawPo();

            vao_b_start.theta += 0.5f;
            // 描画用バッファの切り替え
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        cheat_flag = 0;
        // このwhile内部でgame_stepに応じたステージが始まる
        while (glfwWindowShouldClose(window) == GL_FALSE)
        {
            char winTitle[256];
            sprintf(winTitle, "%s: Stage %d [シュート数 %d,ゴール数 %d]", WIN_TITLE, game_step, shooting_count, goal_count);
            click_wait = 0;
            if (game_step == 1)
            {
                while (glfwWindowShouldClose(window) == GL_FALSE && shooting_count == 0 && click_wait == 0)
                {
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    glfwSetWindowTitle(window, winTitle);
                    vao_s1.drawPo();
                    glfwSwapBuffers(window);
                    glfwPollEvents();
                }
            }
            else if (game_step == 2)
            {
                while (glfwWindowShouldClose(window) == GL_FALSE && shooting_count == 0 && click_wait == 0)
                {
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    glfwSetWindowTitle(window, winTitle);
                    vao_s2.drawPo();
                    glfwSwapBuffers(window);
                    glfwPollEvents();
                }
            }
            else if (game_step == 3)
            {
                while (glfwWindowShouldClose(window) == GL_FALSE && shooting_count == 0 && click_wait == 0)
                {
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    glfwSetWindowTitle(window, winTitle);
                    vao_s3.drawPo();
                    glfwSwapBuffers(window);
                    glfwPollEvents();
                }
            }

            resetShootingVariables(game_step);
            VertexArrayObject vao_b(OBJECT_FILE_b, TEX_FILE_b, 0.006);
            vao_b.initVao();

            VertexArrayObject vao_g(OBJECT_FILE_g, TEX_FILE_g, 0.011);
            vao_g.initVao();
            if (game_step == 1)
            {
                vao_b.modelMat = glm::translate(vao_b.modelMat, free_throw_pos);
            }
            else if (game_step == 2)
            {
                vao_b.modelMat = glm::translate(vao_b.modelMat, front_3p_pos);
            }
            else if (game_step == 3)
            {
                vao_b.modelMat = glm::translate(vao_b.modelMat, center_line_pos);
            }

            vao_g.modelMat *= glm::rotate(glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            vao_g.modelMat *= glm::rotate(glm::radians(-85.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            vao_g.modelMat = glm::translate(vao_g.modelMat, glm::vec3(0.25f, 13.0f, -1.12f));

            VertexArrayObject vao_c(OBJECT_FILE_c, TEX_FILE_c);
            vao_c.initVao();

            float jump_vel = 2.5f;

            // シュート毎のループ
            while (glfwWindowShouldClose(window) == GL_FALSE && shoot_flag == 0)
            {
                // 背景色の描画
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glfwSetWindowTitle(window, winTitle);
                // 描画
                vao_b.drawVao();
                vao_g.drawVao();
                vao_c.drawVao();

                vao_w.drawVao();

                // 描画用バッファの切り替え
                glfwSwapBuffers(window);
                glfwPollEvents();
            }
            vao_b.vel = glm::vec3(0.0f, jump_vel, 0.0f);
            current_time = glfwGetTime();
            grav_a = -4.0f; // ジャンプ中はゆっくり落下させる

            while (glfwWindowShouldClose(window) == GL_FALSE && shoot_flag == 1 && ball_bound_count < 2)
            {
                // 背景色の描画
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glfwSetWindowTitle(window, winTitle);
                // 描画
                vao_b.drawVao();
                vao_g.drawVao();
                vao_c.drawVao();
                vao_w.drawVao();

                // アニメーション
                vao_b.animate();
                vao_b.theta = 0.0f;
                // 描画用バッファの切り替え
                glfwSwapBuffers(window);
                glfwPollEvents();
            }

            shoot_vel /= fmax(0.001, 0.4 + shooting_total_time * 2.0);
            float release_point = vao_b.vel.y;
            float shoot_angle = PI * (0.2 + 0.05 * (2.5 - fmax(vao_b.vel.y, -0.1)) / 2.5);
            vao_b.vel = glm::vec3(cos(shoot_angle) * (4.7f + shoot_vel.y / 100.0f), sin(shoot_angle) * (4.7f + shoot_vel.y / 100.0f), shoot_vel.z / 200.0f);

            grav_a = -9.8f; // 重力加速度を元に戻す
            shooting_count++;
            sprintf(winTitle, "%s v = (%.2f ,%.2f, %.2f)", winTitle, vao_b.vel.x, vao_b.vel.y, vao_b.vel.z);
            while (glfwWindowShouldClose(window) == GL_FALSE && shoot_flag == 2 && ball_bound_count < 2)
            {
                // 背景色の描画
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glfwSetWindowTitle(window, winTitle);
                // 描画
                vao_b.drawVao();
                vao_g.drawVao();
                vao_c.drawVao();
                vao_w.drawVao();
                // vao_gl.drawPo();
                if (goal_flag == 2)
                {
                    vao_gl.drawPo();
                }

                // アニメーション
                vao_b.animate();

                // 描画用バッファの切り替え
                glfwSwapBuffers(window);
                glfwPollEvents();
            }
            if (goal_flag == 2)
            {
                goal_count++;
            }

            // チートモードの有効化判定
            if (cheat_flag == 1)
            {
                shooting_count = 10;
                goal_count = 100;
            }
            sprintf(winTitle, "%s: Stage %d [シュート数 %d,ゴール数 %d]", WIN_TITLE, game_step, shooting_count, goal_count);
            if (goal_count >= 2)
            {
                printf("goals: %d\n", goal_count);
                goal_count = 0;
                shooting_count = 0;
                game_step++;
                click_wait = 0;
                plainObject vao_sc(TEX_FILE_sc);
                vao_sc.initPo();
                while (glfwWindowShouldClose(window) == GL_FALSE && click_wait == 0)
                {
                    // 背景色の描画
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    glfwSetWindowTitle(window, winTitle);
                    vao_sc.drawPo();
                    // 描画用バッファの切り替え
                    glfwSwapBuffers(window);
                    glfwPollEvents();
                }

                printf("Stage Clear!!\n");
                if (game_step >= 4)
                {
                    plainObject vao_gc(TEX_FILE_gc);
                    vao_gc.initPo();
                    click_wait = 0;
                    while (glfwWindowShouldClose(window) == GL_FALSE && click_wait == 0)
                    {
                        // 背景色の描画
                        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                        glfwSetWindowTitle(window, winTitle);
                        vao_gc.drawPo();
                        // 描画用バッファの切り替え
                        glfwSwapBuffers(window);
                        glfwPollEvents();
                    }
                    game_step = 0;
                }
                break;
            }
            if (shooting_count >= 5)
            {
                printf("goals: %d\n", goal_count);
                click_wait = 0;
                plainObject vao_go(TEX_FILE_go);
                vao_go.initPo();
                while (glfwWindowShouldClose(window) == GL_FALSE && click_wait == 0)
                {
                    // 背景色の描画
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    glfwSetWindowTitle(window, winTitle);
                    vao_go.drawPo();
                    // 描画用バッファの切り替え
                    glfwSwapBuffers(window);
                    glfwPollEvents();
                }
                shooting_count = 0;
                goal_count = 0;
                game_step = 0;
                break;
            }
        }
    }
}