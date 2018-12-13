#version 130

in vec3 vs_out_pos;
in vec3 vs_out_normal;
in vec2 vs_out_tex0;

out vec4 fs_out_col;

uniform sampler2D img;

// Light properties
uniform vec3 light_pos; // Point light
uniform vec4 La = vec4(0.1, 0.1, 0.1, 1);
uniform vec4 Ld = vec4(0, 0, 0, 1);
uniform vec4 Ls = vec4(1, 1, 1, 1);

// Material properties
uniform vec4 ka = vec4(1, 1, 1, 1);
uniform vec4 kd = vec4(1, 1, 1, 1);
uniform vec4 ks = vec4(1, 1, 1, 1);

// Scene properties
uniform vec3 eye;

void main()
{
	vec3 to_light = normalize(
		light_pos - vs_out_pos);
	vec3 normal = normalize(vs_out_normal);

	//
	// ambient
	//
	vec4 ambient = La * ka;

	//
	// diffuse
	//
	float diffuse_intensity = 
		max(dot(to_light, normal), 0);
	vec4 diffuse = diffuse_intensity * Ld * kd;

	/* help
		- normalize: http://www.opengl.org/sdk/docs/manglsl/xhtml/normalize.xml
	    - dot: http://www.opengl.org/sdk/docs/manglsl/xhtml/dot.xml
	    - clamp: http://www.opengl.org/sdk/docs/manglsl/xhtml/clamp.xml
	*/
	
	//
	// specular
	//

	vec4 specular = vec4(0,0,0,0);

	if(diffuse_intensity > 0) {
		vec3 to_eye = normalize(eye-vs_out_pos);
		vec3 r = reflect(-to_light, normal);
		float specular_intensity = 
			max(dot(to_eye, r), 0);
		
		specular = 
			pow(specular_intensity, 4) *
			Ls * ks;
	}

	/* help
		- reflect: http://www.opengl.org/sdk/docs/manglsl/xhtml/reflect.xml
		- power: http://www.opengl.org/sdk/docs/manglsl/xhtml/pow.xml
	*/
	
	//
	// final fragment color
	//
	fs_out_col = 
		(ambient + diffuse + specular) 
		* texture(img, vs_out_tex0.st);
}