#ifndef CORE_HH
#define CORE_HH

#include <iostream>
#include <sstream>

#include <ctime>
#include <ratio>
#include <chrono>

#include <map>

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
#include "tools/timers.hh"



class Core : public gk::App
{	
	private:
		std::string																objectPath;
		gk::GLBasicMesh*													object;
		std::map<std::string,	gk::GLResource*>		_GLResources;
		gk::Orbiter																orbiter;
	
		template<typename T> T*										getGLResource(const std::string&);	
	
	public:
    Core(int = 0, char*[] = nullptr);
    ~Core();
	
    int init();
    int quit();
		
		int compute();
    int draw();
	
		void processKeyboardEvent(SDL_KeyboardEvent&);
		void processWindowResize(SDL_WindowEvent&);
};




template<typename T> T* Core::getGLResource(const std::string& str) { return static_cast<T*>(_GLResources[str.c_str()]); }

#endif
