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
uniform		mat4				reproject;
uniform		vec3				cam_pos;
uniform		float				cam_rad;
uniform		float				sce_rad;

in				vec3				position;
out				vec4				fragment_color;

void main()
{
	vec4 cubecoords;
	if (sce_rad > 0.f)	cubecoords = reproject * vec4(normalize(normalize(position).xyz*sce_rad-cam_pos.xyz), 1.0 );
	else								cubecoords = reproject * vec4(normalize(          position                         ), 1.0 );	
	
	if (cubecoords.z <= 0.0) discard;
	
	vec2 coords				= vec2(cubecoords.x / cubecoords.z, cubecoords.y / cubecoords.z);
	vec2 centercoords	=	coords - vec2(320, 240);
	
	if (abs(centercoords.x) > 320 || abs(centercoords.y) > 240)								discard;
	if (cam_rad >= 0.0 && dot(centercoords, centercoords) > cam_rad*cam_rad)	discard;
	
	fragment_color = texture(frame, coords/vec2(640,480));
}


#endif