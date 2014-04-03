#include "core.hh"


#define			SIZE					1024
#define			COUNT					1

// ############################################################################
// #                                  MACROS                                  #
// ############################################################################

#define			LOGHERE					std::cout << "[HERE] " << __FILE__ << " : " << __LINE__ << std::endl;
#define			NOW()						std::chrono::steady_clock::now();

typedef			std::chrono::steady_clock::time_point														timer;
typedef			std::chrono::duration<long int, std::ratio<1l, 1000000000l>>		duration;

std::string formatTimer(duration dt)
{
	char buffer[1024];
	sprintf(buffer, "%3d ms %03d ys %03d ns",
					std::chrono::duration_cast<std::chrono::milliseconds>(dt).count(),
					std::chrono::duration_cast<std::chrono::microseconds>(dt).count()%1000,
					std::chrono::duration_cast<std::chrono::nanoseconds>(dt).count()%1000);
	return std::string(buffer);
}

static gk::Transform bias( {0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.5, 0.5, 0.5, 1.0}	);

// ############################################################################
// #                                 METHODS                                  #
// ############################################################################

Core::Core(int argc, char* argv[]) :
	gk::App()
{
	srand48(time(0));
	
	
	if (argc<2) { printf("[ERROR] #1\n"); exit(0); }
	objectPath = argv[1];
	
	gk::AppSettings settings;
	settings.setGLVersion(3,1);
	if(createWindow(800, 600, settings) < 0) closeWindow();
	
	
	
	gk::programPath("install/shaders");
	
	programLight = gk::createProgram("precompute_light.glsl");
	if (programLight == gk::GLProgram::null()) { printf("[ERROR] #2\n"); exit(1); }
	glBindAttribLocation(programLight->name,	0, "position");
	glBindAttribLocation(programLight->name,	1, "normal");
	glBindAttribLocation(programLight->name,	2, "texcoord");
	
	programAmbiant = gk::createProgram("precompute_ambiant.glsl");
	if (programAmbiant == gk::GLProgram::null()) { printf("[ERROR] #3\n"); exit(1); }
	glBindAttribLocation(programAmbiant->name,	0, "position");
	glBindAttribLocation(programAmbiant->name,	1, "normal");
	glBindAttribLocation(programAmbiant->name,	2, "texcoord");
	
	programViewer = gk::createProgram("precompute_viewer.glsl");
	if (programViewer == gk::GLProgram::null()) { printf("[ERROR] #4\n"); exit(1); }
	glBindAttribLocation(programViewer->name,	0, "position");
	glBindAttribLocation(programViewer->name,	1, "normal");
	glBindAttribLocation(programViewer->name,	2, "texcoord");
	
	
	textureLight			=	(new gk::GLTexture())->createTexture2D(0, SIZE, SIZE);	
	textureAmbiant		= (new gk::GLTexture())->createTexture2D(1, SIZE, SIZE);
	framebuffer				=	(new gk::GLFramebuffer())->create(GL_DRAW_FRAMEBUFFER, SIZE, SIZE, gk::GLFramebuffer::COLOR0_BIT);
	
	glBindTexture(textureLight->target, textureLight->name);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	
	gk::Mesh *mesh = gk::MeshIO::readOBJ(objectPath);
	if (mesh == nullptr) { printf("[ERROR] #5\n"); exit(1); }
	object = new gk::GLBasicMesh(GL_TRIANGLES, mesh->indices.size());
	object->createBuffer(0,	mesh->positions);
  object->createBuffer(1,	mesh->texcoords);
	object->createBuffer(2,	mesh->normals);
	object->createIndexBuffer(mesh->indices);
	orbiter = gk::Orbiter(mesh->box);
	delete mesh;
	
	
}


// ############################################################################


Core::~Core()
{
}


// ############################################################################


int Core::init()
{
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);
	
	glClearColor(0.0, 0.0, 0.0, 1.0);	
	
	// --------------------------------------------------------------------------
	timer begin = NOW();
	// --------------------------------------------------------------------------
	
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->name);
	glViewport(0, 0, framebuffer->width, framebuffer->height);
	
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(textureAmbiant->target, 0);	
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,  GL_TEXTURE_2D, textureAmbiant->name, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	
	for (const Light& light : makelights(COUNT, 50))
	{
		gk::Transform modelview	= gk::LookAt(light.position, gk::Point(0.0, 0.0, 0.0), gk::Vector(0.0, 1.0, 0.0));
		gk::Transform proj			= gk::Perspective(45.f, (float) framebuffer->width/framebuffer->height, 1.f, 1000.f);
		
		
		glDisable(GL_BLEND);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(textureLight->target, 0);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureLight->name, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glUseProgram(programLight->name);
		programLight->uniform("mvp")					= ( proj * modelview ).matrix();
		object->draw();

		
		glEnable(GL_BLEND);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(textureAmbiant->target, 0);	
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureAmbiant->name, 0);
		glBindTexture(textureLight->target, textureLight->name);
		glGenerateMipmap(GL_TEXTURE_2D);

		
		glUseProgram(programAmbiant->name);
		programAmbiant->uniform("mv")					= (        modelview ).matrix();
		programAmbiant->uniform("mvp")				= ( proj * modelview ).matrix();
		programAmbiant->sampler("light_map")	= 0;
		programAmbiant->uniform("idx")				= (int) light.id;
		programAmbiant->uniform("light_nb")		= (int) COUNT;
		object->draw();
		
		gk::ImageIO::writeImage("debug.png", textureLight->image(0));
		
	}	
	


	
	// --------------------------------------------------------------------------
	timer end = NOW();
	std::cout << "Ambiant texture computed in " << formatTimer(end - begin) << std::endl;
	// --------------------------------------------------------------------------
		
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(textureAmbiant->target, textureAmbiant->name);
	glGenerateMipmap(GL_TEXTURE_2D);
	
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
	
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glViewport(0, 0, windowWidth(), windowHeight());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(textureAmbiant->target, textureAmbiant->name);
	
	gk::Transform modelview	= orbiter.view();
	gk::Transform proj			= gk::Perspective(45.f, (float) windowWidth()/windowHeight(), 0.1f, 1000000.f);
	
	glUseProgram(programViewer->name);
//programViewer->uniform("mv")	= (        modelview ).matrix();
	programViewer->uniform("mvp")	= ( proj * modelview ).matrix();
	programViewer->sampler("ambiant")	= 0;
	object->draw();
	
	present();
	
	return 1;
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
			case SDLK_F1:
			{
				init();
				break;
			}
			case SDLK_F2:
			{
				std::stringstream path;
				path << objectPath << ".ambiant.png";
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(textureAmbiant->target, textureAmbiant->name);
				gk::ImageIO::writeImage(path.str(), textureAmbiant->image(0));
				break;
			}
		}
}





// ############################################################################





void Core::processWindowResize(SDL_WindowEvent &event)
{
}





// ############################################################################





void Core::processMouseButtonEvent(SDL_MouseButtonEvent &event)
{	
}





// ############################################################################





void Core::processMouseMotionEvent(SDL_MouseMotionEvent &event)
{
}	
