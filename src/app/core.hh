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
