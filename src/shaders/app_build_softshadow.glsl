#version 410

#define		PI		3.14159265

#ifdef VERTEX_SHADER

uniform												samplerCube	envmap;
uniform												int					method;
uniform												vec2 				bbox = vec2(100, -1.0);

layout(location = 3) in				vec4				sphere;
out														vec3				vertex_position;
out														vec4				vertex_sphere;

void main()
{
	vec2 coords[4] 	= vec2[4](	vec2(-1.0, -1.0),
															vec2(+1.0, -1.0),
															vec2(-1.0, +1.0),
															vec2(+1.0, +1.0) );
	vec3 pos[4]			= vec3[4]	(	vec3(-bbox.x, bbox.y, -bbox.x),
															vec3(+bbox.x, bbox.y, -bbox.x),
															vec3(-bbox.x, bbox.y, +bbox.x),
															vec3(+bbox.x, bbox.y, +bbox.x)	);

	gl_Position = vec4( coords[gl_VertexID].xy, 0.0, 1.0 );
	vertex_position = pos[gl_VertexID];
	vertex_sphere		= sphere;
}
#endif



#ifdef FRAGMENT_SHADER

uniform												samplerCube	envmap;
uniform												int					method;
uniform												vec2 				bbox = vec2(100, -1.0);

in														vec3				vertex_position;
in														vec4				vertex_sphere;
out														vec4				fragment_color;


float energy(vec4	color) 				{ return length(color.rgb); }

void main() 
{
	float value					= 0.f;
	vec3	top						= vec3(0.0, 1.0, 0.0);
	
	vec3	direction		=	vertex_sphere.xyz - vertex_position;
	float distance		= length(direction);
		
	float	size				= textureSize(envmap, 0).x;
	float	viewsize		=	size * vertex_sphere.w / distance;
	
	float	ambiant			=	energy(	textureLod(envmap, vec3(+0.0, +1.0, +0.0), log2(size) 		) * 0.56
														+	textureLod(envmap, vec3(-1.0, +0.0, +0.0), log2(size) 		) * 0.11
														+	textureLod(envmap, vec3(+1.0, +0.0, +0.0), log2(size) 		) * 0.11
														+	textureLod(envmap, vec3(+0.0, +0.0, -1.0), log2(size) 		) * 0.11
														+	textureLod(envmap, vec3(+0.0, +0.0, +1.0), log2(size)			) * 0.11	);
	float	local				=	energy(	textureLod(envmap, direction,              log2(viewsize)	)					);
		
	float solidfactor	=	dot(top, normalize(direction)) * pow(vertex_sphere.w/distance, 2.0);
	float	occult			=	solidfactor * min(local/ambiant, PI);
		
	if (ambiant == 0.f)								fragment_color = vec4(0.f, 0.f, 0.f, 0.f);
	else if ((method & 0x0010) == 0)	fragment_color = vec4(0.f, 0.f, 0.f, occult);
	else															fragment_color = vec4(0.f, 0.f, 0.f, solidfactor);
}
#endif
