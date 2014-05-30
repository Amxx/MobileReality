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

#ifndef CORE_HH
#define CORE_HH

#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>

// =================================
// =   G K I T   I N C L U D E S   =
// =================================
#include "App.h"
#include "Image.h"
#include "ImageIO.h"
#include "ImageManager.h"
#include "Mesh.h"
#include "MeshIO.h"
#include "ProgramManager.h"
#include "Orbiter.h"
#include "OrbiterIO.h"

#include "GL/GLBasicMesh.h"
#include "GL/GLBuffer.h"
#include "GL/GLFramebuffer.h"
#include "GL/GLTexture.h"
#include "GL/GLSampler.h"

// =================================
// =   S E L F   I N C L U D E S   =
// =================================
#include "configuration/configuration.hh"
#include "envmap/envmap.hh"
#include "occlusion/occlusion.hh"
#include "modules/module.hh"
#include "tools/tools.hh"


class Core : public gk::App
{	
	private:
		// Acquisition and environnement processing
		std::vector<Camera*>											_cameras;
		Scanner*																	_scanner;
		Configuration															_config;
		EnvMap																		_envmap;
		Occlusion																	_occlusion;
	
		// OpenGL objects
		gk::GLBasicMesh*													_mesh;
		std::vector<gk::MeshGroup>								_meshGroups;
		gk::BBox																	_meshBox;
	
		std::map<std::string,	gk::GLResource*>		_GLResources;
		template<typename T> T*										getGLResource(const std::string&);
	
		// Status
		int																				_method;
		int																				_rebuild;
		bool																			_buildenvmap;
		gk::Orbiter																_debugviewpoint;
		
	public:
		Core(int = 0, char*[] = nullptr);
		~Core();
	
		int init();
		int quit();
		int draw();
	
		void processKeyboardEvent();
		void processKeyboardEvent(SDL_KeyboardEvent&);
		void processWindowResize(SDL_WindowEvent&);
		void processMouseButtonEvent(SDL_MouseButtonEvent&);
		void processMouseMotionEvent(SDL_MouseMotionEvent&);
};

template<typename T> T* Core::getGLResource(const std::string& str) { return static_cast<T*>(_GLResources[str.c_str()]); }

#endif
