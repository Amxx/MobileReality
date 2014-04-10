#version 140

#define PI 			3.14159
#define SIZE		100



#ifdef VERTEX_SHADER

uniform		mat4				mvpMatrix;
uniform 	samplerCube	envmap;
uniform		int					method;

out				vec3				vertex_position;

void main()
{
	vec3 pos[4]	= vec3[4](	vec3(-SIZE, -1.0, -SIZE),
													vec3(-SIZE, -1.0, +SIZE),
													vec3(+SIZE, -1.0, -SIZE),
													vec3(+SIZE, -1.0, +SIZE)	);
													
	gl_Position			= mvpMatrix * vec4(pos[gl_VertexID], 1.0);
	vertex_position = pos[gl_VertexID];
}
#endif




#ifdef FRAGMENT_SHADER

uniform		mat4				mvpMatrix;
uniform 	samplerCube	envmap;
uniform		int					method;

in				vec3				vertex_position;

out				vec4				fragment_color;

float energy(vec4	color) { return length(color.rgb); }

void main() 
{
	float sphere_size		= 1.0;
	vec3	sphere_center	= vec3(0.0, 0.0, 0.0);
	vec3	top						= vec3(0.0, 1.0, 0.0);
	
	vec3	direction		=	sphere_center - vertex_position;
	float distance		= length(direction);
		
	int		size				= textureSize(envmap, 0).x;
	vec4	ambiant			=	(	textureLod(envmap, vec3(+0.0, +1.0, +0.0), log2(size)                     ) * 1.75
											+	textureLod(envmap, vec3(-1.0, +0.0, +0.0), log2(size)                     ) * 0.35
											+	textureLod(envmap, vec3(+1.0, +0.0, +0.0), log2(size)                     ) * 0.35
											+	textureLod(envmap, vec3(+0.0, +0.0, -1.0), log2(size)                     ) * 0.35
											+	textureLod(envmap, vec3(+0.0, +0.0, +1.0), log2(size)                     ) * 0.35 ) / 3.15;
	vec4	local				=		textureLod(envmap, direction,              log2(size*sphere_size/distance));
	
	float solidfactor	= 	dot(top, normalize(direction))
											*	pow(sin(atan(sphere_size, distance)), 2.0);
											
	float	occult			=	solidfactor * energy(local) / energy(ambiant);
	
	if ((method & 0x2) == 0)
		fragment_color = vec4(0.0, 0.0, 0.0, 2*occult);
	else
		fragment_color = vec4(0.0, 0.0, 0.0, 2*solidfactor);
		
}
#endif
