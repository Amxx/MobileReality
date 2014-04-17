#ifndef CORE_HH
#define CORE_HH

#include <iostream>
#include <sstream>

// =================================
// =   G K I T   I N C L U D E S   =
// =================================
#include "App.h"
#include "Mesh.h"
#include "MeshIO.h"
#include "Orbiter.h"
#include "ProgramManager.h"

#include "GL/GLBasicMesh.h"
#include "GL/GLProgram.h"


#include "LMS.hh"
#include "SphereFit.hh"


class Core : public gk::App
{	
	private:
		
		SphereFit								lms;
	
		gk::GLBasicMesh*				object;
		gk::GLProgram*					programViewer;
		gk::GLProgram*					programCluster;
		gk::Orbiter							orbiter;
	
		int 										renderoptions;
	
	public:
    Core(int = 0, char*[] = nullptr);
    ~Core();
	
    int init();
    int quit();
    int draw();
	
		void processKeyboardEvent(SDL_KeyboardEvent&);
};

#endif