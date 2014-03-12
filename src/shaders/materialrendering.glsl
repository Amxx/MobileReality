#version 140

#ifdef VERTEX_SHADER

uniform		mat4				mvMatrix;
uniform		mat4				mvpMatrix;
in				vec3				position;
in				vec3				texcoords;
in				vec3				normal;
in				vec3				texcoord;
out				vec3				vertex_position;
out				vec3				vertex_normal;
out				vec3				vertex_texcoords;

void main( )
{
	gl_Position				= (mvpMatrix * vec4(position, 1.0));	
	vertex_position 	= (mvMatrix  * vec4(position, 1.0)).xyz;
	vertex_normal			= (mvMatrix  * vec4(normal,   0.0)).xyz;
	vertex_texcoords	= texcoords;
}
#endif




#ifdef FRAGMENT_SHADER
	
	
uniform 	samplerCube	envmap;
uniform 	mat4				mvMatrixInv;

//uniform		vec4				ambient_light;
//uniform		vec4				ambient_color;
//uniform		vec4				diffuse_light;
//uniform		vec4				diffuse_color;
//uniform		vec4				specular_light;
//uniform		vec4				specular_color;
//uniform		float				ka;
uniform		float				kd;
uniform		float				ks;
uniform		float				ns;


in				vec3				vertex_position;        // view
in				vec3				vertex_normal;          // view
in				vec3				vertex_texcoords;

out				vec4				fragment_color;

#define PI 3.14159


void main( ) 
{	
	vec3	n								= normalize(vertex_normal);
	vec3	l								= reflect(normalize(vertex_position), n);
	vec3	l_glob					= normalize(mat3(mvMatrixInv) * l);
	vec3	n_glob					= normalize(mat3(mvMatrixInv) * n);
	vec3	h								= normalize(n + l);
	float cos_theta				= abs(dot(n, l));
//	float blinn						= pow( abs(dot(n, h)), ns ) * (ns + 2) / (2 * PI);
	
//	vec4	ambient 				= vec4(1.0);
//	vec4	diffuse					= vec4(1.0);	  
//	vec4	specular				= vec4(1.0);
//	specular.rgb					= specular.rgb * blinn;

	//! utilise "Plausible Blinn-Phong Reflection of Standard Cube MIP-Maps"
	//! http://graphics.cs.williams.edu/papers/EnvMipReport2013/paper.pdf

	int		size						= textureSize(envmap, 0).x;
	float level_diffuse		= log2(size);
	float level_specular	= log2(size * sqrt(3.0)) - 0.5 * log2(ns + 1);
	
//	vec4	env_ambient			= diffuse_light;
	vec4	env_diffuse			= textureLod(envmap, n_glob, level_diffuse);
	vec4	env_specular		= textureLod(envmap, l_glob, level_specular);

//	vec3 color						= vec3(	env_ambient		* ambient_light		* ka * ambient_color	* ambient		* abs(dot(n, l)) / PI 
//															+ env_diffuse		* diffuse_light		* kd * diffuse_color	* diffuse		* cos_theta 
//															+ env_specular	* specular_light	* ks * specular_color	* specular	* cos_theta);
	
	vec3 color						= vec3(	env_diffuse		* kd * cos_theta 
															+ env_specular	* ks * cos_theta);
															
	fragment_color				= vec4(color, 1);   // opaque
	
}
#endif
