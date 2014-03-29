#ifndef ENVMAP_HH
#define ENVMAP_HH


	
#include "GL/GLFramebuffer.h"
#include "GL/GLProgram.h"
#include "GL/GLTexture.h"

#include "tools.hh"




static const cv::Matx33f toGL(1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, -1.0);

class EnvMap
{
	private:
		gk::GLFramebuffer*	_framebuffer;
		gk::GLProgram*			_program;
		gk::GLTexture*			_envmaptexture;
	

	public:
	
		EnvMap() :
			_program(nullptr),
			_framebuffer(nullptr),
			_envmaptexture(nullptr)
		{
		}	
	
		EnvMap(gk::GLProgram* p, gk::GLFramebuffer* fb, gk::GLTexture* t) :
			_program(p),
			_framebuffer(fb),
			_envmaptexture(t)
		{
		}
	
	
		void cuberender(Camera* camera, cv::Matx44f modelview, gk::GLTexture* texture)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer->name);
			glViewport(0, 0, _framebuffer->width, _framebuffer->height);
			cv::Matx33f transform = camera->A() * toGL * Matx44to33(camera->orientation() * modelview);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(_envmaptexture->target, 0);
			glBindTexture(texture->target, texture->name);
			
			glUseProgram(_program->name);
			_program->sampler("frame")			= 0;
			_program->uniform("reproject")	= cv2gkit(transform).matrix();

			for (int i=0; i<6; ++i)
			{
				glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, _envmaptexture->name, 0);
				_program->uniform("faceID") = i;
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			}
		}
		
		void clear()
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(_envmaptexture->target, _envmaptexture->name);
			std::vector<unsigned int> zeros(_envmaptexture->width*_envmaptexture->height*4, 0);
			for(unsigned int i = 0; i<6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,_envmaptexture->format.internal, _envmaptexture->width, _envmaptexture->height, 0, _envmaptexture->format.data_format, _envmaptexture->format.data_type, &zeros.front());
		}
};


#endif