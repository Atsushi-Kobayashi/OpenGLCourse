#version 330

// Varying変数
in vec2 f_texcoords;

// ディスプレイへの出力変数
out vec4 out_color;

// Uniform変数 (texture)
uniform sampler2D u_tex;

void main() {

    // 描画色を代入
    out_color = vec4(texture(u_tex, f_texcoords).rgb, 1.0);
}