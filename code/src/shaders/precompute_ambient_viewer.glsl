#version 330

#ifdef VERTEX_SHADER
uniform		mat4				mvp;
uniform		mat4				mv;

layout(location=0) in				vec3				position;
layout(location=1) in				vec3				texcoord;
layout(location=2) in				vec3				normal;

out				vec3				vertex_position;
out				vec3				vertex_texcoord;
out				vec3				vertex_normal;

void main()
{
	gl_Position			= mvp * vec4(position, 1.0);
	vertex_position	=	position;
	vertex_texcoord	=	texcoord;
	vertex_normal		=	normal;
}
#endif




#ifdef FRAGMENT_SHADER
uniform		sampler2D		blended;
uniform		mat4				mvp;
uniform		mat4				mv;

in				vec3				vertex_position;
in				vec3				vertex_texcoord;
in				vec3				vertex_normal;

out				vec4				fragment_color;

void main() 
{	
	fragment_color = texture(blended, vertex_texcoord.st);
}
#endif

