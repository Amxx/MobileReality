#version 140


#ifdef VERTEX_SHADER

uniform		mat4				mvMatrix;
uniform		mat4				mvpMatrix;
in				vec3				position;
in				vec3				normal;
in				vec3				texcoord;
out				vec3				vertex_incident;
out				vec3				vertex_normal;

void main()
{
	gl_Position				= (mvpMatrix * vec4(position, 1.0));	
	vertex_incident 	= (mvMatrix  * vec4(position, 1.0)).xyz;
	vertex_normal			= (mvMatrix  * vec4(normal,   0.0)).xyz;
}
#endif


#ifdef FRAGMENT_SHADER

uniform		samplerCube	envmap;
uniform		mat4				mvMatrixInv;
in				vec3				vertex_incident;
in				vec3				vertex_normal;
out				vec4				fragment_color;

void main()
{	
	vec3 i					= normalize(vertex_incident);
	vec3 n					= normalize(vertex_normal);
	vec3 r					= normalize(mat3(mvMatrixInv) * reflect(i, n));
	
	fragment_color 	= 	vec4(0.5, 0.5, 0.5, 1.0)	* 0.5
										+	texture(envmap, r)				* 0.5;
}

#endif