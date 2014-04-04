#include "core.hh"


#define			SIZE					1024
#define			COUNT					1000
#define			DISTANCE			3

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
}


// ############################################################################


Core::~Core()
{
}


// ############################################################################


int Core::init()
{	
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);
	
	
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
	
	framebufferLight		=	(new gk::GLFramebuffer())->create(GL_DRAW_FRAMEBUFFER, SIZE, SIZE, gk::GLFramebuffer::COLOR0_BIT | gk::GLFramebuffer::DEPTH_BIT, gk::TextureRGBA32F);
	framebufferAmbiant	=	(new gk::GLFramebuffer())->create(GL_DRAW_FRAMEBUFFER, SIZE, SIZE, gk::GLFramebuffer::COLOR0_BIT | gk::GLFramebuffer::DEPTH_BIT, gk::TextureRGBA32F);
	
	gk::Mesh *mesh = gk::MeshIO::readOBJ(objectPath);
	if (mesh == nullptr) { printf("[ERROR] #5\n"); exit(1); }
	object = new gk::GLBasicMesh(GL_TRIANGLES, mesh->indices.size());
	object->createBuffer(0,	mesh->positions);
  object->createBuffer(1,	mesh->texcoords);
	object->createBuffer(2,	mesh->normals);
	object->createIndexBuffer(mesh->indices);
	orbiter = gk::Orbiter(mesh->box);
	delete mesh;
	
	compute();
	
	return 1;
}  

// ############################################################################


int Core::quit()
{
	return 1;
}


// ############################################################################


int Core::compute()
{
	// --------------------------------------------------------------------------
	timer begin = NOW();
	// --------------------------------------------------------------------------
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(framebufferAmbiant->texture(gk::GLFramebuffer::COLOR0)->target,	0);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferAmbiant->name);
	glViewport(0, 0, framebufferAmbiant->width, framebufferAmbiant->height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	for (const Light& light : makelights(COUNT, DISTANCE*orbiter.size))
	{
		gk::Transform modelview	= gk::LookAt(light.position, gk::Point(0.0, 0.0, 0.0), gk::Vector(0.0, 1.0, 0.0));
		gk::Transform proj			= gk::Orthographic( -orbiter.size/2,
																								+orbiter.size/2,
																								-orbiter.size/2,
																								+orbiter.size/2, 
																								(DISTANCE-1)*orbiter.size,
																								(DISTANCE+1)*orbiter.size);
		
		// --------------------------------------------------------------------------

		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(framebufferLight->texture(gk::GLFramebuffer::COLOR0)->target, 0);
		
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferLight->name);
		glViewport(0, 0, framebufferLight->width, framebufferLight->height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		glUseProgram(programLight->name);
		programLight->uniform("mvp")					= ( proj * modelview ).matrix();
		object->draw();
	
		// --------------------------------------------------------------------------
		
		glEnable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(framebufferAmbiant->texture(gk::GLFramebuffer::COLOR0)->target,	0);
		glBindTexture(framebufferLight->texture(gk::GLFramebuffer::COLOR0)->target,		framebufferLight->texture(gk::GLFramebuffer::COLOR0)->name);
		glGenerateMipmap(GL_TEXTURE_2D);
		
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferAmbiant->name);
		glViewport(0, 0, framebufferAmbiant->width, framebufferAmbiant->height);
		glClear(GL_DEPTH_BUFFER_BIT);
		
		glUseProgram(programAmbiant->name);
		programAmbiant->uniform("mv")					= (        modelview ).matrix();
		programAmbiant->uniform("mvp")				= ( proj * modelview ).matrix();
		programAmbiant->sampler("light_map")	= 0;
		programAmbiant->uniform("light_nb")		= (int) COUNT;
		object->draw();
		
		// --------------------------------------------------------------------------		
		if (COUNT == 1) gk::ImageIO::writeImage("debug.png", framebufferLight->texture(gk::GLFramebuffer::COLOR0)->image(0));
	}
	
	// --------------------------------------------------------------------------
	timer end = NOW();
	std::cout << "Ambiant texture computed in " << formatTimer(end - begin) << std::endl;
	// --------------------------------------------------------------------------
		
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(framebufferAmbiant->texture(gk::GLFramebuffer::COLOR0)->target, framebufferAmbiant->texture(gk::GLFramebuffer::COLOR0)->name);
	glGenerateMipmap(GL_TEXTURE_2D);
	
}


// ############################################################################


int Core::draw()
{
	int x, y;
	unsigned int button= SDL_GetRelativeMouseState(&x, &y);
	if (button & SDL_BUTTON(1))				orbiter.rotate(x, y);
  else if (button & SDL_BUTTON(2))	orbiter.move(float(x) / float(windowWidth()), float(y) / float(windowHeight()));
	else if (button & SDL_BUTTON(3))	orbiter.move(x);
	
	
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glViewport(0, 0, windowWidth(), windowHeight());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(framebufferAmbiant->texture(gk::GLFramebuffer::COLOR0)->target, framebufferAmbiant->texture(gk::GLFramebuffer::COLOR0)->name);
	
	gk::Transform modelview	= orbiter.view();
	gk::Transform proj			= orbiter.projection(windowWidth(), windowHeight());
	
	glUseProgram(programViewer->name);
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
				compute();
				break;
			}
			case SDLK_F2:
			{
				std::stringstream path;
				path << objectPath << ".ambiant.png";
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(framebufferAmbiant->texture(gk::GLFramebuffer::COLOR0)->target, framebufferAmbiant->texture(gk::GLFramebuffer::COLOR0)->name);
				gk::ImageIO::writeImage(path.str(), framebufferAmbiant->texture(gk::GLFramebuffer::COLOR0)->image(0));
				break;
			}
		}
}


// ############################################################################
