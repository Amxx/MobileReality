#version 410

#define		PI		3.14159265

#ifdef VERTEX_SHADER

uniform												mat4				mvpMatrix;
uniform												vec2 				bbox = vec2(100, -1.0);
out														vec2				vertex_texcoords;

void main()
{
	vec2 coords[4] 	= vec2[4](	vec2(+0.0, +0.0),
															vec2(+0.0, +1.0),
															vec2(+1.0, +0.0),
															vec2(+1.0, +1.0) );
	vec3 pos[4]			= vec3[4]	(	vec3(-bbox.x, bbox.y, -bbox.x),
															vec3(-bbox.x, bbox.y, +bbox.x),
															vec3(+bbox.x, bbox.y, -bbox.x),
															vec3(+bbox.x, bbox.y, +bbox.x)	);

	gl_Position				= mvpMatrix * vec4( pos[gl_VertexID].xyz, 1.0 );
	vertex_texcoords	= coords[gl_VertexID];
}
#endif



#ifdef FRAGMENT_SHADER

uniform												sampler2D		softshadow;
in														vec2				vertex_texcoords;
out														vec4				fragment_color;


void main() 
{
	fragment_color = texture(softshadow, vertex_texcoords);
}
#endif
