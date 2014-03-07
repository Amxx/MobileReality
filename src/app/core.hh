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
#include "tools.hh"
#include "tools_mat.hh"
#include "envmap.hh"


class Core : public gk::App
{	
	private:
		// Acquisition and environnement processing
		std::vector<Camera*>									_cameras;
		Configuration													_config;
		EnvMap																_envmap;
		Scanner*															_scanner;
		
		// Real scene description
		float																	_scale;
		float																	_subscale;
		float																	_objsize;
		
		// Rendering
		std::map<std::string, gk::GLProgram*>	_GLPrograms;
		std::map<std::string,	gk::GLTexture*>	_GLTextures;
		
		gk::GLBasicMesh*											_mesh;
	
		// gk::GLBuffer*						_GLPositions;
		// gk::GLBuffer 						_GLNormals;
	
		
		// static float          		obj_c[3]          = { 0.482f,  0.627f, 0.357f };
		// static float             light_spe[4]     	= { 1.000f,  1.000f, 1.000f, 1.000f };
		// static float             light_dif[4]      = { 1.000f,  1.000f, 1.000f, 1.000f };
		// static float             light_pos[4]      = { 0.000f, 30.000f, 0.000f, 1.000f };


	
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