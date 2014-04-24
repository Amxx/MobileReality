#ifndef OCCLUSION_HH
#define OCCLUSION_HH

#include "GL/GLFramebuffer.h"
#include "GL/GLProgram.h"
#include "GL/GLTexture.h"

#include <fstream>

#include "Mesh.h"

#include "../tools/tools.hh"

struct Occlusion
{	
	void init(gk::GLProgram*, gk::GLFramebuffer*, const gk::BBox&);
	void loadSpheres(const std::string& = "");
	
	void render(gk::GLTexture*, int);
	void clear();
	void generateMipMap();
	
	gk::Vec3 bbox() const;
	
	//-------------------------------------------------------------------------
	gk::GLProgram*					_program;
	gk::GLFramebuffer*			_framebuffer;
	
	std::vector<gk::Vec4>		_instances;
	gk::Point								_box_center;
	float										_box_radius;
	float										_box_bottom;
};


#endif
