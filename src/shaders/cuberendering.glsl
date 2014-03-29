#version 140

#ifdef VERTEX_SHADER
uniform		int					faceID;
out				vec3				position;

void main()
{
	vec3 pos[24] 		= vec3[24](	vec3(+1.0, +1.0, +1.0),
															vec3(+1.0, +1.0, -1.0),
															vec3(+1.0, -1.0, +1.0),
															vec3(+1.0, -1.0, -1.0),
															
															vec3(-1.0, +1.0, -1.0),
															vec3(-1.0, +1.0, +1.0),
															vec3(-1.0, -1.0, -1.0),
															vec3(-1.0, -1.0, +1.0),
																
															vec3(-1.0, +1.0, -1.0),
															vec3(+1.0, +1.0, -1.0),
															vec3(-1.0, +1.0, +1.0),
															vec3(+1.0, +1.0, +1.0),
																
															vec3(-1.0, -1.0, +1.0),
															vec3(+1.0, -1.0, +1.0),
															vec3(-1.0, -1.0, -1.0),
															vec3(+1.0, -1.0, -1.0),
																
															vec3(-1.0, +1.0, +1.0),
															vec3(+1.0, +1.0, +1.0),
															vec3(-1.0, -1.0, +1.0),
															vec3(+1.0, -1.0, +1.0),
																
															vec3(+1.0, +1.0, -1.0),
															vec3(-1.0, +1.0, -1.0),
															vec3(+1.0, -1.0, -1.0),
															vec3(-1.0, -1.0, -1.0)	);	
															
	vec4 coords[4]	= vec4[4](	vec4(-1.0, -1.0, +0.0, +1.0),
															vec4(+1.0, -1.0, +0.0, +1.0),
															vec4(-1.0, +1.0, +0.0, +1.0),
															vec4(+1.0, +1.0, +0.0, +1.0)	);

	position		= pos[faceID*4+gl_VertexID];
	gl_Position = coords[gl_VertexID];
}
#endif


#ifdef FRAGMENT_SHADER

uniform		int					faceID;
	
uniform		sampler2D		frame;
uniform		mat4				reproject;
in				vec3				position;
out				vec4				fragment_color;

void main()
{
	vec4 cube_coord		= reproject * vec4(position, 1.0);
	if (		cube_coord.x >= 0.0 && cube_coord.x <= 640.0
			&&	cube_coord.y >= 0.0 && cube_coord.y <= 480.0
			&&	cube_coord.z >= 0.9)
		fragment_color		= texture(frame, vec2(cube_coord.x / 640, cube_coord.y / 480));
	else
		discard;
	
	//fragment_color = vec4(position, 1.0 );
}

#endif