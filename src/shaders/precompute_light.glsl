#version 140

#ifdef VERTEX_SHADER

uniform		mat4				mvp;
in				vec3				position;

void main()
{
	gl_Position			= mvp * vec4(position, 1.0);
}
#endif




#ifdef FRAGMENT_SHADER

out				vec4				fragment_color;

void main() 
{	
	fragment_color = vec4(gl_FragCoord.x/1024,
												gl_FragCoord.y/1024,
												gl_FragCoord.z,
												1.0);
}
#endif

