#version 140



#ifdef VERTEX_SHADER

uniform		mat4				mvMatrix;
uniform		mat4				mvnMatrix;
uniform		mat4				mvpMatrix;

in				vec3				position;
in				vec3				normal;
in				vec3				texcoord;

out				vec3				vertex_incident;
out				vec3				vertex_normal;


out				vec3				cubeposition;

void main()
{
	gl_Position				= (mvpMatrix				* vec4(position, 1.0));	
	vertex_incident 	= (mvMatrix					* vec4(position, 1.0)).xyz;
	vertex_normal			= (mat3(mvnMatrix)	*	normal).xyz;
	
	cubeposition = position;
}
#endif



#ifdef FRAGMENT_SHADER

uniform		samplerCube	envmap;
uniform		mat4				mvnMatrixInv;
in				vec3				vertex_incident;
in				vec3				vertex_normal;
out				vec4				fragment_color;


in				vec3				cubeposition;

void main()
{	
	vec3 i					= normalize(vertex_incident);
	vec3 n					= normalize(vertex_normal);
	vec3 r					= normalize(mat3(mvnMatrixInv) * reflect(i, n));
	
	//fragment_color 	=	vec4(r, 1.0);
	//fragment_color 	=	texture(envmap, r);
	
	fragment_color 	=	texture(envmap, normalize(cubeposition));
	
}

#endif