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

#version 330

#ifdef VERTEX_SHADER
uniform		mat4				mvp;
uniform		mat4				mv;

layout(location=0) in				vec3				position;
layout(location=1) in				vec3				texcoord;
layout(location=2) in				vec3				normal;

out				vec3				vertex_position;
out				vec3				vertex_texcoord;
out				vec3				vertex_normal;

void main()
{
	gl_Position			= mvp * vec4(position, 1.0);
	vertex_position	=	position;
	vertex_texcoord	=	texcoord;
	vertex_normal		=	normal;
}
#endif




#ifdef FRAGMENT_SHADER
uniform		sampler2D		blended;
uniform		mat4				mvp;
uniform		mat4				mv;

in				vec3				vertex_position;
in				vec3				vertex_texcoord;
in				vec3				vertex_normal;

out				vec4				fragment_color;

void main() 
{	
	fragment_color = texture(blended, vertex_texcoord.st);
}
#endif

