#version 140

#define PI 3.14159



#ifdef VERTEX_SHADER

uniform 	samplerCube	envmap;
uniform		mat4				mvMatrix;
uniform		mat4				mvpMatrix;
uniform		float				ns;

in				vec3				position;
in				vec3				texcoords;
in				vec3				normal;
in				vec3				texcoord;

out				vec3				vertex_position;
out				vec3				vertex_normal;
out				vec3				vertex_texcoords;
out				float				level_diffuse;
out				float 			level_specular;
out 			vec4				env_diffuse;

void main( )
{
	gl_Position				= (mvpMatrix * vec4(position, 1.0));	
	vertex_position 	= (mvMatrix  * vec4(position, 1.0)).xyz;
	vertex_normal			= (mvMatrix  * vec4(normal,   0.0)).xyz;	
	vertex_texcoords	= texcoords;
	
	int		size				= textureSize(envmap, 0).x;
	level_diffuse			= log2(size);
	level_specular		= log2(size * sqrt(3.0)) - 0.5 * log2(ns + 1);
	
	vec3	n						=	normalize(normal);
	env_diffuse				= (	textureLod(envmap, vec3(+1.0, +0.0, +0.0),	level_diffuse) * pow(clamp(0.5 + n.x / sqrt(2.0), 0, 1), 2.0)
											+ textureLod(envmap, vec3(-1.0, +0.0, +0.0),	level_diffuse) * pow(clamp(0.5 - n.x / sqrt(2.0), 0, 1), 2.0)
											+ textureLod(envmap, vec3(+0.0, +1.0, +0.0),	level_diffuse) * pow(clamp(0.5 + n.y / sqrt(2.0), 0, 1), 2.0)
											+ textureLod(envmap, vec3(+0.0, -1.0, +0.0),	level_diffuse) * pow(clamp(0.5 - n.y / sqrt(2.0), 0, 1), 2.0)
											+ textureLod(envmap, vec3(+0.0, +0.0, +1.0),	level_diffuse) * pow(clamp(0.5 + n.z / sqrt(2.0), 0, 1), 2.0)
											+ textureLod(envmap, vec3(+0.0, +0.0, -1.0),	level_diffuse) * pow(clamp(0.5 - n.z / sqrt(2.0), 0, 1), 2.0)	) / 2.0;
}
#endif




#ifdef FRAGMENT_SHADER
	
uniform 	samplerCube	envmap;
uniform 	mat4				mvMatrixInv;
uniform		float				kd;
uniform		float				ks;

in				vec3				vertex_position;
in				vec3				vertex_normal;
in				vec3				vertex_texcoords;
in				float				level_diffuse;
in				float 			level_specular;
in	 			vec4				env_diffuse;

out				vec4				fragment_color;

void main( ) 
{	
	//! utilise "Plausible Blinn-Phong Reflection of Standard Cube MIP-Maps"
	//! http://graphics.cs.williams.edu/papers/EnvMipReport2013/paper.pdf
	
	vec3	n								= normalize(vertex_normal);
	vec3	l								= reflect(normalize(vertex_position), n);
	vec3	l_glob					= normalize(mat3(mvMatrixInv) * l);
	float cos_theta				= abs(dot(n, l));
	
	
	vec4	env_specular		= textureLod(envmap,	l_glob, level_specular);
//vec4	env_specular		= texture(envmap, 		l_glob);
	
	vec3 color						= vec3(	env_diffuse  * kd + env_specular * ks	);
															
	fragment_color				= vec4(color, 1);
	
}
#endif
