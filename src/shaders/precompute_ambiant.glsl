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

in				vec3				vertex_position;
in				vec3				vertex_normal;

out				vec4				fragment_color;

void main() 
{	

	vec4	projected			=	vec4( mvp  * vec4(           vertex_position  , 1.0) );
	vec4	oriented			= vec4( mv   * vec4(normalize( vertex_normal   ), 0.0) );


	vec3	coords				= projected.xyz / projected.w / 2.0 + 0.5;
	float	depth					= texture(light_map, coords.xy).z;
		
		
		
	if ( coords.z <= depth+0.001 )
	{
		float n	= 4.0 / light_nb;  
		float v = abs(oriented.z);
		fragment_color = vec4(v*n, v*n, v*n, 0.0);
	}
	else
	{
		fragment_color = vec4(0.0, 0.0, 0.0, 0.0);
	}
}

#endif
