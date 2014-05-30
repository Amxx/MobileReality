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
uniform		int					faceID;
out				vec3				position;

void main()
{
	vec3 pos[24] 		= vec3[24](	vec3(+1.0, +1.0, +1.0), 				vec3(+1.0, +1.0, -1.0),					vec3(+1.0, -1.0, +1.0),					vec3(+1.0, -1.0, -1.0),
															vec3(-1.0, +1.0, -1.0), 				vec3(-1.0, +1.0, +1.0),					vec3(-1.0, -1.0, -1.0),					vec3(-1.0, -1.0, +1.0),
															vec3(-1.0, +1.0, -1.0), 				vec3(+1.0, +1.0, -1.0),					vec3(-1.0, +1.0, +1.0),					vec3(+1.0, +1.0, +1.0),
															vec3(-1.0, -1.0, +1.0),					vec3(+1.0, -1.0, +1.0),					vec3(-1.0, -1.0, -1.0),					vec3(+1.0, -1.0, -1.0),
															vec3(-1.0, +1.0, +1.0),					vec3(+1.0, +1.0, +1.0),					vec3(-1.0, -1.0, +1.0),					vec3(+1.0, -1.0, +1.0),
															vec3(+1.0, +1.0, -1.0),					vec3(-1.0, +1.0, -1.0),					vec3(+1.0, -1.0, -1.0),					vec3(-1.0, -1.0, -1.0)				);
	vec4 coords[4]	= vec4[4](	vec4(-1.0, -1.0, +0.0, +1.0),		vec4(+1.0, -1.0, +0.0, +1.0),		vec4(-1.0, +1.0, +0.0, +1.0),		vec4(+1.0, +1.0, +0.0, +1.0)	);

	position		= pos[faceID*4+gl_VertexID];
	gl_Position = coords[gl_VertexID];
}
#endif


#ifdef FRAGMENT_SHADER

uniform		sampler2D		frame;
uniform		mat4				reproject;
uniform		vec3				cam_pos;
uniform		float				cam_rad;
uniform		float				sce_rad;

in				vec3				position;
out				vec4				fragment_color;

void main()
{
	vec4 cubecoords;
	if (sce_rad > 0.0)	cubecoords = reproject * vec4(normalize(normalize(position).xyz*sce_rad-cam_pos.xyz), 1.0 );
	else								cubecoords = reproject * vec4(normalize(          position                         ), 1.0 );	
	
	if (cubecoords.z <= 0.0) discard;
	
	vec2 coords				= vec2(cubecoords.x / cubecoords.z, cubecoords.y / cubecoords.z);
	vec2 centercoords	=	coords - vec2(320, 240);
	
	if (abs(centercoords.x) > 320 || abs(centercoords.y) > 240)								discard;
	if (cam_rad >= 0.0 && dot(centercoords, centercoords) > cam_rad*cam_rad)	discard;
	
	fragment_color = texture(frame, coords/vec2(640,480));
}


#endif