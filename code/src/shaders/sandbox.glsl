#version 410

#ifdef VERTEX_SHADER
out				vec2				texcoords;

void main()
{
	vec4 coords[4] 	= vec4[4](	vec4(-1.0, -1.0, +0.0, +1.0),
															vec4(+1.0, -1.0, +0.0, +1.0),
															vec4(-1.0, +1.0, +0.0, +1.0),
															vec4(+1.0, +1.0, +0.0, +1.0) );

	gl_Position = coords[gl_VertexID];
	texcoords		= vec2( coords[gl_VertexID].xy*0.5+0.5	);
}
#endif


#ifdef FRAGMENT_SHADER

uniform		vec2				diff;
uniform		sampler2D		data;

in				vec2				texcoords;
out				vec4				fragment_color;

void main()
{
	// fragment_color	= texture(data, texcoords);

	vec2 dX = dFdx(texcoords) * diff.x;
	vec2 dY = dFdy(texcoords) * diff.y;
	fragment_color = textureGrad(data, texcoords, dX, dY);

	// fragment_color = vec4(0.4, 0.6, 0.8, 1.0);
}


#endif
