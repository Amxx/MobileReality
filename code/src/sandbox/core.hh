#ifndef CORE_HH
#define CORE_HH

#include <iostream>
#include <sstream>

#include <map>

// =================================
// =   G K I T   I N C L U D E S   =
// =================================
#include "App.h"
#include "Image.h"
#include "ImageIO.h"
#include "ImageManager.h"
#include "ProgramManager.h"

#include "GL/GLBasicMesh.h"
#include "GL/GLFramebuffer.h"
#include "GL/GLTexture.h"
#include "GL/GLProgram.h"
#include "GL/GLSampler.h"


class Core : public gk::App
{	
	private:
		gk::TVec2<float>													_diff;
		std::string																_texturePath;

		std::map<std::string,	gk::GLResource*>		_GLResources;
		template<typename T> T*										getGLResource(const std::string&);	
	
	public:
    Core(int = 0, char*[] = nullptr);
    ~Core();
	
    int init();
    int quit();
    int draw();
	
		void processKeyboardEvent(SDL_KeyboardEvent&);
		void processWindowResize(SDL_WindowEvent&);

};




template<typename T> T* Core::getGLResource(const std::string& str) { return static_cast<T*>(_GLResources[str.c_str()]); }

#endif
