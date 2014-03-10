#include "core.hh"

// ############################################################################
// #                                  MACROS                                  #
// ############################################################################

#define  LOGHERE  std::cout << "[HERE] " << __FILE__ << " : " << __LINE__ << std::endl;

// ############################################################################
// #                                  DEFINE                                  #
// ############################################################################

// #define 	DISABLE_CAMERA_0
// #define 	DISABLE_CAMERA_1
// #define 	DISABLE_ENVMAP
// #define	DUAL_ACQUISITION
#define 	DISABLE_VIEW


#define		CONTROL					VideoDevice::BRIGHTNESS
#define 	ENVMAP_SIZE			cv::Size(128, 128)

// ############################################################################
// #                                 METHODE                                  #
// ############################################################################

Core::Core(int argc, char* argv[]) :
	gk::App(),
	
	_cameras(2),
	_envmap(ENVMAP_SIZE),
	
	_scale(6.0),
	_subscale(236.0/300.0),
	_objsize(6.0)
{
	// ===============================================================
	// =   C O N F I G U R A T I O N   I N I T I A L I S A T I O N   =
	// ===============================================================
		
	_config("obj")					= "cube.obj";

	_config("params-front")	= "params/default-front.xml";
	_config("params-back")	= "params/default-back.xml";
	_config("video")				= "install/share/libvideodevice_uvc.so";
	_config("scanner")			= "install/share/libscanner_zbar.so";

	if (argc > 1) _config.load(argv[1]);

	if (_config("scale").size())		_scale		= atof(_config("scale").c_str());
	if (_config("subscale").size())	_subscale = atof(_config("subscale").c_str());
	if (_config("obj-size").size())	_objsize	= atof(_config("obj-size").c_str());
	
	// ===============================================================
	// =                   L O A D   S C A N N E R                   =
	// ===============================================================
		
	_scanner	= loadScanner(_config("scanner"));
	if (_scanner == nullptr) exit(1);
	
	// ===============================================================
	// =                   L O A D   C A M E R A S                   =
	// ===============================================================
	#if !defined(DISABLE_CAMERA_0) || !defined(DISABLE_CAMERA_1)
		VideoDevice* videodevice;
		#ifndef DISABLE_CAMERA_0
			videodevice = loadVideoDevice(_config("video"));
			if (videodevice == nullptr) exit(1);
			_cameras[0] = new Camera(videodevice, cv::Matx33f(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0), _config("params-front"));
			if (_config("video-front-id").size()) _cameras[0]->open(atoi(_config("video-front-id").c_str()));
			_cameras[0]->openAndCalibrate(_config("params-front"), *_scanner);
		#endif
		#ifndef DISABLE_CAMERA_1
			videodevice = loadVideoDevice(_config("video"));
			if (videodevice == nullptr) exit(1);
			_cameras[1] = new Camera(videodevice,  cv::Matx33f(-1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, -1.0), _config("params-back"));
			if (_config("video-back-id").size()) _cameras[1]->open(atoi(_config("video-back-id").c_str()));
			_cameras[1]->openAndCalibrate(_config("params-back"), *_scanner);
		#endif
	#endif
	
	// ===============================================================
	// =                  S E T U P   C A M E R A S                  =
	// ===============================================================
	#if !defined(DISABLE_CAMERA_0) || !defined(DISABLE_CAMERA_1)
		std::cout << "Waiting for camera to set autoexposure ... " << std::flush;
		sleep(1);
		std::cout << "done" << std::endl;
	
		int brightness, gain;
	
		#ifndef DISABLE_CAMERA_0
			brightness	= _cameras[0]->getParameter(VideoDevice::BRIGHTNESS);
			gain				= _cameras[0]->getParameter(VideoDevice::GAIN);
			_cameras[0]->setParameter(VideoDevice::MODE,				VideoDevice::MANUALEXPOSURE);
			_cameras[0]->setParameter(VideoDevice::BRIGHTNESS,	(brightness>0)?brightness:128);
			_cameras[0]->setParameter(VideoDevice::GAIN,				(gain>0)?gain:16);
			// _cameras[0]->showParameters();
		#endif
		#ifndef DISABLE_CAMERA_1
			brightness	= _cameras[1]->getParameter(VideoDevice::BRIGHTNESS);
			gain				= _cameras[1]->getParameter(VideoDevice::GAIN);
			_cameras[1]->setParameter(VideoDevice::MODE,				VideoDevice::MANUALEXPOSURE);
			_cameras[1]->setParameter(VideoDevice::BRIGHTNESS,	(brightness>0)?brightness:128);
			_cameras[1]->setParameter(VideoDevice::GAIN,				(gain>0)?gain:16);
			// _cameras[1]->showParameters();
		#endif		
	#endif

	// ===============================================================
	// =                   R E A D Y   T O   R U N                   =
	// ===============================================================
	
	gk::AppSettings settings;
	settings.setGLVersion(3,1);
	// settings.setFullscreen();
	if(createWindow(640, 480, settings) < 0) closeWindow();
	
}

// ############################################################################

Core::~Core()
{
	for (Camera* camera : _cameras)
		if (camera)
			camera->close();
}

// ############################################################################

int Core::init()
{
	
	glClearColor(0.1, 0.1, 0.1, 1.0);
	
	// ===============================================================
	// =        L O A D   S H A D E R S   A S   P R O G R A M        =
	// ===============================================================
	
	gk::programPath("install/shaders");
	
	_GLPrograms["background"] = gk::createProgram("background.glsl");
	if (_GLPrograms["background"] == gk::GLProgram::null()) return -1;

	_GLPrograms["rendering"] = gk::createProgram("rendering.glsl");
	if (_GLPrograms["rendering"] == gk::GLProgram::null()) return -1;
	
	glBindAttribLocation(_GLPrograms["rendering"]->name, 0, "position");
	glBindAttribLocation(_GLPrograms["rendering"]->name, 1, "normal");
	glBindAttribLocation(_GLPrograms["rendering"]->name, 1, "texcoord");
	
	// ===============================================================
	// =                C R E A T E   T E X T U R E S                =
	// ===============================================================
	
	_GLTextures["frame0"] = (new gk::GLTexture())->createTexture2D(0, 640, 480);
	_GLTextures["envmap"] = (new gk::GLTexture())->createTexture2D(1, ENVMAP_SIZE.width, ENVMAP_SIZE.height);
	
	// ===============================================================
	// =                    C R E A T E   M E S H                    =
	// ===============================================================
	
	gk::Mesh *mesh = gk::MeshIO::readOBJ(_config("obj"));
	if (mesh == nullptr) return -1;
	
	_mesh = new gk::GLBasicMesh(GL_TRIANGLES, mesh->indices.size());
	_mesh->createBuffer(0, mesh->positions);
	_mesh->createBuffer(1, mesh->normals);
	_mesh->createBuffer(2, mesh->texcoords);
	_mesh->createIndexBuffer(mesh->indices);
	
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
	// ===============================================================
	// =                 G R A B I N G   I M A G E S                 =
	// ===============================================================
	#ifndef DISABLE_CAMERA_0
		if (_cameras[0]) _cameras[0]->grabFrame();
	#endif
	#ifndef DISABLE_CAMERA_1
		if (_cameras[1]) _cameras[1]->grabFrame();
	#endif
	
	// ===============================================================
	// =                R E A D I N G   S Y M B O L S                =
	// ===============================================================
	std::vector<Symbol> symbols = _scanner->scan(_cameras[0]->frame());	

	// ===============================================================
	// =                B U I L D I N G   E N V M A P                =
	// ===============================================================
	#ifndef DISABLE_ENVMAP
		cv::Matx33f modelview = ModelView(*_cameras[0], *_scanner);
		_envmap.addFrame(*_cameras[1], modelview);
		#ifdef DUAL_ACQUISITION
			_envmap.addFrame(*_cameras[0], modelview);
		#endif
	#endif
	
	// ===============================================================
	// =                S C E N E   R E N D E R I N G                =
	// ===============================================================
	#ifndef DISABLE_RENDERING
		glBindTexture(_GLTextures["frame0"]->target, _GLTextures["frame0"]->name);
		glTexSubImage2D(_GLTextures["frame0"]->target, 0, 0, 0, _cameras[0]->frame()->width, _cameras[0]->frame()->height, GL_BGR, GL_UNSIGNED_BYTE, _cameras[0]->frame()->imageData);
	
		glBindTexture(_GLTextures["envmap"]->target, _GLTextures["envmap"]->name);
		glTexSubImage2D(_GLTextures["envmap"]->target, 0, 0, 0, ENVMAP_SIZE.width, ENVMAP_SIZE.height, GL_BGR, GL_FLOAT, _envmap.color().ptr());
	
	
	// ===============================================================
	// =               B A C K G R O U N D   F R A M E               =
	// ===============================================================
		glClear(GL_DEPTH_BUFFER_BIT);	
			
		glUseProgram(_GLPrograms["background"]->name);
		_GLPrograms["background"]->sampler("frame") 			= 0;
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	// ===============================================================
	// =                S C E N E   R E N D E R I N G                =
	// ===============================================================
		glClear(GL_DEPTH_BUFFER_BIT);	

		static const	gk::Transform		proj		= cv2gkit(projectionFromIntrinsic(_cameras[0]->A(), windowWidth(), windowHeight(), 1.0, 10000.0));
		static 				gk::Transform		view		= gk::Transform();
		static 				gk::Transform		model		= gk::Transform();
		static const	gk::Transform		scale		= gk::Scale(atof(_config("obj-scale").c_str()));
		static const	gk::Transform		toGL		= gk::Transform(gk::Matrix4x4(1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0));	//.t()

		glUseProgram(_GLPrograms["rendering"]->name);
		_GLPrograms["rendering"]->uniform("obj_color")		= gk::VecColor(0.0, 0.5, 0.0);
		_GLPrograms["rendering"]->sampler("envmap")				= 1;
	
		for (Symbol& symbol : symbols)
			try {
				// LEGACY MODE FOR CUBE
				try 				{	model = cv2gkit(parseSymbolToModel(symbol.data, _scale));																	}
				catch (...)	{ model = cv2gkit(parseMatx33f_tr(symbol.data, cv::Matx31f(_scale/2, _scale/2, _scale/2)));	}
			
				symbol.extrinsic(_cameras[0]->A(), _cameras[0]->K(), Scanner::pattern(_scale, _subscale));
				view = cv2gkit(viewFromSymbol(symbol.rvec, symbol.tvec));
			
				gk::Transform mv	=	view * model * scale * toGL;
				gk::Transform mvp = proj * mv;
			
				_GLPrograms["rendering"]->uniform("mvMatrix")			= mv.matrix();
				_GLPrograms["rendering"]->uniform("mvnMatrix")		= mv.normalMatrix();
				_GLPrograms["rendering"]->uniform("mvnMatrixInv")	= mv.inverse().normalMatrix();
			
				_GLPrograms["rendering"]->uniform("mvpMatrix")		= mvp.matrix();
				_mesh->draw();

			} catch (...) {
				std::cout << "Invalid symbol, could not extract model informations from `" << symbol.data << "`" << std::endl;
			}
		
		present();
	#endif
		
	
	// ===============================================================
	// =                  F R A M E   D I S P L A Y                  =
	// ===============================================================
	#ifndef DISABLE_VIEW
		#ifndef DISABLE_CAMERA_0
			cv::imshow("camera 0",								cv::Mat(_cameras[0]->frame()));
		#endif
		#ifndef DISABLE_CAMERA_1
			cv::imshow("camera 1",								cv::Mat(_cameras[1]->frame()));
		#endif
		#ifndef DISABLE_ENVMAP
			cv::imshow("Environnement Color", _envmap.color());
			cv::imshow("Environnement Lumin", _envmap.lumin());
		#endif
		cv::waitKey(30);
	#endif
	
	return 1;
}

// ############################################################################

void Core::processKeyboardEvent(SDL_KeyboardEvent& event)
{
	if (event.state == SDL_PRESSED)
		switch (event.keysym.sym)
		{
			case 'c':
			{
				_envmap.clear();
				break;
			}
			case 's':		
			{
				time_t now = time(0);
				std::stringstream path_color;
				path_color << "data/env/envmap_" << now << ".png";
				std::cout << "writing color envmap '" << path_color.str() << "' ... " << std::flush;
				EnvMap::save(_envmap.color(), path_color.str());				
				std::cout << "done" << std::endl;
				break;
			}
			
			case SDLK_F6:
			{
				time_t now = time(0);
				std::stringstream path;
				path << "data/view/screenshot_" << now << ".png";
				gk::writeFramebuffer(path.str());
				break;
			}
			
			case SDLK_F5:
				gk::reloadPrograms();
				break;
			
			case SDLK_ESCAPE:
				closeWindow();
				break;
			
			/*
			case SDLK_F1:
				if (_cameras[0]) _cameras[0]->setParameter(CONTROL, 0);
				if (_cameras[1]) _cameras[1]->setParameter(CONTROL, 0);
				break;
			case SDLK_F2:
				if (_cameras[0]) _cameras[0]->setParameter(CONTROL, 25);
				if (_cameras[1]) _cameras[1]->setParameter(CONTROL, 25);
				break;
			case SDLK_F3:
				if (_cameras[0]) _cameras[0]->setParameter(CONTROL, 51);
				if (_cameras[1]) _cameras[1]->setParameter(CONTROL, 51);
				break;
			case SDLK_F4:
				if (_cameras[0]) _cameras[0]->setParameter(CONTROL, 76);
				if (_cameras[1]) _cameras[1]->setParameter(CONTROL, 76);
				break;
			case SDLK_F5:
				if (_cameras[0]) _cameras[0]->setParameter(CONTROL, 102);
				if (_cameras[1]) _cameras[1]->setParameter(CONTROL, 102);
				break;
			case SDLK_F6:
				if (_cameras[0]) _cameras[0]->setParameter(CONTROL, 127);
				if (_cameras[1]) _cameras[1]->setParameter(CONTROL, 127);
				break;
			case SDLK_F7:
				if (_cameras[0]) _cameras[0]->setParameter(CONTROL, 153);
				if (_cameras[1]) _cameras[1]->setParameter(CONTROL, 153);
				break;
			case SDLK_F8:
				if (_cameras[0]) _cameras[0]->setParameter(CONTROL, 178);
				if (_cameras[1]) _cameras[1]->setParameter(CONTROL, 178);
				break;
			case SDLK_F9:
				if (_cameras[0]) _cameras[0]->setParameter(CONTROL, 204);
				if (_cameras[1]) _cameras[1]->setParameter(CONTROL, 204);
				break;
			case SDLK_F10:
				if (_cameras[0]) _cameras[0]->setParameter(CONTROL, 229);
				if (_cameras[1]) _cameras[1]->setParameter(CONTROL, 229);
				break;
			case SDLK_F11:	
				if (_cameras[0]) _cameras[0]->setParameter(CONTROL, 255);
				if (_cameras[1]) _cameras[1]->setParameter(CONTROL, 255);
				break;
			case 13:
				if (_cameras[0]) _cameras[0]->resetParameter(CONTROL);
				if (_cameras[1]) _cameras[1]->resetParameter(CONTROL);
				break;
			*/
			
			
			default:
				printf("Key %d unmapped\n", (int) event.keysym.sym);
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


