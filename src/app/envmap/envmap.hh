#ifndef ENVMAP_HH
#define ENVMAP_HH

#include "GL/GLFramebuffer.h"
#include "GL/GLProgram.h"
#include "GL/GLTexture.h"

#include "../tools/tools.hh"
#include "../camera/camera.hh"

class EnvMap
{
	private:
		gk::GLProgram*			_program;
		gk::GLTexture*			_envmaptexture;
		gk::GLFramebuffer*	_framebuffer;
	
	public:
		EnvMap();
		~EnvMap();
		
		void init(gk::GLProgram* = nullptr, gk::GLTexture* = nullptr);
		void cuberender(Camera*, const gk::Transform&, gk::GLTexture*, float = 0.f);
		void clear();
		void generateMipMap();
};


#endif