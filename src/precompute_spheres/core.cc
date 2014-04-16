#include "core.hh"

size_t			samples_nb	= 100000;
size_t			clusters_nb	= 10;
int					max_step		= 100;
std::string path;
	
// ############################################################################

Core::Core(int argc, char* argv[]) :
	gk::App()
{
	/*
	srand48(time(nullptr));
	if (argc<2) { printf("Usage: %s <file.obj>\n", argv[0]); exit(1); }
	path = argv[1];
	*/
	gk::AppSettings settings;
	settings.setGLVersion(3,3);
	if(createWindow(800, 600, settings) < 0) closeWindow();
}

// ############################################################################

Core::~Core()
{
}

// ############################################################################

int Core::init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);	
/*
	gk::Mesh* mesh = gk::MeshIO::readOBJ(path);
	orbiter = gk::Orbiter(mesh->box);
	lms.init(mesh);
	lms.run(max_step);
	delete mesh;
*/
	return 1;
}

// ############################################################################

int Core::quit()
{
	return 1;
}

// ############################################################################

int Core::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	for (SphereFit::Cluster
	present();
}

// ############################################################################

void Core::processKeyboardEvent(SDL_KeyboardEvent& event)
{	
	if (event.state == SDL_PRESSED)
	switch (event.keysym.sym)
	{
		case SDLK_ESCAPE:
		{
			closeWindow();
			break;
		}
		case SDLK_F5:
		{
			gk::reloadPrograms();
			break;
		}
	}
}