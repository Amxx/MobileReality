#include "envmap.hh"


static const cv::Matx33f toGL(1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, -1.0);


EnvMap::EnvMap() :
	_program(nullptr),
	_envmaptexture(nullptr),
	_framebuffer(nullptr)
{
}

EnvMap::~EnvMap()
{
	if (_framebuffer) { _framebuffer->release(); }
}






void EnvMap::init(gk::GLProgram* p, gk::GLTexture* t)
{
	_program				= p;
	_envmaptexture	= t;
	_framebuffer		=	(new gk::GLFramebuffer())->create(GL_DRAW_FRAMEBUFFER, _envmaptexture->width, _envmaptexture->height, gk::GLFramebuffer::COLOR0_BIT);
	generateMipMap();
}

void EnvMap::cuberender(Camera* camera, const gk::Transform& view, gk::GLTexture* texture, float scene_radius)
{
	glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer->name);
	glViewport(0, 0, _framebuffer->width, _framebuffer->height);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(_envmaptexture->target, 0);
	glBindTexture(texture->target, texture->name);
	
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
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, _envmaptexture->name, 0);
		_program->uniform("faceID") = i;
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
}

void EnvMap::clear()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(_envmaptexture->target, _envmaptexture->name);
	std::vector<unsigned int> zeros(_envmaptexture->width*_envmaptexture->height*4, 0);
	for(unsigned int i = 0; i<6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,_envmaptexture->format.internal, _envmaptexture->width, _envmaptexture->height, 0, _envmaptexture->format.data_format, _envmaptexture->format.data_type, &zeros.front());	
	generateMipMap();
}


void EnvMap::generateMipMap()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(_envmaptexture->target, _envmaptexture->name);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}