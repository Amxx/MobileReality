#version 330

#ifdef VERTEX_SHADER
uniform		mat4				mvpMatrix;
uniform		float				scale;
in				vec3				position;
in				vec3				normal;

void main()
{
	gl_Position = mvpMatrix * vec4(scale * position, 1.0);
}
#endif


#ifdef FRAGMENT_SHADER

out				vec4				fragment_color;

void main()
{
	fragment_color = vec4(1.0, 1.0, 1.0, 1.0);
}

#endif