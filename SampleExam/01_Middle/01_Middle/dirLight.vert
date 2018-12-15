#version 130

// vertex attribute inputs
in vec3 vs_in_pos;
in vec3 vs_in_normal;
in vec2 vs_in_tex0;

// vertex attribute outputs
out vec3 vs_out_pos;
out vec3 vs_out_normal;
out vec2 vs_out_tex0;

// shader parameters
uniform mat4 world;
uniform mat4 worldIT;
uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vec4( vs_in_pos, 1 );

	vs_out_pos = (world * vec4( vs_in_pos, 1 )).xyz;
	vs_out_normal  = (worldIT * vec4(vs_in_normal, 0)).xyz;
	vs_out_tex0 = vs_in_tex0;
}