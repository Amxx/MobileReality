#include "occlusion.hh"

//==================================================================================================
#define SHADOWSIZE	3
gk::Vec3 Occlusion::bbox() const
{
	return gk::Vec3(_box_center.x + SHADOWSIZE * _box_radius,
									_box_bottom,
									_box_center.z + SHADOWSIZE * _box_radius);
}
//==================================================================================================

void Occlusion::init(gk::GLProgram* p, gk::GLFramebuffer* f, const gk::BBox& bbox)
{
	_program						= p;
	_framebuffer				=	f;
	generateMipMap();
	
	bbox.BoundingSphere(_box_center, _box_radius);
	_box_bottom = bbox.pMin.y;
}

void Occlusion::loadSpheres(const std::string& str)
{
	_instances.clear();
	if (!str.empty())
	{
		std::ifstream ifs(str, std::ifstream::in);
		if (ifs.is_open())
		{
			float x, y, z, r;
			while (ifs.good()) { ifs >> x >> y >> z >> r; _instances.push_back(gk::Vec4(x, y, z, r)); }
			ifs.close();
		}
	}
	if (_instances.empty())
	{
		if (!str.empty()) fprintf(stderr, "[WARNING] Could not open / load spheres from file %s\n", str.c_str());
		_instances.push_back(gk::Vec4(_box_center.x, _box_center.y, _box_center.z, _box_radius/sqrtf(3.f)));
	}
}

void Occlusion::render(gk::GLTexture* texture, int renderoptions)
{
	clear();
	glBindTexture(texture->target, texture->name);
	
	glEnable(GL_BLEND);
	glUseProgram(_program->name);
	_program->uniform("method")			= renderoptions;
	_program->sampler("envmap")			= 0;
	_program->uniform("bbox")				= bbox();
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _framebuffer->texture(gk::GLFramebuffer::COLOR0)->name, 0);
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, _instances.size());
	glDisable(GL_BLEND);
}

void Occlusion::clear()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(_framebuffer->texture(gk::GLFramebuffer::COLOR0)->target, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer->name);
	glViewport(0, 0, _framebuffer->width, _framebuffer->height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Occlusion::generateMipMap()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(_framebuffer->texture(gk::GLFramebuffer::COLOR0)->target, _framebuffer->texture(gk::GLFramebuffer::COLOR0)->name);
	glGenerateMipmap(GL_TEXTURE_2D);
}
