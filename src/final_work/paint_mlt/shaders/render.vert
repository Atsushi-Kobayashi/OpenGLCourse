#version 330

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoords;

out vec3 f_fragColor;
out vec2 f_texcoords;

// Oren-Nayar用
out vec3 f_positionCameraSpace;
out vec3 f_normalCameraSpace;
out vec3 f_lightPosCameraSpace;

uniform mat4 u_mvpMat;

// Oren-Nayar用
// 光源の情報
uniform vec3 u_lightPos;
uniform mat4 u_mvMat;
uniform mat4 u_normMat;
uniform mat4 u_lightMat;

void main() {
    gl_Position = u_mvpMat * vec4(in_position, 1.0);

    // Varying変数への代入
    f_texcoords = in_texcoords;
    // カメラ座標系への変換
    f_positionCameraSpace = (u_mvMat * vec4(in_position, 1.0)).xyz;
    f_normalCameraSpace = (u_normMat * vec4(in_normal, 0.0)).xyz;
    f_lightPosCameraSpace = (u_lightMat * vec4(u_lightPos, 1.0)).xyz;
}
