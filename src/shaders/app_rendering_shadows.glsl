#version 140

#define PI 			3.14159
#define SIZE		100


#ifdef VERTEX_SHADER

uniform		mat4				mvpMatrix;
uniform 	samplerCube	envmap;
uniform		int					method;
uniform		float 			bbox_radius = 1000;
uniform		float 			bbox_height	= -1.0;

out				vec3				vertex_position;

void main()
{
	vec3 pos[4]	= vec3[4](	vec3(-SIZE*bbox_radius, bbox_height, -SIZE*bbox_radius),
													vec3(-SIZE*bbox_radius, bbox_height, +SIZE*bbox_radius),
													vec3(+SIZE*bbox_radius, bbox_height, -SIZE*bbox_radius),
													vec3(+SIZE*bbox_radius, bbox_height, +SIZE*bbox_radius)	);
													
	gl_Position			= mvpMatrix * vec4(pos[gl_VertexID], 1.0);
	vertex_position = pos[gl_VertexID];
}
#endif




#ifdef FRAGMENT_SHADER

uniform		mat4				mvpMatrix;
uniform 	samplerCube	envmap;
uniform		int					method;
uniform		float 			sphere_size		= 1.0;
uniform		vec3				sphere_center	= vec3(0.0, 0.0, 0.0);

in				vec3				vertex_position;
out				vec4				fragment_color;

float energy(vec4	color) { return length(color.rgb); }

void main() 
{
	vec3	top						= vec3(0.0, 1.0, 0.0);
	
	vec3	direction		=	sphere_center - vertex_position;
	float distance		= length(direction);
		
	float	size				= textureSize(envmap, 0).x;
	float	viewsize		=	size * sphere_size / distance;

	float	ambiant			=	energy(	textureLod(envmap, vec3(+0.0, +1.0, +0.0), log2(size) 		) * 0.56
														+	textureLod(envmap, vec3(-1.0, +0.0, +0.0), log2(size) 		) * 0.11
														+	textureLod(envmap, vec3(+1.0, +0.0, +0.0), log2(size) 		) * 0.11
														+	textureLod(envmap, vec3(+0.0, +0.0, -1.0), log2(size) 		) * 0.11
														+	textureLod(envmap, vec3(+0.0, +0.0, +1.0), log2(size)			) * 0.11	);
	float	local				=	energy(	textureLod(envmap, direction,              log2(viewsize)	)					);
	
	
	float solidfactor	=	dot(top, normalize(direction)) * pow(sphere_size/distance, 2.0);
	float	occult			=	solidfactor * min(local/ambiant, PI);
	
	
	if (ambiant == 0.f)								fragment_color = vec4(0.f, 0.f, 0.f, 0.f);
	else if ((method & 0x0010) == 0)	fragment_color = vec4(0.f, 0.f, 0.f, occult);
	else															fragment_color = vec4(0.f, 0.f, 0.f, solidfactor);

}
#endif