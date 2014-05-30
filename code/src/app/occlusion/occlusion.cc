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
