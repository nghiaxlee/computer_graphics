#version 130

in vec3 vs_out_col;
in vec2 vs_out_tex0;
out vec4 fs_out_col;

uniform int time = 0;
uniform sampler2D texture;

vec4 LERP(float t, vec4 color1, vec4 color2);

void main()
{
	float t = (sin(time / 1000.0f) + 1) / 2;
	fs_out_col = LERP(t, vec4(vs_out_col, 1), texture2D(texture, vs_out_tex0.st));
}

vec4 LERP(float t, vec4 color1, vec4 color2) // Linear interpolation, 
// t = 0 -> color1 
// t = 1 -> color2
// t in [0,1] -> a mixture of color1 and color2
{
	return (1 - t) * color1 + t * color2;
}