#version 330

// Varying変数
in vec3 f_fragColor;

// ディスプレイへの出力変数
out vec4 out_color;

// 選択を判定するためのID
uniform int u_selectID;

// どのObjectであるかの情報
uniform int u_objectID;

//選択されてる物体の情報
uniform int u_selectedObjectID;

void main() {
    if (u_selectID > 0) {
        // 選択のIDが0より大きければIDで描画する
        float c = u_selectID / 255.0;
        out_color = vec4(c, c, c, 1.0);
    } else {

        // 選択されているObjectのみを明るく表示
        if(u_selectedObjectID > 0){
        if(u_selectedObjectID != floor(u_objectID+0.1)){
            out_color = vec4(f_fragColor*0.3, 1.0);
        }
        else{
            out_color = vec4(f_fragColor, 1.0);
        }
        }
        else{
            out_color = vec4(f_fragColor*0.5, 1.0);
        }
    }
}
