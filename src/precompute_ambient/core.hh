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
#include "Image.h"
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
		gk::GLFramebuffer*			framebufferLight;
		gk::GLFramebuffer*			framebufferAmbient;
		gk::GLFramebuffer*			framebufferBlender;
		gk::GLFramebuffer*			framebufferClamp;
	
		gk::GLProgram*					programLight;
		gk::GLProgram*					programTexture;
		gk::GLProgram*					programBlender;
		gk::GLProgram*					programClamp;
		gk::GLProgram*					programViewer;
			
		gk::Orbiter							orbiter;
	
	
	
	public:
    Core(int = 0, char*[] = nullptr);
    ~Core();
	
    int init();
    int quit();
		
		int compute();
    int draw();
	
		void processKeyboardEvent(SDL_KeyboardEvent&);
};

#endif