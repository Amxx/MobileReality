#version 140

#define PI 3.14159



#ifdef VERTEX_SHADER

uniform 	samplerCube	envmap;
uniform		mat4				mvMatrix;
uniform		mat4				mvpMatrix;
uniform		float				ns;

in				vec3				position;
in				vec3				texcoord;
in				vec3				normal;

out				vec3				vertex_position;
out				vec3				vertex_normal;
out				vec3				vertex_texcoord;

out 			vec4				diffuse_light;
out				float				diffuse_level;
out				float 			specular_level;


void main()
{
	gl_Position				= (mvpMatrix * vec4(position, 1.0));	
	vertex_position 	= (mvMatrix  * vec4(position, 1.0)).xyz;
	vertex_normal			= (mvMatrix  * vec4(normal,   0.0)).xyz;	
	vertex_texcoord	= texcoord;
	
	int		size				= textureSize(envmap, 0).x;
	diffuse_level			= log2(size);
	specular_level		= log2(size * sqrt(3.0)) - 0.5 * log2(ns + 1);
	
	vec3	n						=	normalize(normal);
	diffuse_light			= (	textureLod(envmap, vec3(+1.0, +0.0, +0.0),	diffuse_level)
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
	
	// Old Method
	//diffuse_light				= textureLod(envmap, n,	diffuse_level);
}
#endif




#ifdef FRAGMENT_SHADER
	
uniform 	samplerCube	envmap;
uniform 	mat4				mvMatrixInv;
uniform		float				kd;
uniform		float				ks;
uniform		bool				usevisibility;
uniform 	sampler2D		visibility;

in				vec3				vertex_position;
in				vec3				vertex_normal;
in				vec3				vertex_texcoord;

in	 			vec4				diffuse_light;
in				float				diffuse_level;
in				float 			specular_level;

out				vec4				fragment_color;

void main() 
{	
	vec3	n								= normalize(vertex_normal);
	vec3	l								= reflect(normalize(vertex_position), n);
	vec3	l_glob					= normalize(mat3(mvMatrixInv) * l);
	float cos_theta				= abs(dot(n, l));
	
	vec4	env_diffuse			= diffuse_light;
	if (usevisibility) env_diffuse *= texture(visibility, vertex_texcoord.st);

	vec4	env_specular		= textureLod(envmap,	l_glob, specular_level);
	vec3	color						= vec3(	env_diffuse  * kd + env_specular * ks	);
															
	fragment_color				= vec4(color, 1);
	
}
#endif
