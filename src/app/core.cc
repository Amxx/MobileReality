#include "core.hh"

// ############################################################################
// #                                  MACROS                                  #
// ############################################################################

#define  LOGHERE  std::cout << "[HERE] " << __FILE__ << " : " << __LINE__ << std::endl;


// ############################################################################
// #                                 METHODS                                  #
// ############################################################################

Core::Core(int argc, char* argv[]) :
	gk::App(),
	_cameras(2)
{
	// ===============================================================
	// =          C O N F I G U R A T I O N   L O A D I N G          =
	// ===============================================================
	if (argc > 1) _config.load(argv[1]).check();
	if (_config.general.verbose) _config.display();

	_buildenvmap	= _config.general.envmap.type == Options::DYNAMIC;
	_newmethod		= true;
	// ===============================================================
	// =                   L O A D   S C A N N E R                   =
	// ===============================================================
	_scanner	= Module<Scanner>::load(_config.general.modules.scanner);
	if (_scanner == nullptr) exit(1);
	
	// ===============================================================
	// =                   L O A D   C A M E R A S                   =
	// ===============================================================
	if (_config.devices.front.enable)
	{
		VideoDevice* videodevice = Module<VideoDevice>::load(_config.general.modules.video);
		if (videodevice == nullptr) exit(1);
		_cameras[0] = new Camera(videodevice, cv::Matx44f(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0));
		if (_config.devices.front.id != -2) _cameras[0]->open(_config.devices.front.id);
		_cameras[0]->UIOpen();
	}
	if (_config.devices.back.enable)
	{
		VideoDevice* videodevice = Module<VideoDevice>::load(_config.general.modules.video);
		if (videodevice == nullptr) exit(1);
		_cameras[1] = new Camera(videodevice,  cv::Matx44f(-1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0));
		if (_config.devices.back.id != -2) _cameras[1]->open(_config.devices.back.id);
		_cameras[1]->UIOpen();
	}
	
	// ===============================================================
	// =                  S E T U P   C A M E R A S                  =
	// ===============================================================
	if (_config.devices.front.enable || _config.devices.back.enable)
	{
		if (_cameras[0]) _cameras[0]->setParameter(VideoDevice::MODE, VideoDevice::AUTOEXPOSURE);
		if (_cameras[1]) _cameras[1]->setParameter(VideoDevice::MODE, VideoDevice::AUTOEXPOSURE);
		
		printf("Waiting for camera to set autoexposure ... "); fflush(stdout); sleep(1); printf("done\n");
	
		if (_config.devices.front.enable)
		{
			int brightness	= (_config.general.defaultValues.brightness<0)?_cameras[0]->getParameter(VideoDevice::BRIGHTNESS):_config.general.defaultValues.brightness;
			int gain				= (_config.general.defaultValues.gain      <0)?_cameras[0]->getParameter(VideoDevice::GAIN)      :_config.general.defaultValues.gain;
			_cameras[0]->setParameter(VideoDevice::MODE,				VideoDevice::MANUALEXPOSURE);
			_cameras[0]->setParameter(VideoDevice::BRIGHTNESS,	brightness);
			_cameras[0]->setParameter(VideoDevice::GAIN,				gain);
			if (_config.general.verbose)
			{
				printf("┌─────────────────────────────────────────────────────────────────────┐\n");
				printf("│                           C A M E R A   0                           │\n");
				printf("├─────────────────────────────────────────────────────────────────────┤\n");
				printf("│ brightness : %-54d │\n", brightness);
				printf("│ gain       : %-54d │\n", gain);
				printf("└─────────────────────────────────────────────────────────────────────┘\n");
				// _cameras[0]->showParameters();
			}
			_cameras[0]->UICalibrate(_config.devices.front.params, *_scanner);
		}
		if (_config.devices.back.enable)
		{
			int brightness	= (_config.general.defaultValues.brightness==-1)?_cameras[1]->getParameter(VideoDevice::BRIGHTNESS):_config.general.defaultValues.brightness;
			int gain				= (_config.general.defaultValues.gain      ==-1)?_cameras[1]->getParameter(VideoDevice::GAIN)      :_config.general.defaultValues.gain;
			_cameras[1]->setParameter(VideoDevice::MODE,				VideoDevice::MANUALEXPOSURE);
			_cameras[1]->setParameter(VideoDevice::BRIGHTNESS,	brightness);
			_cameras[1]->setParameter(VideoDevice::GAIN,				gain);
			if (_config.general.verbose)
			{
				printf("┌─────────────────────────────────────────────────────────────────────┐\n");
				printf("│                           C A M E R A   1                           │\n");
				printf("├─────────────────────────────────────────────────────────────────────┤\n");
				printf("│ brightness : %-54d │\n", brightness);
				printf("│ gain       : %-54d │\n", gain);
				printf("└─────────────────────────────────────────────────────────────────────┘\n");
				// _cameras[1]->showParameters();
			}
			_cameras[1]->UICalibrate(_config.devices.back.params, *_scanner);
		}
	}
	
	// ===============================================================
	// =                   R E A D Y   T O   R U N                   =
	// ===============================================================
	gk::AppSettings settings;
	settings.setGLVersion(3,1);
	// settings.setFullscreen();
	if(createWindow(800, 600, settings) < 0) closeWindow();
	
	glClearColor(0.1, 0.1, 0.1, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	present();
		
}





// ############################################################################





Core::~Core()
{
	for (Camera* camera : _cameras) if (camera) camera->close();
	for (auto r : _GLResources)	r.second->release();
}





// ############################################################################





int Core::init()
{
	glClearColor(1.0, 1.0, 1.0, 1.0); // WHITE
	// glClearColor(0.0, 0.0, 0.0, 1.0); // BLACK
	// glClearColor(0.1, 0.1, 0.1, 1.0); //BLACK!10
	
	// ===============================================================
	// =        L O A D   S H A D E R S   A S   P R O G R A M        =
	// ===============================================================
	gk::programPath("install/shaders");
	
	_GLResources["prg:background"]				= gk::createProgram("background.glsl");
	_GLResources["prg:cuberendering"]			= gk::createProgram("cuberendering.glsl");
	_GLResources["prg:materialrendering"]	= gk::createProgram("materialrendering.glsl");
	
	if (_GLResources["prg:background"]				== gk::GLProgram::null())	return -1;
	if (_GLResources["prg:cuberendering"]			== gk::GLProgram::null())	return -1;
	if (_GLResources["prg:materialrendering"]	== gk::GLProgram::null())	return -1;
	
	glBindAttribLocation(_GLResources["prg:materialrendering"]->name,	0, "position");
	glBindAttribLocation(_GLResources["prg:materialrendering"]->name,	1, "normal");
	glBindAttribLocation(_GLResources["prg:materialrendering"]->name,	2, "texcoord");
		
	// ===============================================================
	// =                C R E A T E   T E X T U R E S                =
	// ===============================================================	
	_GLResources["tex:frame0"]				= (new gk::GLTexture())->createTexture2D(0, 640, 480);
	_GLResources["tex:frame1"]				= (new gk::GLTexture())->createTexture2D(1, 640, 480);
	if (!_config.object.visibility.empty())
		_GLResources["tex:visibility"]	= (new gk::GLTexture())->createTexture2D(2, gk::readImage(_config.object.visibility));
		
	// ===============================================================
	// =                  C R E A T E   E N V M A P                  =
	// ===============================================================	
	if (_config.general.envmap.type == Options::DEBUG && !_config.general.envmap.path.empty())
		_GLResources["tex:envmap"]	= (new gk::GLTexture())->createTextureCube(3, gk::readImageArray(_config.general.envmap.path.c_str(), 6));
	else
		_GLResources["tex:envmap"]	= (new gk::GLTexture())->createTextureCube(3, _config.general.envmap.size.width, _config.general.envmap.size.height);

	_envmap.init(	getGLResource<gk::GLProgram>("prg:cuberendering"),
								getGLResource<gk::GLTexture>("tex:envmap")					);
	
	// ===============================================================
	// =                    E A M L E S S   C U B E                  =
	// ===============================================================
	_GLResources["spl:linear"]				= gk::createLinearSampler();
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	
	// ===============================================================
	// =                    C R E A T E   M E S H                    =
	// ===============================================================
	
	gk::Mesh *mesh = gk::MeshIO::readOBJ(_config.object.file);
	if (mesh == nullptr) return -1;
	_mesh = new gk::GLBasicMesh(GL_TRIANGLES, mesh->indices.size());
	_mesh->createBuffer(0,	mesh->positions);
  _mesh->createBuffer(1,	mesh->texcoords);
	_mesh->createBuffer(2,	mesh->normals);
	_mesh->createIndexBuffer(mesh->indices);
	_debugviewpoint = gk::Orbiter(mesh->box);
	delete mesh;
	
	return 1;
}
  

// ############################################################################


int Core::quit()
{
	return 1;
}





// ############################################################################



#define 		NEWCHRONO(X)		std::chrono::steady_clock::time_point X = std::chrono::steady_clock::now();


int Core::draw()
{
	// ===============================================================
	// =                        C O N T E X T                        =
	// ===============================================================
	static	cv::Matx44f	model;																				// Presistent	: Model
	static	cv::Matx44f	view;																					// Persistent	: View (bloc camera)
					bool				position_fresh		= false;
	static	int					position_duration	= 0;												// Persistent : Persistency duration
	
	
	// ===============================================================
	// =               U S E R   I N T E R A C T I O N               =
	// ===============================================================
	NEWCHRONO(t1)
	processKeyboardEvent();
	int x, y;
	unsigned int button= SDL_GetRelativeMouseState(&x, &y);
	if (button & SDL_BUTTON(1))				_debugviewpoint.rotate(x, y);
  else if (button & SDL_BUTTON(2))	_debugviewpoint.move(float(x) / float(windowWidth()), float(y) / float(windowHeight()));
	else if (button & SDL_BUTTON(3))	_debugviewpoint.move(x);
	
	// ===============================================================
	// =                 G R A B I N G   I M A G E S                 =
	// ===============================================================
	NEWCHRONO(t2)
	if (_config.devices.front.enable)
	{
		_cameras[0]->grabFrame();
		glActiveTexture	(GL_TEXTURE0);
		glBindTexture		(getGLResource<gk::GLTexture>("tex:frame0")->target, _GLResources["tex:frame0"]->name);
		glTexSubImage2D	(getGLResource<gk::GLTexture>("tex:frame0")->target, 0, 0, 0, _cameras[0]->frame()->width, _cameras[0]->frame()->height, GL_BGR, GL_UNSIGNED_BYTE, _cameras[0]->frame()->imageData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	if (_config.devices.back.enable)
	{
		_cameras[1]->grabFrame();
		glActiveTexture	(GL_TEXTURE0);
		glBindTexture		(getGLResource<gk::GLTexture>("tex:frame1")->target, _GLResources["tex:frame1"]->name);
		glTexSubImage2D	(getGLResource<gk::GLTexture>("tex:frame1")->target, 0, 0, 0, _cameras[1]->frame()->width, _cameras[1]->frame()->height, GL_BGR, GL_UNSIGNED_BYTE, _cameras[1]->frame()->imageData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	
	// ===============================================================
	// =                    P O S I T I O N I N G                    =
	// ===============================================================	
	NEWCHRONO(t3)
	if (_config.general.localisation.type == Options::DYNAMIC)
		for (Symbol& symbol : _scanner->scan(_cameras[0]->frame()))
			try {
				static cv::Matx44f toGL(1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
				// LEGACY MODE FOR CUBE
				try 				{	model = parseSymbolToModel(	symbol.data, _config.markers.size															) * toGL;	}
				catch (...)	{ model = parseMatx33f_tr		(	symbol.data, _config.markers.size *cv::Matx31f(0.5, 0.5, 0.5)	) * toGL;	}
				symbol.extrinsic(_cameras[0]->A(), _cameras[0]->K(), Scanner::pattern(_config.markers.size, _config.markers.scale));
				view							=	_cameras[0]->orientation().inv() * viewFromSymbol(symbol.rvec, symbol.tvec);
				position_fresh		= !isNull(view) && !isNull(model);
				if (position_fresh) { position_duration = _config.general.defaultValues.persistency; break; }
			} catch (...) {
				printf("Invalid symbol, could not extract model informations from `%s`\n", symbol.data.c_str());
			}
	
	// ===============================================================
	// =                B U I L D I N G   E N V M A P                =
	// ===============================================================
	NEWCHRONO(t4)
	if (_buildenvmap && position_fresh)
	{
		if (_config.devices.back.enable)	_envmap.cuberender(_cameras[1], view * model, getGLResource<gk::GLTexture>("tex:frame1"));
		if (_config.general.envmap.dual)	_envmap.cuberender(_cameras[0], view * model, getGLResource<gk::GLTexture>("tex:frame0"));
		_envmap.generateMipMap();
	}

	// ===============================================================
	// =                      R E N D E R I N G                      =
	// ===============================================================
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glViewport(0, 0, windowWidth(), windowHeight());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// ===============================================================
	// =               B A C K G R O U N D   F R A M E               =
	// ===============================================================
	NEWCHRONO(t5)
	if (_config.general.rendering.background)
	{
		glClear(GL_DEPTH_BUFFER_BIT);	
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(getGLResource<gk::GLTexture>("tex:frame0")->target, _GLResources["tex:frame0"]->name);
		glUseProgram(_GLResources["prg:background"]->name);
		getGLResource<gk::GLProgram>("prg:background")->sampler("frame") = 0;
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	
	// ===============================================================
	// =                S C E N E   R E N D E R I N G                =
	// ===============================================================
	NEWCHRONO(t6)
	if (_config.general.rendering.scene && (_config.general.localisation.type == Options::DEBUG || (position_duration && position_duration--)))
	{
		glClear(GL_DEPTH_BUFFER_BIT);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(getGLResource<gk::GLTexture>("tex:envmap")->target, _GLResources["tex:envmap"]->name);
		glBindSampler(0, _GLResources["spl:linear"]->name);
				
		if (!_config.object.visibility.empty())
		{
			glActiveTexture(GL_TEXTURE0+1);
			glBindTexture(getGLResource<gk::GLTexture>("tex:visibility")->target, _GLResources["tex:visibility"]->name);
		}
		
		gk::Transform		scale	= gk::Scale(_config.object.scale);
		gk::Transform		proj, mv;
		switch(_config.general.localisation.type)
		{
			case Options::DYNAMIC:
				mv		=	cv2gkit(_cameras[0]->orientation() * view * model) * scale;																																	// Point de vue de _cameras[0]
				proj	= cv2gkit(projectionFromIntrinsic(_cameras[0]->A(), _cameras[0]->frame()->width, _cameras[0]->frame()->height, 1.f, 10000.f));
				break;
			case Options::DEBUG:
				mv		= _debugviewpoint.view();
				proj	= gk::Perspective(45.f, (float) windowWidth()/windowHeight(), 0.1f, 1000000.f);
				break;
		}
		gk::Transform mvp = proj * mv;
		
		glUseProgram(_GLResources["prg:materialrendering"]->name);
		getGLResource<gk::GLProgram>("prg:materialrendering")->uniform("new_method")		= _newmethod;
		getGLResource<gk::GLProgram>("prg:materialrendering")->uniform("mvMatrix")			= mv.matrix();
		getGLResource<gk::GLProgram>("prg:materialrendering")->uniform("mvMatrixInv")		= mv.inverseMatrix();		
		getGLResource<gk::GLProgram>("prg:materialrendering")->uniform("mvpMatrix")			= mvp.matrix();
		getGLResource<gk::GLProgram>("prg:materialrendering")->uniform("kd")						= _config.object.material.kd;
		getGLResource<gk::GLProgram>("prg:materialrendering")->uniform("ks")						= _config.object.material.ks;
		getGLResource<gk::GLProgram>("prg:materialrendering")->uniform("ns")						= _config.object.material.ns;
		getGLResource<gk::GLProgram>("prg:materialrendering")->uniform("usevisibility")	= !_config.object.visibility.empty();
		getGLResource<gk::GLProgram>("prg:materialrendering")->sampler("envmap")				= 0;
		getGLResource<gk::GLProgram>("prg:materialrendering")->sampler("visibility")		= 1;
		_mesh->draw();
	}
	
	// ===============================================================
	// =                 F R A M E S   D I S P L A Y                 =
	// ===============================================================
	NEWCHRONO(t7)
	if (_config.general.rendering.view)
	{
		if (_config.devices.front.enable)	cv::imshow("camera 0", cv::Mat(_cameras[0]->frame()));
		if (_config.devices.back.enable)	cv::imshow("camera 1", cv::Mat(_cameras[1]->frame()));
	}
	
	// ===============================================================
	// =                         T I M E R S                         =
	// ===============================================================
	NEWCHRONO(t8)
	
	if (_config.general.verbose > 1)
	{
		printf("User interaction:       %8d µs\n",		std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count());
		printf("Grabbing frames:        %8d µs\n",		std::chrono::duration_cast<std::chrono::microseconds>(t3-t2).count());
		printf("Positionning:           %8d µs\n",		std::chrono::duration_cast<std::chrono::microseconds>(t4-t3).count());
		printf("Bilding EnvMap:         %8d µs\n",		std::chrono::duration_cast<std::chrono::microseconds>(t5-t4).count());
		printf("Rendering - background: %8d µs\n",		std::chrono::duration_cast<std::chrono::microseconds>(t6-t5).count());
		printf("Rendering - scene:      %8d µs\n",		std::chrono::duration_cast<std::chrono::microseconds>(t7-t6).count());
		printf("Rendering - frames:     %8d µs\n\n",	std::chrono::duration_cast<std::chrono::microseconds>(t8-t7).count());
	}
	
	present();
	cv::waitKey(30);
	return 1;
}





// ############################################################################


void Core::processKeyboardEvent()
{
	if (key('b'))
	{
		key('b') = 0;
		_buildenvmap = !_buildenvmap;
		if (_config.general.verbose) printf("- build envmap : %s\n", _buildenvmap?"on":"off");
	}
	
	if (key('c'))
	{
		key('c') = 0;
		_envmap.clear();
		if (_config.general.verbose) printf("- envmap cleared\n");
	}
	
	if (key('m'))
	{
		key('m') = 0;
		_newmethod = !_newmethod;
		if (_config.general.verbose) printf("- switch to %s methode\n", (_newmethod?std::string("new"):std::string("old")).c_str());
	}
}


// ############################################################################


void Core::processKeyboardEvent(SDL_KeyboardEvent& event)
{	
	if (event.state == SDL_PRESSED)
		switch (event.keysym.sym)
		{
			case SDLK_F1:
			{
				time_t now = time(0);
				std::stringstream path;
				path << "data/view/screenshot_" << now << ".png";
				gk::writeFramebuffer(path.str());
				break;
			}
			case SDLK_F2:
			{
				time_t now = time(0);
				std::stringstream path;
				path << "data/view/envmap_" << now << "_%03d.png";
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(getGLResource<gk::GLTexture>("tex:envmap")->target, _GLResources["tex:envmap"]->name);
				gk::ImageIO::writeImageArray(path.str(), getGLResource<gk::GLTexture>("tex:envmap")->imageArray(0));
				break;
			}
			case SDLK_F5:
			{
				gk::reloadPrograms();
				break;
			}
			case SDLK_F9:
			{
				gk::writeOrbiter(_debugviewpoint, ".debugviewpoint.save");
				break;
			}
			case SDLK_F10:
			{
				_debugviewpoint = gk::readOrbiter(".debugviewpoint.save");
				break;
			}
			case SDLK_ESCAPE:
			{
				closeWindow();
				break;
			}
			case SDLK_RIGHT:
			{
				int brightness;
				if (_config.devices.front.enable)
				{
					brightness = std::min(_cameras[0]->getParameter(VideoDevice::BRIGHTNESS) + 8, 255);
					_cameras[0]->setParameter(VideoDevice::BRIGHTNESS, brightness);
					if (_config.general.verbose) printf("[CAMERA %d] brightness set to %d\n", 0, brightness);
				}
				if (_config.devices.back.enable)
				{
					brightness = std::min(_cameras[1]->getParameter(VideoDevice::BRIGHTNESS) + 8, 255);
					_cameras[1]->setParameter(VideoDevice::BRIGHTNESS, brightness);
					if (_config.general.verbose) printf("[CAMERA %d] brightness set to %d\n", 1, brightness);
				}
				break;
			}
			case SDLK_LEFT:
			{
				if (_config.devices.front.enable)
				{
					int brightness = std::max(_cameras[0]->getParameter(VideoDevice::BRIGHTNESS) - 8, 0);
					_cameras[0]->setParameter(VideoDevice::BRIGHTNESS, brightness);
					if (_config.general.verbose) printf("[CAMERA %d] brightness set to %d\n", 0, brightness);
				}
				if (_config.devices.back.enable)
				{
					int brightness = std::max(_cameras[1]->getParameter(VideoDevice::BRIGHTNESS) - 8, 0);
					_cameras[1]->setParameter(VideoDevice::BRIGHTNESS, brightness);
					if (_config.general.verbose) printf("[CAMERA %d] brightness set to %d\n", 1, brightness);
				}
				break;
			}
			case SDLK_UP:
			{
				if (_config.devices.front.enable)
				{
					int gain = std::min(_cameras[0]->getParameter(VideoDevice::GAIN) + 8, 255);
					_cameras[0]->setParameter(VideoDevice::GAIN, gain);
					if (_config.general.verbose) printf("[CAMERA %d] gain set to %d\n", 0, gain);
				}
				if (_config.devices.back.enable)
				{
					int gain = std::min(_cameras[1]->getParameter(VideoDevice::GAIN) + 8, 255);
					_cameras[1]->setParameter(VideoDevice::GAIN, gain);
					if (_config.general.verbose) printf("[CAMERA %d] gain set to %d\n", 1, gain);
				}
				break;
			}
			case SDLK_DOWN:
			{
				if (_config.devices.front.enable)
				{
					int gain = std::max(_cameras[0]->getParameter(VideoDevice::GAIN) - 8, 0);
					_cameras[0]->setParameter(VideoDevice::GAIN, gain);
					if (_config.general.verbose) printf("[CAMERA %d] gain set to %d\n", 0, gain);
				}
				if (_config.devices.back.enable)
				{
					int gain = std::max(_cameras[1]->getParameter(VideoDevice::GAIN) - 8, 0);
					_cameras[1]->setParameter(VideoDevice::GAIN, gain);
					if (_config.general.verbose) printf("[CAMERA %d] gain set to %d\n", 1, gain);
				}
				break;
			}
			default:
				if (_config.general.verbose) printf("Key %d unmapped\n", (int) event.keysym.sym);
				break;
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

