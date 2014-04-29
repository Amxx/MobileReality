#version 410

#define PI 3.14159

#ifdef VERTEX_SHADER

uniform												mat4				mvMatrix;
uniform												mat4				mvMatrixInv;
uniform												mat4				mvpMatrix;
uniform												int					method;
uniform												sampler2D		softshadow;
uniform												samplerCube	envmap;
uniform												vec3				bbox;

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
	
	float	size				= float(textureSize(envmap, 0).x);
	diffuse_level			= log2(size);
	specular_level		= log2(size * sqrt(3.0)) - 0.5 * log2(ns + 1.0);
	
	vec3	n						=	normalize(normal);
	if ((method & 0x0001) == 0)
		diffuse_light		= (	textureLod(envmap, vec3(+1.0, +0.0, +0.0), diffuse_level) * pow(max(0.75 + n.x, 0.0), 2.0)
											+ textureLod(envmap, vec3(-1.0, +0.0, +0.0), diffuse_level) * pow(max(0.75 - n.x, 0.0), 2.0)
											+ textureLod(envmap, vec3(+0.0, +1.0, +0.0), diffuse_level) * pow(max(0.75 + n.y, 0.0), 2.0)
											+ textureLod(envmap, vec3(+0.0, -1.0, +0.0), diffuse_level) * pow(max(0.75 - n.y, 0.0), 2.0)
											+ textureLod(envmap, vec3(+0.0, +0.0, +1.0), diffuse_level) * pow(max(0.75 + n.z, 0.0), 2.0)
											+ textureLod(envmap, vec3(+0.0, +0.0, -1.0), diffuse_level) * pow(max(0.75 - n.z, 0.0), 2.0) ) / 1.75 / PI;
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
uniform												vec3				bbox;

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




float erfc(float x)
{
	return 2.0*exp(-x*x)/(2.319*x+sqrt(4.0+1.52*x*x));
}
vec2 simpleDs(float gaussian, float cos_theta)
{
	float	sin_theta	=	1.0-sqrt(cos_theta*cos_theta);
	float	cot_theta	=	cos_theta/sin_theta;
	float	E1				=	sqrt(0.5*gaussian/PI)*exp(-0.5*cot_theta*cot_theta/gaussian);
	float	E2				=	1.0-0.5*erfc(cot_theta/sqrt(2.0*gaussian));
	float	I2				=	-cot_theta*E1+gaussian*E2;
	float	I3				=	-2.0*gaussian*E1-cot_theta*cot_theta*E1;
	float	N					=	(+sin_theta*E1+cos_theta*E2);
	float	ds_para0	=	(-sin_theta*I2-cos_theta*E1)/N;
	float	shift			=	ds_para0;
	float	aniso			=	(-sin_theta*I3+cos_theta*I2)/N-ds_para0*ds_para0;
	return vec2(shift, aniso);
}


void main() 
{
	//if (ni > 0.f) discard;

	vec3	p_v		= vertex_position_v;
	vec3	p_g		= vertex_position_g;
	vec3	n_v		= normalize(vertex_normal_v);
	vec3	n_g		= normalize(vertex_normal_g);


	vec2	Ds		= simpleDs(1.0/(ns+1.0), dot(n_v, normalize(p_v)));
	vec3	Dn_v	=	normalize(n_v+Ds.x*normalize(p_v));
	
	vec3	l_v		= ((method & 0x0000)==0) ? normalize(reflect(normalize(p_v), n_v)) : normalize(reflect(normalize(p_v), Dn_v));
	vec3	l_g		= normalize(mat3(mvMatrixInv) * l_v);

	//-----------------------------------------------------------------
	
	vec4	env_diffuse = kd * diffuse_color  * diffuse_light;
	if ((method & 0x0002) == 0 && n_g.y < 0.0)
	{
		float	dist					= (bbox.y - p_g.y) / n_g.y;
		vec2	shadowpos			= p_g.xz + dist * n_g.xz;
		vec2	shadowcoords	= shadowpos.xy / bbox.xz / 2.0 + vec2(.5, .5);
		float shadowlevel		= log2(2.0*dist);

		if ( shadowcoords.x > 0.0 && shadowcoords.x < 1.0 && shadowcoords.y > 0.0 && shadowcoords.y < 1.0 )
			env_diffuse *= 1.0 - textureLod(softshadow, shadowcoords.xy, shadowlevel).w;
	}
	if (use_diffuse_texture) env_diffuse *= texture(diffuse_texture, vertex_texcoord.st);
	
	//-----------------------------------------------------------------
	




	vec4	env_specular;

	if ((method & 0x0008)==0)
	{
		env_specular = ks * specular_color * textureLod(envmap, l_g, specular_level);
	}
	else
	{
		// vec2	dF	= vec2( 100.f, 100.f )
		// 					* length(dFdx(l_g) + dFdy(l_g));
		// vec3	dX	= normalize(n_g-dot(n_g, l_g)*l_g	) * dF.x;
		// vec3	dY	= normalize(cross(n_g, l_g)				) * dF.y;
		// env_specular	= ks * specular_color * textureGrad(envmap, l_g, dX, dY);

		vec3	dx				= dFdx(l_g);
		vec3	dy				= dFdy(l_g);
		float	dlength		=	length(dx+dy);
		vec3	ddir			= normalize(dx * dot(dx, l_g) + dy * dot(l_g, dy)) * dlength * 1.0;
		vec3	dort			= normalize(dy * dot(dx, l_g) - dx * dot(l_g, dy)) * dlength * 1.0;
		env_specular	= ks * specular_color * textureGrad(envmap, l_g, ddir, dort);
	}


	if ((method & 0x0004) == 0 && l_g.y < 0.0)
	{
		float	dist				= (bbox.y - p_g.y) / l_g.y;
		vec2 shadowpos		= p_g.xz + dist * l_g.xz; 
		vec2 shadowcoords	= shadowpos.xy / bbox.xz / 2.0 + vec2(0.5, 0.5);
		float shadowlevel	= log2(2.0*dist * sqrt(3.0)) - 0.5 * log2(ns + 1.0);

		if ( shadowcoords.x > 0.0 && shadowcoords.x < 1.0 && shadowcoords.y > 0.0 && shadowcoords.y < 1.0 )
			env_specular *= 1.0 - textureLod(softshadow, shadowcoords.xy, shadowlevel).w;
	}
	if (use_specular_texture) env_specular *= texture(specular_texture, vertex_texcoord.st);
	
	//-----------------------------------------------------------------
	
	fragment_color = vec4(env_diffuse.rgb + env_specular.rgb + emission.rgb, 1.0);

	// ================================================================
	// =										S H O W   C U B E M A P										=
	// ================================================================
	//fragment_color				= texture(envmap, p_g);
}
#endif
