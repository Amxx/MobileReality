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

void main()
{
	gl_Position				= (mvpMatrix	* vec4(position, 1.0));
	
	vertex_incident 	= (mvMatrix		* vec4(position, 1.0)).xyz;
//vertex_normal			= (mvnMatrix	*	vec4(normal,   1.0)).xyz;
	vertex_normal			= mat3(mvnMatrix) * normal;

}
#endif



#ifdef FRAGMENT_SHADER

uniform		sampler2D		envmap;
uniform		vec4				obj_color;
uniform		mat4				mvnMatrixInv;

in				vec3				vertex_incident;
in				vec3				vertex_normal;

out				vec4				fragment_color;

void main()
{	
	vec3 i = normalize(vertex_incident);
	vec3 n = normalize(vertex_normal);
	vec3 r = normalize(reflect(i, n));
	vec3 pt = normalize(mat3(mvnMatrixInv) * r);
	
	
	float phi		= atan(pt.x, -pt.z);												// -Pi   .. Pi
	float theta	= atan(pt.y, sqrt(pt.x*pt.x + pt.z*pt.z));	// -Pi/2 .. Pi/2
		
	vec2 euler;
	euler.x			= (0.5 + phi/6.28318531);
	euler.y			= (0.5 - theta/3.14158265);

	fragment_color.rgb	=		obj_color.rgb								* 1.0
												+	texture(envmap, euler).rgb	* pow(dot(n, i), 4);
													
	
}

#endif