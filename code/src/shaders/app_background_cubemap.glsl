#version 410

#ifdef VERTEX_SHADER
out				vec2				texcoords;

void main()
{
	vec4 pos[4] 		= vec4[4](vec4(-1.0, -1.0, +0.0, +1.0), vec4(+1.0, -1.0, +0.0, +1.0), vec4(-1.0, +1.0, +0.0, +1.0), vec4(+1.0, +1.0, +0.0, +1.0));
	vec2 coords[4]	= vec2[4](	vec2(-320.0, -240.0),
															vec2(+320.0, -240.0),
															vec2(-320.0, +240.0),
															vec2(+320.0, +240.0)	);
	
	gl_Position = pos[gl_VertexID];
	texcoords		= coords[gl_VertexID];
}
#endif


#ifdef FRAGMENT_SHADER

uniform		samplerCube	envmap;
uniform		mat4				reproject;
in				vec2				texcoords;
out				vec4				fragment_color;

void main()
{
	vec3 azimut			= mat3(reproject) * vec3(texcoords, 1.0);
	fragment_color	= texture(envmap, azimut);
}

#endif