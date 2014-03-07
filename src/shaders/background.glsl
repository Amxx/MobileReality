#version 330

#ifdef VERTEX_SHADER
out				vec2				texcoords;

void main()
{
	vec4 pos[8] 		= vec4[8](vec4(-1.0, -1.0, +0.0, +1.0), vec4(+1.0, -1.0, +0.0, +1.0), vec4(-1.0, +1.0, +0.0, +1.0), vec4(+1.0, +1.0, +0.0, +1.0),
														vec4(-1.0, -1.0, +0.0, +1.0), vec4(+1.0, -1.0, +0.0, +1.0), vec4(-1.0, +1.0, +0.0, +1.0), vec4(+1.0, +1.0, +0.0, +1.0));
														
	vec2 coords[8]	= vec2[8](vec2(0.0, 0.0), vec2(1.0, 0.0), vec2(0.0, 1.0), vec2(1.0, 1.0),
														vec2(1.0, 1.0), vec2(0.0, 1.0), vec2(1.0, 0.0), vec2(0.0, 0.0));
	
	gl_Position = pos[gl_VertexID];
	texcoords		= coords[gl_VertexID];
}
#endif


#ifdef FRAGMENT_SHADER

uniform		sampler2D		frame;
in				vec2				texcoords;
out				vec4				fragment_color;

void main()
{
	fragment_color = texture(frame, texcoords);
}

#endif