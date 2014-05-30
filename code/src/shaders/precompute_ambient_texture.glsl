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
layout(location=0) in		vec3				position;
layout(location=1) in		vec3				texcoord;
layout(location=2) in		vec3				normal;
out											vec3				vertex_position;
out											vec3				vertex_normal;

void main()
{
	gl_Position			= vec4( 2.0*texcoord.st-vec2(1.0, 1.0), 0.0, 1.0 );
	vertex_position	=	position;
	vertex_normal		=	normal;
}

#endif



#ifdef GEOMETRY_SHADER

uniform								float				width;
uniform								float				height;


layout(triangles) in;
layout(triangle_strip) out;
layout(max_vertices = 9) out;

in										vec3				vertex_position[];
in										vec3				vertex_normal[];
out										vec3				geometry_position;
out										vec3				geometry_normal;

vec4 BL(vec4 p)	{	return vec4(p.x-2.0/width, p.y-2.0/height, p.zw); }
vec4 BR(vec4 p) { return vec4(p.x+2.0/width, p.y-2.0/height, p.zw); }
vec4 TL(vec4 p)	{	return vec4(p.x-2.0/width, p.y+2.0/height, p.zw); }
vec4 TR(vec4 p)	{	return vec4(p.x+2.0/width, p.y+2.0/height, p.zw); }


void main()
{

	vec4	vertices_glpos[9];
	vec3	vertices_pos	[9];
	vec3	vertices_nor	[9];
	int		verticesID = 0;
	
	vec2	d1	= gl_in[1].gl_Position.xy - gl_in[0].gl_Position.xy;
	vec2	d2	= gl_in[2].gl_Position.xy - gl_in[0].gl_Position.xy;
	bool	o		=	(d1.x*d2.y - d1.y*d2.x >= 0.f);
	
	for(int i=0; i<3; ++i)
	{
		int idx = 0;
		idx += (gl_in[i].gl_Position.x > gl_in[(i+(o?1:2))%3].gl_Position.x)?0x8:0x0;			// x1
		idx	+= (gl_in[i].gl_Position.y > gl_in[(i+(o?1:2))%3].gl_Position.y)?0x4:0x0;			// y1
		idx	+= (gl_in[i].gl_Position.x > gl_in[(i+(o?2:1))%3].gl_Position.x)?0x2:0x0;			// x2
		idx	+= (gl_in[i].gl_Position.y > gl_in[(i+(o?2:1))%3].gl_Position.y)?0x1:0x0;			// y2

		switch(idx)
		{
			case 0x1: 
			case 0x7:
			case 0x8:
			case 0xE:
				break;
			
			case 0x3:
				vertices_glpos[verticesID] = BR(gl_in[i].gl_Position);
				vertices_pos	[verticesID] = vertex_position[i];
				vertices_nor	[verticesID] = vertex_normal[i];
				verticesID++;
				break;
			case 0x6:
				vertices_glpos[verticesID] = BL(gl_in[i].gl_Position);
				vertices_pos	[verticesID] = vertex_position[i];
				vertices_nor	[verticesID] = vertex_normal[i];
				verticesID++;
				break;
			case 0x9:
				vertices_glpos[verticesID] = TR(gl_in[i].gl_Position);
				vertices_pos	[verticesID] = vertex_position[i];
				vertices_nor	[verticesID] = vertex_normal[i];
				verticesID++;
				break;
			case 0xC:
				vertices_glpos[verticesID] = TL(gl_in[i].gl_Position);
				vertices_pos	[verticesID] = vertex_position[i];
				vertices_nor	[verticesID] = vertex_normal[i];
				verticesID++;
				break;
			
			case 0x2:
				vertices_glpos[verticesID] = o?BL(gl_in[i].gl_Position):BR(gl_in[i].gl_Position);
				vertices_pos	[verticesID] = vertex_position[i];
				vertices_nor	[verticesID] = vertex_normal[i];
				verticesID++;
				vertices_glpos[verticesID] = o?BR(gl_in[i].gl_Position):BL(gl_in[i].gl_Position);
				vertices_pos	[verticesID] = vertex_position[i];
				vertices_nor	[verticesID] = vertex_normal[i];
				verticesID++;
				break;
			case 0x4:
				vertices_glpos[verticesID] = o?TL(gl_in[i].gl_Position):BL(gl_in[i].gl_Position);
				vertices_pos	[verticesID] = vertex_position[i];
				vertices_nor	[verticesID] = vertex_normal[i];
				verticesID++;
				vertices_glpos[verticesID] = o?BL(gl_in[i].gl_Position):TL(gl_in[i].gl_Position);
				vertices_pos	[verticesID] = vertex_position[i];
				vertices_nor	[verticesID] = vertex_normal[i];
				verticesID++;
				break;
			
			case 0xB:
				vertices_glpos[verticesID] = o?BR(gl_in[i].gl_Position):TR(gl_in[i].gl_Position);
				vertices_pos	[verticesID] = vertex_position[i];
				vertices_nor	[verticesID] = vertex_normal[i];
				verticesID++;
				vertices_glpos[verticesID] = o?TR(gl_in[i].gl_Position):BR(gl_in[i].gl_Position);
				vertices_pos	[verticesID] = vertex_position[i];
				vertices_nor	[verticesID] = vertex_normal[i];
				verticesID++;
				break;
			case 0xD:
				vertices_glpos[verticesID] = o?TR(gl_in[i].gl_Position):TL(gl_in[i].gl_Position);
				vertices_pos	[verticesID] = vertex_position[i];
				vertices_nor	[verticesID] = vertex_normal[i];
				verticesID++;
				vertices_glpos[verticesID] = o?TL(gl_in[i].gl_Position):TR(gl_in[i].gl_Position);
				vertices_pos	[verticesID] = vertex_position[i];
				vertices_nor	[verticesID] = vertex_normal[i];
				verticesID++;
				break;
				
			case 0x0:
				vertices_glpos[verticesID] = o?TL(gl_in[i].gl_Position):BR(gl_in[i].gl_Position);
				vertices_pos	[verticesID] = vertex_position[i];
				vertices_nor	[verticesID] = vertex_normal[i];
				verticesID++;
				vertices_glpos[verticesID] = o?BL(gl_in[i].gl_Position):BL(gl_in[i].gl_Position);
				vertices_pos	[verticesID] = vertex_position[i];
				vertices_nor	[verticesID] = vertex_normal[i];
				verticesID++;
				vertices_glpos[verticesID] = o?BR(gl_in[i].gl_Position):TL(gl_in[i].gl_Position);
				vertices_pos	[verticesID] = vertex_position[i];
				vertices_nor	[verticesID] = vertex_normal[i];
				verticesID++;
				break;
			case 0x5:
				vertices_glpos[verticesID] = o?TR(gl_in[i].gl_Position):BL(gl_in[i].gl_Position);
				vertices_pos	[verticesID] = vertex_position[i];
				vertices_nor	[verticesID] = vertex_normal[i];
				verticesID++;
				vertices_glpos[verticesID] = o?TL(gl_in[i].gl_Position):TL(gl_in[i].gl_Position);
				vertices_pos	[verticesID] = vertex_position[i];
				vertices_nor	[verticesID] = vertex_normal[i];
				verticesID++;
				vertices_glpos[verticesID] = o?BL(gl_in[i].gl_Position):TR(gl_in[i].gl_Position);
				vertices_pos	[verticesID] = vertex_position[i];
				vertices_nor	[verticesID] = vertex_normal[i];
				verticesID++;
				break;
			case 0xA:
				vertices_glpos[verticesID] = o?BL(gl_in[i].gl_Position):TR(gl_in[i].gl_Position);
				vertices_pos	[verticesID] = vertex_position[i];
				vertices_nor	[verticesID] = vertex_normal[i];
				verticesID++;
				vertices_glpos[verticesID] = o?BR(gl_in[i].gl_Position):BR(gl_in[i].gl_Position);
				vertices_pos	[verticesID] = vertex_position[i];
				vertices_nor	[verticesID] = vertex_normal[i];
				verticesID++;
				vertices_glpos[verticesID] = o?TR(gl_in[i].gl_Position):BL(gl_in[i].gl_Position);
				vertices_pos	[verticesID] = vertex_position[i];
				vertices_nor	[verticesID] = vertex_normal[i];
				verticesID++;
				break;
			case 0xF:
				vertices_glpos[verticesID] = o?BR(gl_in[i].gl_Position):TL(gl_in[i].gl_Position);
				vertices_pos	[verticesID] = vertex_position[i];
				vertices_nor	[verticesID] = vertex_normal[i];
				verticesID++;
				vertices_glpos[verticesID] = o?TR(gl_in[i].gl_Position):TR(gl_in[i].gl_Position);
				vertices_pos	[verticesID] = vertex_position[i];
				vertices_nor	[verticesID] = vertex_normal[i];
				verticesID++;
				vertices_glpos[verticesID] = o?TL(gl_in[i].gl_Position):BR(gl_in[i].gl_Position);
				vertices_pos	[verticesID] = vertex_position[i];
				vertices_nor	[verticesID] = vertex_normal[i];
				verticesID++;
				break;
			
			default:
				vertices_glpos[verticesID] = gl_in[i].gl_Position;
				vertices_pos	[verticesID] = vertex_position[i];
				vertices_nor	[verticesID] = vertex_normal[i];
				verticesID++;
				break;
		}		
	}
	
	for (int i=0, j=verticesID-1; i<=j; ++i, --j)
	{
		gl_Position 			= vertices_glpos[i];
		geometry_position	=	vertices_pos  [i];
		geometry_normal		= vertices_nor  [i];
		EmitVertex();
		if (i<j)
		{
			gl_Position 			= vertices_glpos[j];
			geometry_position	=	vertices_pos  [j];
			geometry_normal		= vertices_nor  [j];
			EmitVertex();
		}
	}
	
	EndPrimitive();
}

#endif



#ifdef FRAGMENT_SHADER
uniform								sampler2D		light_map;
uniform								mat4				mv;
uniform								mat4				mvp;
in										vec3				geometry_position;
in										vec3				geometry_normal;
out										vec4				fragment_color;

void main() 
{	
	vec4	projected			=	vec4( mvp  * vec4(           geometry_position  , 1.0) );
	vec3	coords				= projected.xyz/projected.w/2.0+0.5;
	float	depth					= texture(light_map, coords.xy).z;

	vec4	oriented			= vec4( mv   * vec4(normalize( geometry_normal   ), 0.0) );
	float value 				= max(0.0, oriented.z);	
	float bias					= 0.005*tan(acos(value));

	if ( coords.z <= depth+bias )	fragment_color = vec4(value, value, value, 1.0);
	else													fragment_color = vec4(0.0,   0.0,   0.0,   0.0);
}

#endif
