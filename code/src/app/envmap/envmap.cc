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

#include "envmap.hh"


static const cv::Matx33f toGL(1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, -1.0);

void EnvMap::init(gk::GLProgram* p, gk::GLFramebuffer* f)
{
	_program				= p;
	_framebuffer		=	f;
	generateMipMap();
}

void EnvMap::render(gk::GLTexture* texture, Camera* camera, const gk::Transform& view, float scene_radius)
{	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(_framebuffer->texture(gk::GLFramebuffer::COLOR0)->target, 0);
	glBindTexture(texture->target, texture->name);
	
	glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer->name);
	glViewport(0, 0, _framebuffer->width, _framebuffer->height);
	
	gk::Matrix4x4		reproject = gk::Matrix4x4::mul( cv2gkit(camera->A() * toGL * Matx44to33(camera->orientation())).matrix(), view.normalMatrix() );
	gk::Point				cam_pos		=	view.inverse()(gk::Point(0,0,0));
	
	glUseProgram(_program->name);
	_program->sampler("frame")			= 0;
	_program->uniform("reproject")	= reproject;
	_program->uniform("cam_pos")		= cam_pos;
	_program->uniform("cam_rad")		= (float) camera->radius();
	_program->uniform("sce_rad")		= (float) scene_radius;
	
	for (int i=0; i<6; ++i)
	{
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, _framebuffer->texture(gk::GLFramebuffer::COLOR0)->name, 0);
		_program->uniform("faceID") = i;
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
}

void EnvMap::clear()
{
	glClearColor(0.5, 0.5, 0.5, 1.0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(_framebuffer->texture(gk::GLFramebuffer::COLOR0)->target, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer->name);
	glViewport(0, 0, _framebuffer->width, _framebuffer->height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	generateMipMap();
}


void EnvMap::generateMipMap()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(_framebuffer->texture(gk::GLFramebuffer::COLOR0)->target, _framebuffer->texture(gk::GLFramebuffer::COLOR0)->name);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}
