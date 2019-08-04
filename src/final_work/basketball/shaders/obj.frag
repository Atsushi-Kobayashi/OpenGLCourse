#version 330

in vec2 f_texcoords;

in vec3 f_positionCameraSpace;
in vec3 f_normalCameraSpace;
in vec3 f_lightPosCameraSpace;

out vec4 out_color;

// Uniform変数 (texture)
uniform sampler2D u_tex;

// マテリアルのデータ
// uniform vec3 u_diffColor;
uniform vec3 u_specColor;
uniform vec3 u_ambiColor;
uniform float u_roughness;

void main() {
    vec3 diff_color = texture(u_tex, f_texcoords).rgb;

	// カメラ座標系を元にした局所座標系への変換
	vec3 V = normalize(-f_positionCameraSpace);
	vec3 N = normalize(f_normalCameraSpace);
	vec3 L = normalize(f_lightPosCameraSpace - f_positionCameraSpace);

	// Oren-Nayarの反射モデル
	float ndotl = max(0.0, dot(N, L)); // cos(theta_i)
	float ndotv = max(0.0, dot(N, V)); // cos(theta_r)
	vec3 ncrossl = normalize(cross(N, L));
	vec3 ncrossv = normalize(cross(N, V));
	float A = 1.0 - 0.5*u_roughness/(u_roughness+0.33);
	float B = 0.45*u_roughness/(u_roughness+0.09);
	float theta_i = acos(ndotl);
	float theta_r = acos(ndotv);
	float alpha = max(theta_i,theta_r);
	float beta = min(theta_i,theta_r);
	float phi = acos(dot(ncrossl,ncrossv));

	vec3 ON_diff = diff_color* ndotl*(A+B*max(0.0, cos(phi))*sin(alpha)*tan(beta));
	vec3 ambient = u_ambiColor;

    out_color = vec4(ON_diff + ambient, 1.0);
}