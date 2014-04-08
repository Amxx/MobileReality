#version 330

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
//	fragment_color = vec4(cos(.1* gl_FragCoord.x)/2.0+0.5,	// [0,1024] -> [0,1]
//												cos(.1* gl_FragCoord.y)/2.0+0.5,	// [0,1024] -> [0,1]
//												gl_FragCoord.z,										// [0,1]
//												1.0);

	fragment_color = vec4(gl_FragCoord.zzz, 1.0);

}
#endif

