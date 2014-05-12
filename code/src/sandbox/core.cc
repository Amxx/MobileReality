#include "core.hh"

#define			LOGHERE				std::cout << "[HERE] " << __FILE__ << " : " << __LINE__ << std::endl;



// ############################################################################
// #                                 METHODS                                  #
// ############################################################################

Core::Core(int argc, char* argv[]) :
	gk::App()
{
	if (argc<2) { printf("Usage : %s <obj>\n", argv[0]); exit(0); }
	_texturePath = argv[1];

	gk::AppSettings settings;
	settings.setGLVersion(4,1);
	if(createWindow(800, 800, settings) < 0) closeWindow();

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	present();
}


// ############################################################################


Core::~Core()
{
	for (auto r : _GLResources)
	{
		printf("releasing '%s' ... ", r.first.c_str()); fflush(stdout);
		r.second->release();
		printf("done\n");
	}
}


// ############################################################################


int Core::init()
{	
	_diff = gk::TVec2<float>(1.0, 1.0);

	gk::programPath("install/shaders");

	_GLResources["prg"]	= gk::createProgram("sandbox.glsl");
	if (getGLResource<gk::GLProgram>("prg")	== gk::GLProgram::null()) { printf("[ERROR] #1\n"); exit(1); }

	_GLResources["vao"] = (new gk::GLVertexArray())->create();
	_GLResources["tex"]	= (new gk::GLTexture())->createTexture2D(gk::GLTexture::UNIT0, gk::readImage(_texturePath));
	// _GLResources["spl"] = gk::createLinearSampler();
	// glBindSampler(0, _GLResources["spl"]->name);

	#if 0
	GLint num = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &num);
	std::cout << num << std::endl;
	while(0<--num) {
		std::cout << glGetStringi(GL_EXTENSIONS, num-1) << std::endl;
	}
	#endif

	float maxA;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxA);
	printf("maxA : %ld\n", maxA);

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
	

	printf("\r[diff] x = %f, y = %f", _diff.x, _diff.y); fflush(stdout);
	
	glActiveTexture	(GL_TEXTURE0);
	glBindTexture		(getGLResource<gk::GLTexture>("tex")->target, _GLResources["tex"]->name);
	
	glBindVertexArray(_GLResources["vao"]->name);
	glUseProgram(_GLResources["prg"]->name);
	getGLResource<gk::GLProgram>("prg")->uniform("diff") = _diff;
	getGLResource<gk::GLProgram>("prg")->sampler("data") = 0;
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	
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
			case SDLK_F5:
			{
				gk::reloadPrograms();
				break;
			}
			case SDLK_UP:			_diff.x = std::min(_diff.x*2.0, 512.0); break;
			case SDLK_DOWN:		_diff.x = std::max(_diff.x/2.0, 	1.0); break;
			case SDLK_RIGHT:	_diff.y = std::min(_diff.y*2.0, 512.0); break;
			case SDLK_LEFT:		_diff.y = std::max(_diff.y/2.0, 	1.0); break;
		}
}


// ############################################################################

void Core::processWindowResize(SDL_WindowEvent& event)
{
	glViewport(0, 0, windowWidth(), windowHeight());
}