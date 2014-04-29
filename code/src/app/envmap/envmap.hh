#ifndef ENVMAP_HH
#define ENVMAP_HH

#include "GL/GLFramebuffer.h"
#include "GL/GLProgram.h"
#include "GL/GLTexture.h"

#include "../tools/tools.hh"
#include "../camera/camera.hh"

struct EnvMap
{		
	void init(gk::GLProgram*, gk::GLFramebuffer*);
	
	void render(gk::GLTexture*, Camera*, const gk::Transform&, float = 0.f);
	void clear();
	void generateMipMap();
	//-------------------------------------------------------------------------
	gk::GLProgram*			_program;
	gk::GLFramebuffer*	_framebuffer;
};


#endif
