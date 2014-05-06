#include "core.hh"


#define			SIZE					1024
#define			COUNT					1000

#define			LOGHERE				std::cout << "[HERE] " << __FILE__ << " : " << __LINE__ << std::endl;



// ############################################################################
// #                                 METHODS                                  #
// ############################################################################

Core::Core(int argc, char* argv[]) :
	gk::App()
{
	srand48(time(0));
	
	if (argc<2) { printf("Usage : %s <obj>\n", argv[0]); exit(0); }
	objectPath = argv[1];
	
	gk::AppSettings settings;
	settings.setGLVersion(4,1);
	if(createWindow(1000, 1000, settings) < 0) closeWindow();
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
	glClearColor(0.0, 0.0, 0.0, 0.0);
	
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	gk::programPath("install/shaders");

	_GLResources["prg:light"	]	= gk::createProgram("precompute_ambient_light.glsl"		);
	_GLResources["prg:texture"]	= gk::createProgram("precompute_ambient_texture.glsl"	);
	_GLResources["prg:blender"]	= gk::createProgram("precompute_ambient_blender.glsl"	);
	_GLResources["prg:clamp"	]	= gk::createProgram("precompute_ambient_clamp.glsl"		);
	_GLResources["prg:viewer"	]	= gk::createProgram("precompute_ambient_viewer.glsl"	);
	
	if (getGLResource<gk::GLProgram>("prg:light"	)	== gk::GLProgram::null()) { printf("[ERROR] #1\n"); exit(1); }
	if (getGLResource<gk::GLProgram>("prg:texture")	== gk::GLProgram::null()) { printf("[ERROR] #2\n"); exit(1); }
	if (getGLResource<gk::GLProgram>("prg:blender")	== gk::GLProgram::null()) { printf("[ERROR] #3\n"); exit(1); }
	if (getGLResource<gk::GLProgram>("prg:clamp"	)	== gk::GLProgram::null()) { printf("[ERROR] #4\n"); exit(1); }
	if (getGLResource<gk::GLProgram>("prg:viewer"	)	== gk::GLProgram::null()) { printf("[ERROR] #5\n"); exit(1); }
	
	_GLResources["fbf:light"	]	= (new gk::GLFramebuffer())->create(GL_DRAW_FRAMEBUFFER, SIZE, SIZE, gk::GLFramebuffer::COLOR0_BIT | gk::GLFramebuffer::DEPTH_BIT, gk::TextureRGBA32F);
	_GLResources["fbf:texture"]	= (new gk::GLFramebuffer())->create(GL_DRAW_FRAMEBUFFER, SIZE, SIZE, gk::GLFramebuffer::COLOR0_BIT, gk::TextureRGBA32F);
	_GLResources["fbf:blender"]	= (new gk::GLFramebuffer())->create(GL_DRAW_FRAMEBUFFER, SIZE, SIZE, gk::GLFramebuffer::COLOR0_BIT, gk::TextureRGBA32F);
	_GLResources["fbf:clamp"	]	= (new gk::GLFramebuffer())->create(GL_DRAW_FRAMEBUFFER, SIZE, SIZE, gk::GLFramebuffer::COLOR0_BIT, gk::TextureRGBA32F);
	
	_GLResources["tex:light"	]	= getGLResource<gk::GLFramebuffer>("fbf:light"	)->texture(gk::GLFramebuffer::COLOR0);
	_GLResources["tex:texture"]	= getGLResource<gk::GLFramebuffer>("fbf:texture")->texture(gk::GLFramebuffer::COLOR0);
	_GLResources["tex:blender"]	= getGLResource<gk::GLFramebuffer>("fbf:blender")->texture(gk::GLFramebuffer::COLOR0);
	_GLResources["tex:clamp"	]	= getGLResource<gk::GLFramebuffer>("fbf:clamp"	)->texture(gk::GLFramebuffer::COLOR0);
	
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
	timer begin = now();
	// --------------------------------------------------------------------------
	
	glActiveTexture		(GL_TEXTURE0);
	glBindTexture			(getGLResource<gk::GLTexture>("tex:blender")->target,	0);
	
	glBindFramebuffer	(GL_FRAMEBUFFER, _GLResources["fbf:blender"]->name);
	glViewport				(0, 0, getGLResource<gk::GLFramebuffer>("fbf:blender")->width,	getGLResource<gk::GLFramebuffer>("fbf:blender")->height);
	glClear						(GL_COLOR_BUFFER_BIT);
	
	
	for (int i=0; i<COUNT; ++i)
	{
		// --------------------------------------------------------------------------
		
		Light light = Light::Random() * orbiter.size;
		gk::Transform modelview	= gk::LookAt(light, gk::Point(0.0, 0.0, 0.0), gk::Vector(0.0, 1.0, 0.0));
		gk::Transform proj			= gk::Orthographic( -orbiter.size/2, +orbiter.size/2,
																								-orbiter.size/2, +orbiter.size/2, 
																								0.f,             +orbiter.size*2	);
		
		// --------------------------------------------------------------------------
		
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
	
		glActiveTexture		(GL_TEXTURE0);
		glBindTexture			(getGLResource<gk::GLTexture>("tex:light")->target, 0);
	
		glBindFramebuffer	(GL_FRAMEBUFFER, _GLResources["fbf:light"]->name);
		glViewport				(0, 0, getGLResource<gk::GLFramebuffer>("fbf:light")->width, getGLResource<gk::GLFramebuffer>("fbf:light")->height);
		glClear						(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		glUseProgram(_GLResources["prg:light"]->name);
		getGLResource<gk::GLProgram>("prg:light")->uniform("mvp")					= ( proj * modelview ).matrix();
		object->draw();

		// glActiveTexture					(GL_TEXTURE0);
		// glBindTexture						(getGLResource<gk::GLTexture>("tex:light")->target, _GLResources["tex:light"]->name	);
		// gk::ImageIO::writeImage("tex:light.png", getGLResource<gk::GLTexture>("tex:light")->image(0));

		// --------------------------------------------------------------------------
		
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		
		glActiveTexture		(GL_TEXTURE0);
		glBindTexture			(getGLResource<gk::GLTexture>("tex:texture")->target, 0																);
		glBindTexture			(getGLResource<gk::GLTexture>("tex:light"  )->target, _GLResources["tex:light"]->name	);
		glGenerateMipmap	(GL_TEXTURE_2D);
	
		glBindFramebuffer	(GL_FRAMEBUFFER, _GLResources["fbf:texture"]->name);
		glViewport				(0, 0, getGLResource<gk::GLFramebuffer>("fbf:texture")->width, getGLResource<gk::GLFramebuffer>("fbf:texture")->height);
		glClear						(GL_COLOR_BUFFER_BIT);

		glUseProgram(_GLResources["prg:texture"]->name);
		getGLResource<gk::GLProgram>("prg:texture")->uniform("height"		) = (float) getGLResource<gk::GLFramebuffer>("fbf:texture")->height;
		getGLResource<gk::GLProgram>("prg:texture")->uniform("width"		) = (float) getGLResource<gk::GLFramebuffer>("fbf:texture")->width;
		getGLResource<gk::GLProgram>("prg:texture")->uniform("mv"				) = (        modelview ).matrix();
		getGLResource<gk::GLProgram>("prg:texture")->uniform("mvp"			) = ( proj * modelview ).matrix();
		getGLResource<gk::GLProgram>("prg:texture")->sampler("light_map")	= 0;
		object->draw();

		// glActiveTexture					(GL_TEXTURE0);
		// glBindTexture						(getGLResource<gk::GLTexture>("tex:texture")->target, _GLResources["tex:texture"]->name	);
		// gk::ImageIO::writeImage("tex:texture.png", getGLResource<gk::GLTexture>("tex:texture")->image(0));

		// --------------------------------------------------------------------------
		
		glEnable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		
		glActiveTexture		(GL_TEXTURE0);
		glBindTexture			(getGLResource<gk::GLTexture>("tex:blender")->target, 0																	);
		glBindTexture			(getGLResource<gk::GLTexture>("tex:texture")->target, _GLResources["tex:texture"]->name	);
		glGenerateMipmap	(GL_TEXTURE_2D);

		glBindFramebuffer	(GL_FRAMEBUFFER, _GLResources["fbf:blender"]->name);
		glViewport				(0, 0, getGLResource<gk::GLFramebuffer>("fbf:blender")->width,	getGLResource<gk::GLFramebuffer>("fbf:blender")->height);
		
		glUseProgram(_GLResources["prg:blender"]->name);
		getGLResource<gk::GLProgram>("prg:blender")->sampler("ambient"	) = 0;
		getGLResource<gk::GLProgram>("prg:blender")->uniform("light_nb"	) = (int) COUNT;
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		// --------------------------------------------------------------------------
	}


	
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	
	glActiveTexture		(GL_TEXTURE0);
	glBindTexture			(getGLResource<gk::GLTexture>("tex:clamp"  )->target, 0																	);
	glBindTexture			(getGLResource<gk::GLTexture>("tex:blender")->target, _GLResources["tex:blender"]->name	);
	glGenerateMipmap	(GL_TEXTURE_2D);

	glBindFramebuffer	(GL_FRAMEBUFFER, _GLResources["fbf:clamp"]->name);
	glViewport				(0, 0, getGLResource<gk::GLFramebuffer>("fbf:clamp")->width,	getGLResource<gk::GLFramebuffer>("fbf:clamp")->height);
		
	glUseProgram(_GLResources["prg:clamp"]->name);
	getGLResource<gk::GLProgram>("prg:clamp")->sampler("data")				= 0;
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// --------------------------------------------------------------------------
	timer end = now();
	std::cout << "Ambient texture computed in " << formatTimer(end - begin) << std::endl;
	// --------------------------------------------------------------------------	
	glActiveTexture					(GL_TEXTURE0);
	glBindTexture						(getGLResource<gk::GLTexture>("tex:clamp")->target, _GLResources["tex:clamp"]->name	);
	glGenerateMipmap				(GL_TEXTURE_2D);
	
	gk::Image* finalAmbient = getGLResource<gk::GLTexture>("tex:clamp")->image(0);
	
	float r=0, g=0, b=0, n=0;
	for (int i=0; i<finalAmbient->width; ++i)
		for (int j=0; j<finalAmbient->height; ++j)
		{
			gk::VecColor c = finalAmbient->pixel(i,j);
			if (c.a != 0.f) { r += c.r; g += c.g; b += c.b; n++; }
		}

	gk::VecColor m = gk::VecColor(r/n, g/n, b/n, 1.f);
	for (int i=0; i<finalAmbient->width; ++i)
		for (int j=0; j<finalAmbient->height; ++j)
		{
			gk::VecColor c = finalAmbient->pixel(i,j);
			if (c.a == 0.f) { finalAmbient->setPixel(i,j, m); }
		}
		
	// std::stringstream path;
	// path << objectPath << ".ambient.png";
	// gk::ImageIO::writeImage(path.str(), finalAmbient);
	
	gk::ImageIO::writeImage	("debug.png", finalAmbient);
	
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
	
	glClearColor(0.0, 0.0, 0.0, 1.0);
	
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	
	glBindFramebuffer	(GL_DRAW_FRAMEBUFFER, 0);
	glViewport				(0, 0, windowWidth(), windowHeight());
	glClear						(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
	glActiveTexture	(GL_TEXTURE0);
	glBindTexture		(getGLResource<gk::GLTexture>("tex:clamp")->target, _GLResources["tex:clamp"]->name);
	
	gk::Transform modelview	= orbiter.view();
	gk::Transform proj			= orbiter.projection(windowWidth(), windowHeight());
	
	glUseProgram(_GLResources["prg:viewer"]->name);
	getGLResource<gk::GLProgram>("prg:viewer")->uniform("mvp"			) = ( proj * modelview ).matrix();
	getGLResource<gk::GLProgram>("prg:viewer")->sampler("blended"	) = 0;
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
