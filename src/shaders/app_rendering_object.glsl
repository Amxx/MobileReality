#version 410

#define PI 3.14159



#ifdef VERTEX_SHADER

uniform												mat4				mvMatrix;
uniform												mat4				mvMatrixInv;
uniform												mat4				mvpMatrix;
uniform												int					method;
uniform												sampler2D		softshadow;
uniform												samplerCube	envmap;
uniform												vec2				bbox;

uniform												vec4				diffuse_color;
uniform												bool				use_diffuse_texture;
uniform												sampler2D		diffuse_texture;
uniform												float				kd;
uniform												vec4				specular_color;
uniform												bool				use_specular_texture;
uniform												sampler2D		specular_texture;
uniform												float				ks;
uniform												float				ns;
uniform												vec4				emission;
uniform												float				ni;

layout(location = 0) in				vec3				position;
layout(location = 1) in				vec3				texcoord;
layout(location = 2) in				vec3				normal;

out														vec3				vertex_position_v;
out														vec3				vertex_position_g;
out														vec3				vertex_normal_v;
out														vec3				vertex_normal_g;
out														vec3				vertex_texcoord;

out														vec4				diffuse_light;
out														float				diffuse_level;
out														float 			specular_level;


void main()
{
	gl_Position				= (mvpMatrix * vec4(position, 1.0));	
	vertex_position_v = (mvMatrix  * vec4(position, 1.0)).xyz;
	vertex_position_g = position;
	vertex_normal_v		= (mvMatrix  * vec4(normal,   0.0)).xyz;	
	vertex_normal_g		= normal;	
	vertex_texcoord		= texcoord;
	
	int		size				= textureSize(envmap, 0).x;
	diffuse_level			= log2(size);
	specular_level		= log2(size * sqrt(3.0)) - 0.5 * log2(ns + 1);
	
	vec3	n						=	normalize(normal);
	if ((method & 0x0001) == 0)
		diffuse_light		= (	textureLod(envmap, vec3(+1.0, +0.0, +0.0), diffuse_level) * pow(max(0.75 + n.x, 0), 2.0)
											+ textureLod(envmap, vec3(-1.0, +0.0, +0.0), diffuse_level) * pow(max(0.75 - n.x, 0), 2.0)
											+ textureLod(envmap, vec3(+0.0, +1.0, +0.0), diffuse_level) * pow(max(0.75 + n.y, 0), 2.0)
											+ textureLod(envmap, vec3(+0.0, -1.0, +0.0), diffuse_level) * pow(max(0.75 - n.y, 0), 2.0)
											+ textureLod(envmap, vec3(+0.0, +0.0, +1.0), diffuse_level) * pow(max(0.75 + n.z, 0), 2.0)
											+ textureLod(envmap, vec3(+0.0, +0.0, -1.0), diffuse_level) * pow(max(0.75 - n.z, 0), 2.0) ) / 1.75 / PI;
	else
		diffuse_light		= textureLod(envmap, n,	diffuse_level);
}
#endif




#ifdef FRAGMENT_SHADER
	
uniform												mat4				mvMatrix;
uniform												mat4				mvMatrixInv;
uniform												mat4				mvpMatrix;
uniform												int					method;
uniform												sampler2D		softshadow;
uniform												samplerCube	envmap;
uniform												vec2				bbox;

uniform												vec4				diffuse_color;
uniform												bool				use_diffuse_texture;
uniform												sampler2D		diffuse_texture;
uniform												float				kd;
uniform												vec4				specular_color;
uniform												bool				use_specular_texture;
uniform												sampler2D		specular_texture;
uniform												float				ks;
uniform												float				ns;
uniform												vec4				emission;
uniform												float				ni;

in														vec3				vertex_position_v;
in														vec3				vertex_position_g;
in														vec3				vertex_normal_v;
in														vec3				vertex_normal_g;
in														vec3				vertex_texcoord;

in														vec4				diffuse_light;
in														float				diffuse_level;
in														float 			specular_level;

out														vec4				fragment_color;

void main() 
{
	//if (ni > 0.f) discard;

	vec3	p_v						= vertex_position_v;
	vec3	p_g						= vertex_position_g;
	vec3	n_v						= normalize(vertex_normal_v);
	vec3	n_g						= normalize(vertex_normal_g);
	
	vec3	l_v						= reflect(normalize(p_v), n_v);
	vec3	l_g						= normalize(mat3(mvMatrixInv) * l_v);
	
	//-----------------------------------------------------------------
	
	vec4	env_diffuse		=		kd * diffuse_color  * diffuse_light;
	if ((method & 0x0002) == 0 && n_g.y < 0)
	{
		vec2 shadowpos		= p_g.xz + n_g.xz * (bbox.y - p_g.y) / n_g.y;
		vec2 shadowcoords	= shadowpos.xy / bbox.x / 2 + vec2(.5, .5); 
		if ( shadowcoords.x > 0 && shadowcoords.x < 1 && shadowcoords.y > 0 && shadowcoords.y < 1 )
			env_diffuse *= 1 - texture(softshadow, shadowcoords.xy).w;
	}
	if (use_diffuse_texture)	env_diffuse		*=	texture(diffuse_texture, vertex_texcoord.st);
	
	//-----------------------------------------------------------------
	
	vec4	env_specular	=		ks * specular_color * textureLod(envmap, l_g, specular_level);
	if ((method & 0x0004) == 0 && l_g.y < 0)
	{
		vec2 shadowpos		= p_g.xz + l_g.xz * (bbox.y - p_g.y) / l_g.y;
		vec2 shadowcoords	= shadowpos.xy / bbox.x / 2 + vec2(.5, .5); 
		if ( shadowcoords.x > 0 && shadowcoords.x < 1 && shadowcoords.y > 0 && shadowcoords.y < 1 )
			env_specular *= 1 - texture(softshadow, shadowcoords.xy).w;
	}
	if (use_specular_texture)	env_specular	*=	texture(specular_texture, vertex_texcoord.st);
	
	//-----------------------------------------------------------------
	
	fragment_color = vec4(env_diffuse.rgb + env_specular.rgb + emission.rgb, 1.0);

	// ================================================================
	// =										S H O W   C U B E M A P										=
	// ================================================================
	//fragment_color				= texture(envmap, p_g);
}
#endif
