#ifndef CORE_HH
#define CORE_HH

#include <iostream>
#include <sstream>

#include <ctime>
#include <ratio>
#include <chrono>

// =================================
// =   G K I T   I N C L U D E S   =
// =================================
#include "App.h"
#include "ImageIO.h"
#include "Mesh.h"
#include "MeshIO.h"
#include "Orbiter.h"
#include "ProgramManager.h"

#include "GL/GLBasicMesh.h"
#include "GL/GLFramebuffer.h"
#include "GL/GLTexture.h"
#include "GL/GLProgram.h"


#include "light.hh"



class Core : public gk::App
{	
	private:
		std::string							objectPath;
		
		gk::GLBasicMesh*				object;
		gk::GLFramebuffer*			framebuffer;
	
		gk::GLProgram*					programLight;
		gk::GLProgram*					programAmbiant;
		gk::GLProgram*					programViewer;
	
		gk::GLTexture*					textureLight;
		gk::GLTexture*					textureAmbiant;
		
		gk::Orbiter							orbiter;
	
	
	
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