#version 330

// Varying変数
in vec2 f_texcoords;
in float f_faceID;

// ディスプレイへの出力変数
out vec4 out_color;

// Uniform変数 (texture)
uniform sampler2D u_tex;

// 選択を判定するためのID
uniform int u_selectID;

//選択されてる面がどこであるかの情報
uniform int selectedFaceID;

void main() {
    // 描画色を代入
    if (u_selectID > 0) {
        // 選択のIDが0より大きければIDで描画する
        float c = f_faceID / 255.0;
        out_color = vec4(c, c, c, 1.0);
    } else {
        if(selectedFaceID != floor(f_faceID+0.1) && selectedFaceID > 0){
            out_color = vec4(texture(u_tex, f_texcoords).rgb*0.3, 1.0);
        }

        else{
            out_color = vec4(texture(u_tex, f_texcoords).rgb, 1.0);
        }
    }
}