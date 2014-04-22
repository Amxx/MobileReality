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
