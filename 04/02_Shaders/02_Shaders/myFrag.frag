#version 130

in vec3 vs_out_col;
in vec3 vs_out_pos;
out vec4 fs_out_col;

uniform int max_iter = 100;
uniform vec2 center = vec2(0, 0);

void main()
{
	fs_out_col = vec4(vs_out_col, 1);
	//if (vs_out_pos.y > -0.5 && vs_out_pos.y < 0.5 && vs_out_pos.x > -.5 && vs_out_pos.x < .5)
	//{
	//	fs_out_col = vec4(1, 1, 1, 1);
	//}
	//if ((vs_out_pos.x + 0.5) * (vs_out_pos.x + 0.5) + (vs_out_pos.y + 0.5) * (vs_out_pos.y + 0.5) <= 0.5 )
	//{
	//	fs_out_col = vec4(1, 1, 1, 1);
	//}

	//if ((vs_out_pos.x - 0.5) * (vs_out_pos.x - 0.5) + (vs_out_pos.y - 0.5) * (vs_out_pos.y - 0.5) <= 0.5 )
	//{
	//	fs_out_col = vec4(1, 1, 1, 1);
	//}

	// Mandelbrot set. (Fractal)
	vec2 c = vs_out_pos.xy; 
	vec2 z = c;
	int n = 0;
	for(int i = 0; i < max_iter && length(z) < 2; ++i)
	{
		z = vec2(z.x * z.x - z.y * z.y,
				2 * z.x * z.y) + center;
		
		fs_out_col = vec4(z / length(z), 1.0 * i / max_iter, 1);
	}

}

// 1.: draw a white rectangle

// 2.: by using uniform variables, let the application determine the color of the triangle

// 3.: draw a circle, use the discard command
