#ifndef CORE_HH
#define CORE_HH

#include <iostream>

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
#include "GL/GLQuery.h"
#include "GL/GLTexture.h"
#include "GL/GLSampler.h"
#include "GL/GLVertexArray.h"

// =================================
// =   S E L F   I N C L U D E S   =
// =================================
#include "camera.hh"
#include "configuration.hh"
#include "envmap.hh"
#include "module.hh"
#include "tools.hh"
#include "tools_mat.hh"


class Core : public gk::App
{	
	private:
		// Acquisition and environnement processing
		std::vector<Camera*>									_cameras;
		Configuration													_config;
		EnvMap																_envmap;
		Scanner*															_scanner;
				
		// Object
		gk::GLBasicMesh*											_mesh;
	
		// Rendering
		std::map<std::string, gk::GLProgram*>	_GLPrograms;
		std::map<std::string,	gk::GLTexture*>	_GLTextures;
	
		// Status
		bool																	_new_method;
		bool																	_buildenvmap;
		gk::Orbiter														_debugviewpoint;
		
	public:
    Core(int = 0, char*[] = nullptr);
    ~Core();
	
    int init();
    int quit();
    int draw();
	
		void processKeyboardEvent(SDL_KeyboardEvent&);
		void processWindowResize(SDL_WindowEvent&);
		void processMouseButtonEvent(SDL_MouseButtonEvent&);
		void processMouseMotionEvent(SDL_MouseMotionEvent&);
};

#endif