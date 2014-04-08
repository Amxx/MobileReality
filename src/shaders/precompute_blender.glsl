#version 330

#ifdef VERTEX_SHADER
uniform		int					faceID;
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

uniform		sampler2D		ambiant;
uniform		int					light_nb;
in				vec2				texcoords;
out				vec4				fragment_color;

void main()
{
	float n					= 4.0 / light_nb;
	fragment_color	= vec4( texture(ambiant, texcoords).rgb * n, 1.0 );
}


#endif