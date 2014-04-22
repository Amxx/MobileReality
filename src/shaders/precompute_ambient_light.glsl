#version 410

#ifdef VERTEX_SHADER

uniform									mat4				mvp;
layout(location=0) in		vec3				position;

void main()
{
	gl_Position			= mvp * vec4(position, 1.0);
}
#endif




#ifdef FRAGMENT_SHADER

out											vec4				fragment_color;

void main() 
{	
	fragment_color = vec4(gl_FragCoord.zzz, 1.0);
}
#endif

