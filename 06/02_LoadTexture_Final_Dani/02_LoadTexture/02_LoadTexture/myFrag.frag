#version 130

in vec3 vs_out_col;
in vec2 vs_out_tex0;
out vec4 fs_out_col;

uniform sampler2D texture;
uniform float alpha = 0.5;

vec4 LERP(float t, vec4 p1, vec4 p2);

void main()
{
	fs_out_col = LERP(alpha, vec4(vs_out_col, 1), texture2D(texture, vs_out_tex0.st));
}

vec4 LERP(float t, vec4 p1, vec4 p2)
{
	return (1 - t) * p1 + t * p2;
}