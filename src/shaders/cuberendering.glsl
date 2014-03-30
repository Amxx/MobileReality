#version 140

#ifdef VERTEX_SHADER
uniform		int					faceID;
out				vec3				position;

void main()
{
	vec3 pos[24] 		= vec3[24](	vec3(+1.0, +1.0, +1.0), 				vec3(+1.0, +1.0, -1.0),					vec3(+1.0, -1.0, +1.0),					vec3(+1.0, -1.0, -1.0),
															vec3(-1.0, +1.0, -1.0), 				vec3(-1.0, +1.0, +1.0),					vec3(-1.0, -1.0, -1.0),					vec3(-1.0, -1.0, +1.0),
															vec3(-1.0, +1.0, -1.0), 				vec3(+1.0, +1.0, -1.0),					vec3(-1.0, +1.0, +1.0),					vec3(+1.0, +1.0, +1.0),
															vec3(-1.0, -1.0, +1.0),					vec3(+1.0, -1.0, +1.0),					vec3(-1.0, -1.0, -1.0),					vec3(+1.0, -1.0, -1.0),
															vec3(-1.0, +1.0, +1.0),					vec3(+1.0, +1.0, +1.0),					vec3(-1.0, -1.0, +1.0),					vec3(+1.0, -1.0, +1.0),
															vec3(+1.0, +1.0, -1.0),					vec3(-1.0, +1.0, -1.0),					vec3(+1.0, -1.0, -1.0),					vec3(-1.0, -1.0, -1.0)				);
	vec4 coords[4]	= vec4[4](	vec4(-1.0, -1.0, +0.0, +1.0),		vec4(+1.0, -1.0, +0.0, +1.0),		vec4(-1.0, +1.0, +0.0, +1.0),		vec4(+1.0, +1.0, +0.0, +1.0)	);

	position		= pos[faceID*4+gl_VertexID];
	gl_Position = coords[gl_VertexID];
}
#endif


#ifdef FRAGMENT_SHADER

uniform		sampler2D		frame;
uniform		float				radius;

uniform		mat4				reproject;
in				vec3				position;
out				vec4				fragment_color;

void main()
{
	vec4 cubecoords	= reproject * vec4(normalize(position), 1.0);
	if (cubecoords.z <= 0.0) discard;
	
	vec2 coords				= vec2(cubecoords.x / cubecoords.z, cubecoords.y / cubecoords.z);
	vec2 centercoords	=	coords - vec2(320, 240);
	
	if (abs(centercoords.x) > 320 || abs(centercoords.y) > 240)						discard;
	if (radius >= 0.0 && dot(centercoords, centercoords) > radius*radius)	discard;
	
	fragment_color = texture(frame, coords/vec2(640,480));
}


#endif