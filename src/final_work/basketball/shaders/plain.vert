#version 330

// Attribute変数
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_texcoords;


// Varying変数
out vec2 f_texcoords;

// Uniform変数
// uniform mat4 u_mvpMat;


void main() {

    gl_Position = vec4(in_position, 1.0);
    f_texcoords = in_texcoords;

}
