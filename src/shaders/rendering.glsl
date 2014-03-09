#version 140



#ifdef VERTEX_SHADER

uniform		mat4				mvMatrix;
uniform		mat4				mvpMatrix;
uniform		mat4				mvnMatrix;

in				vec3				position;
in				vec3				normal;
in				vec3				texcoord;

out				vec3				light_position;
out				vec3				vertex_position;
out				vec3				vertex_normal;
out				vec2				vertex_texcoord;


void main()
{


	gl_Position				= (mvpMatrix	* vec4(position, 1.0));
	
	light_position		= (mvMatrix		* vec4(0.0, 100.0, 0.0, 1.0)).xyz;
	
	vertex_position 	= (mvMatrix		* vec4(position, 1.0)).xyz;
	vertex_normal			= (mvnMatrix	* vec4(normal, 1.0)).xyz;
	vertex_texcoord		= texcoord.st;

}
#endif



#ifdef FRAGMENT_SHADER

uniform		vec4				obj_color;
uniform		vec4				light_color;

in				vec3				light_position;
in				vec3				vertex_position;
in				vec3				vertex_normal;
in				vec2				vertex_texcoord;

out				vec4				fragment_color;

void main()
{

		//vec3 l	= vec3(0.0, 1.0, 0.0);
		vec3 l	= normalize(light_position);
		
		vec3 n	= normalize(vertex_normal);
		vec3 p	= normalize(vertex_position);
		vec3 lp	= normalize(l-p);
		
		float luminance 		=		1.0
													+ 1.0 * dot(n, l)
													+ 0.5 * pow(dot(n, lp), 64);

		fragment_color.rgb	=		obj_color.rgb				* (1.0 + dot(n, l)) * 0.5
													+ light_color.rgb			* (1.0 + dot(n, l)) * 0.2
													+ light_color.rgb			* pow(dot(n, lp), 64);

}

#endif