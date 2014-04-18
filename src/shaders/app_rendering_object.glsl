#version 410

#define PI 3.14159



#ifdef VERTEX_SHADER

uniform												mat4				mvMatrix;
uniform												mat4				mvMatrixInv;
uniform												mat4				mvpMatrix;
uniform												int					method;
uniform												samplerCube	envmap;

uniform												vec4				diffuse_color;
uniform												bool				use_diffuse_texture;
uniform												sampler2D		diffuse_texture;
uniform												float				kd;
uniform												vec4				specular_color;
uniform												bool				use_specular_texture;
uniform												sampler2D		specular_texture;
uniform												float				ks;
uniform												float				ns;
uniform												vec4				emission;
uniform												float				ni;

layout(location = 0) in				vec3				position;
layout(location = 1) in				vec3				texcoord;
layout(location = 2) in				vec3				normal;

out														vec3				vertex_position;
out														vec3				vertex_normal;
out														vec3				vertex_texcoord;

out														vec4				diffuse_light;
out														float				diffuse_level;
out														float 			specular_level;


void main()
{
	gl_Position				= (mvpMatrix * vec4(position, 1.0));	
	vertex_position 	= (mvMatrix  * vec4(position, 1.0)).xyz;
	vertex_normal			= (mvMatrix  * vec4(normal,   0.0)).xyz;	
	vertex_texcoord		= texcoord;
	
	int		size				= textureSize(envmap, 0).x;
	diffuse_level			= log2(size);
	specular_level		= log2(size * sqrt(3.0)) - 0.5 * log2(ns + 1);
	
	vec3	n						=	normalize(normal);
	if ((method & 0x0001) == 0)
		diffuse_light		= (	textureLod(envmap, vec3(+1.0, +0.0, +0.0),	diffuse_level)
												* pow(max(0.75 + n.x, 0), 2.0)
											+ textureLod(envmap, vec3(-1.0, +0.0, +0.0),	diffuse_level)
												* pow(max(0.75 - n.x, 0), 2.0)
											+ textureLod(envmap, vec3(+0.0, +1.0, +0.0),	diffuse_level)
												* pow(max(0.75 + n.y, 0), 2.0)
											+ textureLod(envmap, vec3(+0.0, -1.0, +0.0),	diffuse_level)
												* pow(max(0.75 - n.y, 0), 2.0)
											+ textureLod(envmap, vec3(+0.0, +0.0, +1.0),	diffuse_level)
												* pow(max(0.75 + n.z, 0), 2.0)
											+ textureLod(envmap, vec3(+0.0, +0.0, -1.0),	diffuse_level)
												* pow(max(0.75 - n.z, 0), 2.0)															) / 1.75 / PI;
	else
		diffuse_light		= textureLod(envmap, n,	diffuse_level);
}
#endif




#ifdef FRAGMENT_SHADER
	
uniform												mat4				mvMatrix;
uniform												mat4				mvMatrixInv;
uniform												mat4				mvpMatrix;
uniform												int					method;
uniform												samplerCube	envmap;

uniform												vec4				diffuse_color;
uniform												bool				use_diffuse_texture;
uniform												sampler2D		diffuse_texture;
uniform												float				kd;
uniform												vec4				specular_color;
uniform												bool				use_specular_texture;
uniform												sampler2D		specular_texture;
uniform												float				ks;
uniform												float				ns;
uniform												vec4				emission;
uniform												float				ni;

in														vec3				vertex_position;
in														vec3				vertex_normal;
in														vec3				vertex_texcoord;

in														vec4				diffuse_light;
in														float				diffuse_level;
in														float 			specular_level;

out														vec4				fragment_color;

void main() 
{
	//if (ni > 0.f) discard;

	vec3	n								= normalize(vertex_normal);
	vec3	l								= reflect(normalize(vertex_position), n);
	vec3	l_glob					= normalize(mat3(mvMatrixInv) * l);
	float cos_theta				= abs(dot(n, l));
	
	vec4	env_diffuse			= kd * diffuse_color  * diffuse_light;
	if (use_diffuse_texture) env_diffuse *= texture(diffuse_texture, vertex_texcoord.st);
	
	vec4	env_specular		= ks * specular_color * textureLod(envmap, l_glob, specular_level);
	if (use_specular_texture) env_specular *= texture(specular_texture, vertex_texcoord.st);
	
	fragment_color				= vec4(env_diffuse.rgb + env_specular.rgb + emission.rgb, 1.0);


	// ===============================
	// =   S H O W   C U B E M A P   =
	// ===============================
	//fragment_color				= texture(envmap, vec3(mvMatrixInv * vec4(vertex_position, 1.0)));
}
#endif
