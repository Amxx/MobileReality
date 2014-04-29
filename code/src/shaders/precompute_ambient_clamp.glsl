#version 410

#ifdef VERTEX_SHADER
out				vec2				texcoords;

void main()
{
	vec2 coords[4] 	= vec2[4](	vec2(-1.0, -1.0),
															vec2(-1.0, +1.0),
															vec2(+1.0, -1.0),
															vec2(+1.0, +1.0) );

	gl_Position = vec4( coords[gl_VertexID].xy, 0.0, 1.0 );
	texcoords		= vec2( coords[gl_VertexID].xy / 2 + 0.5 );
}
#endif


#ifdef FRAGMENT_SHADER

uniform		sampler2D		data;
in				vec2				texcoords;
out				vec4				fragment_color;

void main()
{
	fragment_color	= vec4( clamp(texture(data, texcoords).rgba, 0.f, 1.f) );
}


#endif
