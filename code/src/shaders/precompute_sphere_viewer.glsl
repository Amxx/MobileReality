#version 410

#ifdef VERTEX_SHADER
uniform									mat4				mvp;

layout (location=0)	in	vec3				position;
layout (location=1)	in	vec3				texcoord;
layout (location=2)	in	vec3				normal;

out											vec3				vertex_position;
out											vec3				vertex_texcoord;
out											vec3				vertex_normal;

void main()
{
	gl_Position			= mvp * vec4(position, 1.0);
	vertex_position	=	position;
	vertex_texcoord	=	texcoord;
	vertex_normal		=	normal;
}
#endif






#ifdef FRAGMENT_SHADER
uniform									mat4				mvp;

in											vec3				vertex_position;
in											vec3				vertex_texcoord;
in											vec3				vertex_normal;

out											vec4				fragment_color;

void main() 
{	
	vec3 color = vec3(0.2, 0.6, 1.0);

	float n = dot(vertex_normal, normalize(vec3(0.1,1.0,0.3))) * 0.5f + 0.5f;
	fragment_color = vec4(color * n, 1.f);
}
#endif

