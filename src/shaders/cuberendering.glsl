#version 140

#ifdef VERTEX_SHADER
uniform		mat4				mvp;
uniform		mat4				reproject;
out				vec2				texcoords;

void main()
{
	vec4 pos[4] 		= vec4[4](vec4(+000.0, +000.0, +1.0, +1.0),
														vec4(+640.0, +000.0, +1.0, +1.0),
														vec4(+000.0, +480.0, +1.0, +1.0),
														vec4(+640.0, +480.0, +1.0, +1.0));
	vec2 coords[4]	= vec2[4](vec2(0.0, 0.0),
														vec2(1.0, 0.0),
														vec2(0.0, 1.0),
														vec2(1.0, 1.0));

	gl_Position			= mvp * reproject * pos[gl_VertexID];
	texcoords				= coords[gl_VertexID];
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