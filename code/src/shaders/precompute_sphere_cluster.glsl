/*
Copyright or © or Copr. Hadrien Croubois (2014)

hadrien.croubois@ens-lyon.fr

This software is a computer program whose purpose is to render, in real
time, virtual objects usgin dynamically acquired environnement.

This software is governed by the CeCILL-B license under French law and
abiding by the rules of distribution of free software.  You can  use, 
modify and/ or redistribute the software under the terms of the CeCILL-B
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info". 

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability. 

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or 
data to be ensured and,  more generally, to use and operate it in the 
same conditions as regards security. 

The fact that you are presently reading this means that you have had
knowledge of the CeCILL-B license and that you accept its terms.
*/

#version 410



#ifdef VERTEX_SHADER
out				vec3				vertex_position;

void main()
{
	int faces[60]	= int[60]		(	2,	1,	0,
															3,	2,	0,
															4,	3,	0,
															5,	4,	0,
															1,	5,	0,
															11, 6,  7,
															11, 7,  8,
															11, 8,  9,
															11, 9,  10,
															11, 10, 6,
															1,	2,	6,
															2,	3,	7,
															3,	4,	8,
															4,	5,	9,
															5,	1,	10,
															2,  7,	6,
															3,  8,	7,
															4,  9,	8,
															5,	10, 9,
															1,	6,	10 );
	vec3 pos[12]	= vec3[12]	(	vec3(+0.000, +0.000, +1.000),
															vec3(+0.894, +0.000, +0.447),
															vec3(+0.276, +0.851, +0.447),
															vec3(-0.724, +0.526, +0.447),
															vec3(-0.724, -0.526, +0.447),
															vec3(+0.276, -0.851, +0.447),
															vec3(+0.724, +0.526, -0.447),
															vec3(-0.276, +0.851, -0.447),
															vec3(-0.894, +0.000, -0.447),
															vec3(-0.276, -0.851, -0.447),
															vec3(+0.724, -0.526, -0.447),
															vec3(+0.000, +0.000, -1.000) );
															
	vertex_position = pos[faces[gl_VertexID]];
}
#endif



#ifdef CONTROL_SHADER
layout(vertices = 3) out;
uniform		vec2				tesselation_level = vec2(3.0, 3.0);
in				vec3				vertex_position[];
out				vec3				control_position[];
 
void main(void)
{
	control_position[gl_InvocationID] = vertex_position[gl_InvocationID];
	if (gl_InvocationID == 0)
	{
		gl_TessLevelInner[0] = tesselation_level.x;
		gl_TessLevelOuter[0] = tesselation_level.y;
		gl_TessLevelOuter[1] = tesselation_level.y;
		gl_TessLevelOuter[2] = tesselation_level.y;
	}
}
#endif



#ifdef EVALUATION_SHADER
layout(triangles, equal_spacing, cw) in;
uniform 	mat4 				mvp;
uniform		vec4				sphere;
in				vec3				control_position[];
out				vec3				evaluation_position;
out				vec3				evaluation_normal;

void main( void )
{
	vec3 p0 = gl_TessCoord.x * control_position[0];
  vec3 p1 = gl_TessCoord.y * control_position[1];
  vec3 p2 = gl_TessCoord.z * control_position[2];
	
	evaluation_normal		= normalize(p0 + p1 + p2);
	evaluation_position	= evaluation_normal * sphere.w + sphere.xyz;
	gl_Position					= mvp * vec4(evaluation_position, 1.0);
	
}
#endif


#ifdef GEOMETRY_SHADER
layout(triangles) in;
layout(line_strip, max_vertices = 3) out;
in				vec3				evaluation_position[];
in				vec3				evaluation_normal[];
out				vec3				geometry_position;
out				vec3				geometry_normal;
 
void main()
{
	for (int i=0; i<3; ++i)
	{
		gl_Position				= gl_in[i].gl_Position;
		geometry_position	= evaluation_position[i];
		geometry_normal		= evaluation_normal[i];
		EmitVertex();
	}
	EndPrimitive();
}
#endif




#ifdef FRAGMENT_SHADER
in				vec3				evaluation_position;
in				vec3				evaluation_normal;
out				vec4				fragment_color;

void main()
{
	vec3 color = vec3(1.0, 0.6, 0.2);

	float n = dot(evaluation_normal, normalize(vec3(0.1, 1.0, 0.3))) * 0.5 + 0.5;
	fragment_color = vec4(color * n, 1.0);
	
//fragment_color = vec4(evaluation_normal * 0.5f + 0.5f, 1.f);	
//fragment_color = vec4(1.f, 1.f, 1.f, 1.f);
}
#endif