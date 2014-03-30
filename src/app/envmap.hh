#ifndef ENVMAP_HH
#define ENVMAP_HH

#include "GL/GLFramebuffer.h"
#include "GL/GLProgram.h"
#include "GL/GLTexture.h"

#include "tools.hh"

class EnvMap
{
	private:
		gk::GLFramebuffer*	_framebuffer;
		gk::GLProgram*			_program;
		gk::GLTexture*			_envmaptexture;
	

	public:
		EnvMap();
		~EnvMap();
		
		void init(gk::GLProgram* = nullptr, gk::GLTexture* = nullptr);
		void cuberender(Camera*, cv::Matx44f, gk::GLTexture*);
		void clear();
		void generateMipMap();
};


#endif