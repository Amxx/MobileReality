#include "core.hh"


#define			SIZE					1024
#define			COUNT					1000
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
	sprintf(buffer, "%3d ms %03d µs %03d ns",
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

	programBlender = gk::createProgram("precompute_blender.glsl");
	if (programBlender == gk::GLProgram::null()) { printf("[ERROR] #4\n"); exit(1); }
	
	programClamp = gk::createProgram("precompute_clamp.glsl");
	if (programClamp == gk::GLProgram::null()) { printf("[ERROR] #5\n"); exit(1); }
	
	programViewer = gk::createProgram("precompute_viewer.glsl");
	if (programViewer == gk::GLProgram::null()) { printf("[ERROR] #6\n"); exit(1); }
	glBindAttribLocation(programViewer->name,	0, "position");
	glBindAttribLocation(programViewer->name,	1, "normal");
	glBindAttribLocation(programViewer->name,	2, "texcoord");
	
	
	framebufferLight		=	(new gk::GLFramebuffer())->create(GL_DRAW_FRAMEBUFFER, SIZE, SIZE, gk::GLFramebuffer::COLOR0_BIT | gk::GLFramebuffer::DEPTH_BIT, gk::TextureRGBA32F);
	framebufferAmbiant	=	(new gk::GLFramebuffer())->create(GL_DRAW_FRAMEBUFFER, SIZE, SIZE, gk::GLFramebuffer::COLOR0_BIT | gk::GLFramebuffer::DEPTH_BIT, gk::TextureRGBA32F);
	framebufferBlender	=	(new gk::GLFramebuffer())->create(GL_DRAW_FRAMEBUFFER, SIZE, SIZE, gk::GLFramebuffer::COLOR0_BIT | gk::GLFramebuffer::DEPTH_BIT, gk::TextureRGBA32F);
	framebufferClamp		=	(new gk::GLFramebuffer())->create(GL_DRAW_FRAMEBUFFER, SIZE, SIZE, gk::GLFramebuffer::COLOR0_BIT | gk::GLFramebuffer::DEPTH_BIT, gk::TextureRGBA32F);
	
	gk::Mesh *mesh = gk::MeshIO::readOBJ(objectPath);
	if (mesh == nullptr) { printf("[ERROR] #7\n"); exit(1); }
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
	glBindTexture(framebufferBlender->texture(gk::GLFramebuffer::COLOR0)->target,	0);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferBlender->name);
	glViewport(0, 0, framebufferBlender->width, framebufferBlender->height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
	for (const Light& light : makelights(COUNT, orbiter.size))
	{
		gk::Transform modelview	= gk::LookAt(light.position, gk::Point(0.0, 0.0, 0.0), gk::Vector(0.0, 1.0, 0.0));
		gk::Transform proj			= gk::Orthographic( -orbiter.size/2, +orbiter.size/2,
																								-orbiter.size/2, +orbiter.size/2, 
																								0.f,             +orbiter.size*2	);
		
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
		
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(framebufferAmbiant->texture(gk::GLFramebuffer::COLOR0)->target,	0);
		glBindTexture(framebufferLight->texture(gk::GLFramebuffer::COLOR0)->target,		framebufferLight->texture(gk::GLFramebuffer::COLOR0)->name);
		glGenerateMipmap(GL_TEXTURE_2D);
		
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferAmbiant->name);
		glViewport(0, 0, framebufferAmbiant->width, framebufferAmbiant->height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glUseProgram(programAmbiant->name);
		programAmbiant->uniform("height")			= (float) framebufferAmbiant->height;
		programAmbiant->uniform("width")			= (float) framebufferAmbiant->width;
		programAmbiant->uniform("mv")					= (        modelview ).matrix();
		programAmbiant->uniform("mvp")				= ( proj * modelview ).matrix();
		programAmbiant->sampler("light_map")	= 0;
		object->draw();
		
		// --------------------------------------------------------------------------		
		
		glEnable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(framebufferBlender->texture(gk::GLFramebuffer::COLOR0)->target,	0);
		glBindTexture(framebufferAmbiant->texture(gk::GLFramebuffer::COLOR0)->target,	framebufferAmbiant->texture(gk::GLFramebuffer::COLOR0)->name);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindFramebuffer(GL_FRAMEBUFFER, framebufferBlender->name);
		glViewport(0, 0, framebufferBlender->width, framebufferBlender->height);
		glClear(GL_DEPTH_BUFFER_BIT);
		
		glUseProgram(programBlender->name);
		programBlender->sampler("ambiant")		= 0;
		programBlender->uniform("light_nb")		= (int) COUNT;
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		
		// --------------------------------------------------------------------------		
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(framebufferClamp->texture(gk::GLFramebuffer::COLOR0)->target,	0);
		glBindTexture(framebufferBlender->texture(gk::GLFramebuffer::COLOR0)->target,	framebufferBlender->texture(gk::GLFramebuffer::COLOR0)->name);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindFramebuffer(GL_FRAMEBUFFER, framebufferClamp->name);
		glViewport(0, 0, framebufferClamp->width, framebufferClamp->height);
		glClear(GL_DEPTH_BUFFER_BIT);
		
		glUseProgram(programClamp->name);
		programClamp->sampler("input")				= 0;
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	
	// --------------------------------------------------------------------------
	timer end = NOW();
	std::cout << "Ambiant texture computed in " << formatTimer(end - begin) << std::endl;
	// --------------------------------------------------------------------------
		
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(framebufferClamp->texture(gk::GLFramebuffer::COLOR0)->target, framebufferClamp->texture(gk::GLFramebuffer::COLOR0)->name);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	gk::Image* finalAmbiant = framebufferClamp->texture(gk::GLFramebuffer::COLOR0)->image(0);
	float r=0, g=0, b=0, n=0;
	for (int i=0; i<finalAmbiant->width; ++i)
		for (int j=0; j<finalAmbiant->height; ++j)
		{
			gk::VecColor c = finalAmbiant->pixel(i,j);
			if (c.r == 0.f && c.g == 0.f && c.b == 0.f) continue;
			r += c.r; g += c.g; b += c.b; n++;
		}

	std::cout << "samples : " << n << std::endl;

	gk::VecColor m = gk::VecColor(r/n, g/n, b/n, 1.f);
	for (int i=0; i<finalAmbiant->width; ++i)
		for (int j=0; j<finalAmbiant->height; ++j)
		{
			gk::VecColor c = finalAmbiant->pixel(i,j);
			if (c.r != 0.f || c.g != 0.f || c.b != 0.f) continue;
			finalAmbiant->setPixel(i,j, m);
		}

	std::stringstream path;
	path << objectPath << ".ambiant.png";
	gk::ImageIO::writeImage(path.str(), finalAmbiant);
				
		
//	if (COUNT == 1) gk::ImageIO::writeImage("debug_blended.png", framebufferBlender->texture(gk::GLFramebuffer::COLOR0)->image(0));
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
	glBindTexture(framebufferClamp->texture(gk::GLFramebuffer::COLOR0)->target, framebufferClamp->texture(gk::GLFramebuffer::COLOR0)->name);
	
	gk::Transform modelview	= orbiter.view();
	gk::Transform proj			= orbiter.projection(windowWidth(), windowHeight());
	
	glUseProgram(programViewer->name);
	programViewer->uniform("mvp")			= ( proj * modelview ).matrix();
	programViewer->sampler("blended")	= 0;
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
			case SDLK_F5:
			{
				gk::reloadPrograms();
				break;
			}
		}
}


// ############################################################################
