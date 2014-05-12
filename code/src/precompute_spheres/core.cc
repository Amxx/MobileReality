#include "core.hh"

size_t				samples_nb	= 100000;
size_t				clusters_nb	= 100;
int						max_step		= 1000;
std::string 	path;
	
// ############################################################################

Core::Core(int argc, char* argv[]) :
	gk::App(),
	lms(nullptr)
{
	srand48(time(nullptr));

	
	for (int i=1; i<argc; ++i) 
		if			(i+1 < argc && !strcmp(argv[i], "-i"))		path				= argv[++i];
		else if (i+1 < argc && !strcmp(argv[i], "-s"))		samples_nb	= atoi(argv[++i]);
		else if (i+1 < argc && !strcmp(argv[i], "-c"))		clusters_nb	= atoi(argv[++i]);
		else if (i+1 < argc && !strcmp(argv[i], "-m"))		max_step		= atoi(argv[++i]);
	
	if (argc<2 || path.empty())
	{
		printf("Usage: %s [options] -i file.obj\n", argv[0]);
		exit(1);
	}
	
	
	gk::AppSettings settings;
	settings.setGLVersion(4,1);
	if(createWindow(800, 600, settings) < 0) closeWindow();
}

// ############################################################################

Core::~Core()
{
	if (lms) delete lms;
}

// ############################################################################

int Core::init()
{
	renderoptions = 0x02 | 0x01;
	
	glClearColor(1.0, 1.0, 1.0, 1.0);	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	present();
	
	
	gk::programPath("install/shaders");

	programCluster	= gk::createProgram("precompute_sphere_cluster.glsl");
	programViewer		= gk::createProgram("precompute_sphere_viewer.glsl");

	if (programCluster	== gk::GLProgram::null()) { printf("[ERROR] #1\n"); exit(1); }
	if (programViewer		== gk::GLProgram::null()) { printf("[ERROR] #2\n"); exit(1); }
	
	gk::Mesh* mesh = gk::MeshIO::readOBJ(path);
	object = new gk::GLBasicMesh(GL_TRIANGLES, mesh->indices.size());
	object->createBuffer(0,	mesh->positions);
  object->createBuffer(1,	mesh->texcoords);
	object->createBuffer(2,	mesh->normals);
	object->createIndexBuffer(mesh->indices);
	orbiter = gk::Orbiter(mesh->box);
	
	
	// lms = new SphereFit(samples_nb, clusters_nb);
	lms = new PointFit(samples_nb, clusters_nb);
	
	
	printf("mesh sampling ... "); fflush(stdout);
	sampleMesh(*lms, *mesh);
	printf("done\n");
	lms->run(max_step);
	
	delete mesh;

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
	int x, y;
	unsigned int button= SDL_GetRelativeMouseState(&x, &y);
	if (button & SDL_BUTTON(1))				orbiter.rotate(x, y);
  else if (button & SDL_BUTTON(2))	orbiter.move(float(x) / float(windowWidth()), float(y) / float(windowHeight()));
	else if (button & SDL_BUTTON(3))	orbiter.move(x);
		
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	gk::Transform modelview	= orbiter.view();
	gk::Transform proj			= orbiter.projection(windowWidth(), windowHeight());
	
	if (renderoptions & 0x01)
	{
		glUseProgram(programViewer->name);
		programViewer->uniform("mvp") = ( proj * modelview ).matrix();
		object->draw();
	}
	
	if (renderoptions & 0x02)
		for (const SphereFit::clustertype& cluster : lms->clusters)
		{
			glUseProgram(programCluster->name);
			programCluster->uniform("mvp")								= ( proj * modelview ).matrix();
			programCluster->uniform("tesselation_level")	= gk::Vec2(5.f, 5.f);
			programCluster->uniform("sphere") 						= gk::Vec4(cluster.x, cluster.y, cluster.z, cluster.parameter);
			glDrawArrays(GL_PATCHES, 0, 60);
		}

	present();
	return 1;
}

// ############################################################################

void Core::processKeyboardEvent(SDL_KeyboardEvent& event)
{	
	if (event.state == SDL_PRESSED)
	switch (event.keysym.sym)
	{
		case SDLK_ESCAPE: closeWindow();					break;
		case SDLK_F1:			renderoptions ^= 0x01;	break;
		case SDLK_F2:			renderoptions ^= 0x02;	break;
		case SDLK_F5:			gk::reloadPrograms();		break;
		case SDLK_F10:		lms->finalize();				break;

		case SDLK_SPACE:
		{
			printf("computing step ... "); fflush(stdout);
			lms->step();
			printf("done\n");
			break;
		}

		case SDLK_RETURN:
		{
			std::stringstream spherepath;
			spherepath << path << ".spheres";
			
			printf("Exporting sphere informations to %s ... ", spherepath.str().c_str()); fflush(stdout);
			std::ofstream file(spherepath.str());
			for (const SphereFit::clustertype& cluster : lms->clusters)
				file << cluster.x << " " << cluster.y << " " << cluster.z << " " << cluster.parameter << std::endl;
			file.close();
			printf("done\n");
			break;
		}
	}
}

// ############################################################################

void Core::processWindowResize(SDL_WindowEvent&)
{
	glViewport(0, 0, windowWidth(), windowHeight());
}