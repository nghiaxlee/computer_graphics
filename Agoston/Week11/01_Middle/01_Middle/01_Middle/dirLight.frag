#version 130

in vec3 vs_out_pos;
in vec3 vs_out_normal;
in vec2 vs_out_tex0;

out vec4 fs_out_col;

//
// uniform variables
//

// scene attributes
uniform vec4 La = vec4(0.1f, 0.1f, 0.1f, 1);
uniform vec3 eye_pos = vec3(0, 15, 15);

// light attributes
uniform vec3 light_pos = vec3( 3, 8, 0 );
uniform vec4 Ld = vec4(0.5f, 0.5f, 0.5f, 1);
uniform vec4 Ls = vec4(1, 1, 1, 1);

// material attributes
uniform vec4 Ka = vec4(1, 1, 1, 1);
uniform vec4 Kd = vec4(0.75f, 0.25f, 0.125f, 1);
uniform vec4 Ks = vec4(0, 1, 0, 1);
uniform float specular_power = 16;
uniform sampler2D texImage;

uniform int index = 0;

void main()
{
	//
	// ambient
	//
	vec4 ambient = La * Ka;

	//
	// diffuse
	//
	vec3 normal = normalize( vs_out_normal );
	vec3 toLight = normalize(light_pos - vs_out_pos);
	float di = clamp( dot( toLight, normal), 0.0f, 1.0f );
	vec4 diffuse = Ld*Kd*di;

	//
	// specular
	//
	vec4 specular = vec4(0);

	if ( di > 0 )
	{
		vec3 e = normalize( eye_pos - vs_out_pos );
		vec3 r = normalize( reflect( -toLight, normal ) );
		float si = pow( clamp( dot(e, r), 0.0f, 1.0f ), specular_power );
		specular = Ls*Ks*si;
	}

	fs_out_col = index*(ambient+diffuse+specular)*texture(texImage, vs_out_tex0.st);
}
