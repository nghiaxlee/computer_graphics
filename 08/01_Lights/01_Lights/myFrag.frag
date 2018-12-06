#version 130

in vec3 vs_out_pos;
in vec3 vs_out_normal;
in vec2 vs_out_tex0;

out vec4 fs_out_col;

uniform sampler2D img;

//Playaround
uniform vec3 color;

// Eye properties
uniform vec3 eye_pos;

// Light properties
uniform vec3 light_pos; // Point light
uniform vec4 La = vec4(0.1, 0.1, 0.1, 1); // L ambient.
uniform vec4 Ld = vec4(1, 1, 1, 1); // L diffuse.
uniform vec4 Ls = vec4(1, 1, 1, 1); // L specular.

// Material properties
uniform vec4 ka = vec4(1, 1, 1, 1);
uniform vec4 kd = vec4(1, 1, 1, 1);
//uniform vec4 kd = vec4(color, 1);
uniform vec4 ks = vec4(1, 1, 1, 1);

void main()
{
	vec3 to_light = normalize(light_pos - vs_out_pos);
	vec3 to_eye = normalize(eye_pos - vs_out_pos);
	vec3 normal = normalize(vs_out_normal);
	//
	// ambient
	//
	vec4 ambient = La * ka;
	//
	// diffuse
	//
	//vec4 kd = vec4(color, 1);
	float diffuse_scalar = max(dot(to_light, normal), 0);
	vec4 diffuse = diffuse_scalar * Ld * kd; // dot(to_light, normal) == cos(to_light, normal) since to_light and normal are normalized;
	/* help
		- normalize: http://www.opengl.org/sdk/docs/manglsl/xhtml/normalize.xml
	    - dot: http://www.opengl.org/sdk/docs/manglsl/xhtml/dot.xml
	    - clamp: http://www.opengl.org/sdk/docs/manglsl/xhtml/clamp.xml
	*/
	
	//
	// specular
	//
	vec3 reflected = normalize(reflect(-to_light, normal)); // Remember to take -to_light
	vec4 specular = vec4(0, 0, 0, 0);
	if (diffuse_scalar > 0)
	{
		specular = pow(dot(reflected, to_eye), 4) * Ls * ks;
	}
	/* help
		- reflect: http://www.opengl.org/sdk/docs/manglsl/xhtml/reflect.xml
		- power: http://www.opengl.org/sdk/docs/manglsl/xhtml/pow.xml
	*/
	
	//
	// final fragment color
	//
	fs_out_col = (ambient + diffuse + specular) * texture(img, vs_out_tex0.st);
}