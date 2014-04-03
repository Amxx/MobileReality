#version 140

#ifdef VERTEX_SHADER

in				vec3				position;
in				vec3				texcoord;
in				vec3				normal;

out				vec3				vertex_position;
out				vec3				vertex_normal;

void main()
{
	gl_Position			= vec4( 2*texcoord.st-vec2(1.0, 1.0), 0.0, 1.0 );
	vertex_position	=	position;
	vertex_normal		=	normal;
}

#endif




#ifdef FRAGMENT_SHADER
uniform		mat4				mv;
uniform		mat4				mvp;
uniform		sampler2D		light_map;
uniform		int					light_nb;
uniform		int					idx;

in				vec3				vertex_position;
in				vec3				vertex_normal;

out				vec4				fragment_color;

void main() 
{	

	vec4	projected			=	vec4( mvp  * vec4(           vertex_position  , 1.0) );
	vec4	oriented			= vec4( mv   * vec4(normalize( vertex_normal   ), 0.0) );


	vec2	coords				= vec2(	.5+projected.x/10,
															.5+projected.y/10	);

	fragment_color 			= 0.8 * texture(light_map, coords)
											+	0.2	*	vec4(1.0, 1.0, 1.0, 1.0);


	/*
	float n	= (2.0 / light_nb);  
	float v = oriented.z;
	
	float ddist = texture(light_map, coords).z - projected.z;

	switch (idx%3)
	{
		case 0:		fragment_color = vec4(v*n, 0.0, 0.0, 1.0); break;
		case 1:		fragment_color = vec4(0.0, v*n, 0.0, 1.0); break;
		case 2:		fragment_color = vec4(0.0, 0.0, v*n, 1.0); break;
		default:	fragment_color = vec4(v*n, v*n, v*n, 1.0); break;
	}
//	fragment_color = texture( light_map, coords );
//	fragment_color = vec4(coords, 0.0, 1.0);
	*/




}

#endif
