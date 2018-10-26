#version 130

in vec3 vs_out_col;
in vec3 vs_out_pos;
out vec4 fs_out_col;

uniform vec3 color = vec3(1,1,1);
uniform int rows = 8;
uniform int cols = 8;

void main()
{
	// draw chess board
	float cell_width = 2.0f / cols;
	float cell_height = 2.0f / rows;
	
	int col = int((vs_out_pos.x + 1) / cell_width);
	int row = int((vs_out_pos.y + 1) / cell_height);
	
	fs_out_col = vec4(color, 1);
	if ((col + row) % 2 == 0)
		fs_out_col = vec4(0, 0, 0, 1);
	
	// draw a circle, use the discard command
	float distance = sqrt(vs_out_pos.x * vs_out_pos.x + vs_out_pos.y * vs_out_pos.y + vs_out_pos.z * vs_out_pos.z);
	if (distance > 1)
		discard;
}


