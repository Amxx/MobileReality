#include "core.hh"

// ############################################################################
// #                                  MACROS                                  #
// ############################################################################

#define  LOGHERE  std::cout << "[HERE] " << __FILE__ << " : " << __LINE__ << std::endl;

// ############################################################################
// #                                  DEFINE                                  #
// ############################################################################

// =========== CAMERA OPTIONS (OTHER OPTIONS MAY SUFFER FROM THIS) ============
// #define 	DISABLE_CAMERA_0
// #define 	DISABLE_CAMERA_1
#define		DEFAULT_BRIGHTNESS	128																								// NEEDED
#define		DEFAULT_GAIN				16																								// NEEDED

// =========== ENVMAP OPTIONS (OTHER OPTIONS MAY SUFFER FROM THIS) ============
// #define 	DISABLE_ENVMAP
// #define	DUAL_ACQUISITION
#define 	ENVMAP_SIZE					cv::Size(256, 256)																// NEEDED

// =========== RENDERING OPTIONS ============
// #define	DISABLE_BACKGROUND_RENDERING
// #define	DISABLE_SCENE_RENDERING
#define		VALIDFRAMES					10																								//NEEDED (DEFAULT 1)
// ============== VIEW OPTIONS ==============
#define 	DISABLE_VIEW

// ============= OTHERS OPTIONS =============
// #define		VERBOSE
#define		CONTROL							VideoDevice::BRIGHTNESS														// NEEDED









// ############################################################################
// #                                 METHODS                                  #
// ############################################################################

Core::Core(int argc, char* argv[]) :
	gk::App(),
	
	_cameras(2),
	_envmap(ENVMAP_SIZE),
	
	_scale(6.0),
	_subscale(236.0/300.0),
	
	_buildenvmap(true)
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
			_cameras[0] = new Camera(videodevice, cv::Matx44f(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0), _config("params-front"));
			if (_config("video-front-id").size()) _cameras[0]->open(atoi(_config("video-front-id").c_str()));
			_cameras[0]->openAndCalibrate(_config("params-front"), *_scanner);
		#endif
		#ifndef DISABLE_CAMERA_1
			videodevice = loadVideoDevice(_config("video"));
			if (videodevice == nullptr) exit(1);
			_cameras[1] = new Camera(videodevice,  cv::Matx44f(-1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0), _config("params-back"));
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
			_cameras[0]->setParameter(VideoDevice::BRIGHTNESS,	(brightness>0)?brightness:DEFAULT_BRIGHTNESS);
			_cameras[0]->setParameter(VideoDevice::GAIN,				(gain>0)?gain:DEFAULT_GAIN);
			#ifdef VERBOSE
				std::cout << "============ CAMERA 0 ============" << std::endl;
				std::cout << "BRIGHTNESS : " << brightness				<< std::endl;
				std::cout << "GAIN       : " << gain							<< std::endl;
				std::cout << "==================================" << std::endl;
				// _cameras[0]->showParameters();
			#endif
		#endif
		#ifndef DISABLE_CAMERA_1
			brightness	= _cameras[1]->getParameter(VideoDevice::BRIGHTNESS);
			gain				= _cameras[1]->getParameter(VideoDevice::GAIN);
			_cameras[1]->setParameter(VideoDevice::MODE,				VideoDevice::MANUALEXPOSURE);
			_cameras[1]->setParameter(VideoDevice::BRIGHTNESS,	(brightness>0)?brightness:DEFAULT_BRIGHTNESS);
			_cameras[1]->setParameter(VideoDevice::GAIN,				(gain>0)?gain:DEFAULT_GAIN);
			#ifdef VERBOSE
				std::cout << "============ CAMERA 1 ============" << std::endl;
				std::cout << "BRIGHTNESS : " << brightness				<< std::endl;
				std::cout << "GAIN       : " << gain							<< std::endl;
				std::cout << "==================================" << std::endl;
				// _cameras[1]->showParameters();
			#endif
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

	_GLPrograms["materialrendering"] = gk::createProgram("materialrendering.glsl");
	if (_GLPrograms["materialrendering"] == gk::GLProgram::null()) return -1;
	glBindAttribLocation(_GLPrograms["materialrendering"]->name, 0, "position");
	glBindAttribLocation(_GLPrograms["materialrendering"]->name, 1, "normal");
	glBindAttribLocation(_GLPrograms["materialrendering"]->name, 2, "texcoord");
	
	// ===============================================================
	// =                C R E A T E   T E X T U R E S                =
	// ===============================================================
	
	_GLTextures["frame0"] = (new gk::GLTexture())->createTexture2D(0, 640, 480);
	_GLTextures["envmap"] = (new gk::GLTexture())->createTextureCube(1, ENVMAP_SIZE.width, ENVMAP_SIZE.height);
	
	// ===============================================================
	// =                    C R E A T E   M E S H                    =
	// ===============================================================
	
	gk::Mesh *mesh = gk::MeshIO::readOBJ(_config("obj"));
	if (mesh == nullptr) return -1;
	
	_mesh = new gk::GLBasicMesh(GL_TRIANGLES, mesh->indices.size());
	_mesh->createBuffer(0,	mesh->positions);
  _mesh->createBuffer(1,	mesh->texcoords);
	_mesh->createBuffer(2,		mesh->normals);
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
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	// ===============================================================
	// =               B A C K G R O U N D   F R A M E               =
	// ===============================================================
	
	#ifndef DISABLE_BACKGROUND_RENDERING	
		glClear(GL_DEPTH_BUFFER_BIT);	
		glBindTexture(_GLTextures["frame0"]->target, _GLTextures["frame0"]->name);
		glTexSubImage2D(_GLTextures["frame0"]->target, 0, 0, 0, _cameras[0]->frame()->width, _cameras[0]->frame()->height, GL_BGR, GL_UNSIGNED_BYTE, _cameras[0]->frame()->imageData);
				
		glUseProgram(_GLPrograms["background"]->name);
		_GLPrograms["background"]->sampler("frame") = 0;
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	#endif
	
	// ===============================================================
	// =                R E A D I N G   S Y M B O L S                =
	// ===============================================================
	
	std::vector<Symbol> symbols = _scanner->scan(_cameras[0]->frame());	
	
	// ===============================================================
	// =                    P O S I T I O N I N G                    =
	// ===============================================================
	
	static cv::Matx44f	model;																				// Presistent	: Model
	static cv::Matx44f	view;																					// Persistent	: View (bloc camera)
	static int					presist	= 0;																	// Persistent : Persistency duration
	
	for (Symbol& symbol : symbols)
		try {
			static cv::Matx44f toGL(1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
			// LEGACY MODE FOR CUBE
			try 				{	model = parseSymbolToModel(symbol.data, _scale)																	* toGL;	}
			catch (...)	{ model = parseMatx33f_tr(symbol.data, cv::Matx31f(_scale/2, _scale/2, _scale/2))	* toGL;	}
			symbol.extrinsic(_cameras[0]->A(), _cameras[0]->K(), Scanner::pattern(_scale, _subscale));
			view		=	_cameras[0]->orientation().inv() * viewFromSymbol(symbol.rvec, symbol.tvec);
			presist	= (!isNull(view) && !isNull(model))?VALIDFRAMES:0;
			if (presist) break;
		} catch (...) {
			std::cout << "Invalid symbol, could not extract model informations from `" << symbol.data << "`" << std::endl;
		}
	
	if (presist && presist--)
	{		
	// ===============================================================
	// =                B U I L D I N G   E N V M A P                =
	// ===============================================================
		#ifndef DISABLE_ENVMAP
			if (_buildenvmap)
			{
				_envmap.addFrame(*_cameras[1], view * model);
				#ifdef DUAL_ACQUISITION
					_envmap.addFrame(*_cameras[0], view * model);
				#endif
			}
		#endif
		
	// ===============================================================
	// =                S C E N E   R E N D E R I N G                =
	// ===============================================================
		#ifndef DISABLE_SCENE_RENDERING
			glClear(GL_DEPTH_BUFFER_BIT);	

			glBindTexture(_GLTextures["envmap"]->target, _GLTextures["envmap"]->name);
			glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X,	0, 0, 0, ENVMAP_SIZE.width, ENVMAP_SIZE.height, GL_BGR, GL_FLOAT, _envmap[0]->ptr());
			glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X,	0, 0, 0, ENVMAP_SIZE.width, ENVMAP_SIZE.height, GL_BGR, GL_FLOAT, _envmap[1]->ptr());
			glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y,	0, 0, 0, ENVMAP_SIZE.width, ENVMAP_SIZE.height, GL_BGR, GL_FLOAT, _envmap[2]->ptr());
			glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,	0, 0, 0, ENVMAP_SIZE.width, ENVMAP_SIZE.height, GL_BGR, GL_FLOAT, _envmap[3]->ptr());
			glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z,	0, 0, 0, ENVMAP_SIZE.width, ENVMAP_SIZE.height, GL_BGR, GL_FLOAT, _envmap[4]->ptr());
			glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,	0, 0, 0, ENVMAP_SIZE.width, ENVMAP_SIZE.height, GL_BGR, GL_FLOAT, _envmap[5]->ptr());
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		
			static	gk::Transform		scale	= gk::Scale(atof(_config("obj-scale").c_str()));
			static	gk::Transform		proj	= cv2gkit(projectionFromIntrinsic(_cameras[0]->A(), _cameras[0]->frame()->width, _cameras[0]->frame()->height, 1.0, 10000.0));
		//static	gk::Transform		proj	= cv2gkit(projectionFromIntrinsic(_cameras[0]->A(), windowWidth(), windowHeight(), 1.0, 10000.0));
			
			gk::Transform 					mv		=	cv2gkit(_cameras[0]->orientation() * view * model) * scale;	// Point de vue de _cameras[0]
			gk::Transform 					mvp		= proj * mv;																									// Projection de _cameras[0]

			glUseProgram(_GLPrograms["materialrendering"]->name);    
			_GLPrograms["materialrendering"]->uniform("mvMatrix")				= mv.matrix();
			_GLPrograms["materialrendering"]->uniform("mvMatrixInv")		= mv.inverseMatrix();
			_GLPrograms["materialrendering"]->uniform("mvpMatrix")			= mvp.matrix();
		//_GLPrograms["materialrendering"]->uniform("ka")							= 0.0f;			// 1.0
			_GLPrograms["materialrendering"]->uniform("kd")							= 0.5f;				// 0.6
			_GLPrograms["materialrendering"]->uniform("ks")							= 0.5f;				// 0.3
			_GLPrograms["materialrendering"]->uniform("ns")							= 4096.0f;		// 64.0
		//_GLPrograms["materialrendering"]->uniform("ambient_color")	= gk::VecColor(1, 1, 1, 1);
		//_GLPrograms["materialrendering"]->uniform("diffuse_color")	= gk::VecColor(1, 1, 1, 1);
		//_GLPrograms["materialrendering"]->uniform("specular_color")	= gk::VecColor(1, 1, 1, 1);    
		//_GLPrograms["materialrendering"]->uniform("ambient_light")	= gk::VecColor(1, 1, 1, 1);
		//_GLPrograms["materialrendering"]->uniform("diffuse_light")	= gk::VecColor(1, 1, 1, 1);
		//_GLPrograms["materialrendering"]->uniform("specular_light")	= gk::VecColor(1, 1, 1, 1);
			_GLPrograms["materialrendering"]->sampler("envmap")					= 1;
			
			_mesh->draw();
		#endif
	}
	present();
	
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
			cv::imshow("Envmap 1 : +X", _envmap.color(0));
			cv::imshow("Envmap 2 : -X", _envmap.color(1));
			cv::imshow("Envmap 3 : +Y", _envmap.color(2));
			cv::imshow("Envmap 4 : -Y", _envmap.color(3));
			cv::imshow("Envmap 5 : +Z", _envmap.color(4));
			cv::imshow("Envmap 6 : -Z", _envmap.color(5));
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
			case SDLK_F1:
			{
				_buildenvmap = !_buildenvmap;
				#ifdef VERBOSE 
					printf("build envmap : %s\n", _buildenvmap?"on":"off");
				#endif
				break;
			}
			case SDLK_F2:
			{
				_envmap.clear();
				#ifdef VERBOSE 
					printf("envmap cleared\n");
				#endif
				break;
			}
			case SDLK_F3:
			{
				time_t now = time(0);
				std::stringstream path;
				path << "data/env/envmap_" << now << ".png";
				#ifdef VERBOSE 
					printf("writing color envmap '%s' ... \n", path.str().c_str());
				#endif
				_envmap.save(path.str());				
				break;
			}
			
			case SDLK_F4:
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
			
			case SDLK_RIGHT:
			{
				int brightness;
				#ifndef DISABLE_CAMERA_0	
					brightness = std::min(_cameras[0]->getParameter(VideoDevice::BRIGHTNESS) + 8, 255);
					_cameras[0]->setParameter(VideoDevice::BRIGHTNESS, brightness);
					#ifdef VERBOSE 
						printf("[CAMERA %d] brightness set to %d\n", 0, brightness);
					#endif
				#endif
				#ifndef DISABLE_CAMERA_1
					brightness = std::min(_cameras[1]->getParameter(VideoDevice::BRIGHTNESS) + 8, 255);
					_cameras[1]->setParameter(VideoDevice::BRIGHTNESS, brightness);
					#ifdef VERBOSE 
						printf("[CAMERA %d] brightness set to %d\n", 1, brightness);
					#endif
				#endif
				break;
			}
			case SDLK_LEFT:
			{
				int brightness;
				#ifndef DISABLE_CAMERA_0	
					brightness = std::max(_cameras[0]->getParameter(VideoDevice::BRIGHTNESS) - 8, 0);
					_cameras[0]->setParameter(VideoDevice::BRIGHTNESS, brightness);
					#ifdef VERBOSE 
						printf("[CAMERA %d] brightness set to %d\n", 0, brightness);
					#endif
				#endif
				#ifndef DISABLE_CAMERA_1
					brightness = std::max(_cameras[1]->getParameter(VideoDevice::BRIGHTNESS) - 8, 0);
					_cameras[1]->setParameter(VideoDevice::BRIGHTNESS, brightness);
					#ifdef VERBOSE 
						printf("[CAMERA %d] brightness set to %d\n", 1, brightness);
					#endif
				#endif
				break;
			}
			case SDLK_UP:
			{
				int gain;
				#ifndef DISABLE_CAMERA_0	
					gain = std::min(_cameras[0]->getParameter(VideoDevice::GAIN) + 8, 255);
					_cameras[0]->setParameter(VideoDevice::GAIN, gain);
					#ifdef VERBOSE 
						printf("[CAMERA %d] gain set to %d\n", 0, gain);
					#endif
				#endif
				#ifndef DISABLE_CAMERA_0	
					gain = std::min(_cameras[1]->getParameter(VideoDevice::GAIN) + 8, 255);
					_cameras[1]->setParameter(VideoDevice::GAIN, gain);
					#ifdef VERBOSE 
						printf("[CAMERA %d] gain set to %d\n", 1, gain);
					#endif
				#endif
				break;
			}
			case SDLK_DOWN:
			{
				int gain;
				#ifndef DISABLE_CAMERA_0	
					gain = std::max(_cameras[0]->getParameter(VideoDevice::GAIN) - 8, 0);
					_cameras[0]->setParameter(VideoDevice::GAIN, gain);
					#ifdef VERBOSE 
						printf("[CAMERA %d] gain set to %d\n", 0, gain);
					#endif
				#endif
				#ifndef DISABLE_CAMERA_0	
					gain = std::max(_cameras[1]->getParameter(VideoDevice::GAIN) - 8, 0);
					_cameras[1]->setParameter(VideoDevice::GAIN, gain);
					#ifdef VERBOSE 
						printf("[CAMERA %d] gain set to %d\n", 1, gain);
					#endif
				#endif
				break;
			}
			
			default:
				#ifdef VERBOSE			
					printf("Key %d unmapped\n", (int) event.keysym.sym);
				#endif
				break;
		}
}





// ############################################################################





void Core::processWindowResize(SDL_WindowEvent &event)
{
	glViewport(0, 0, windowWidth(), windowHeight());
}





// ############################################################################





void Core::processMouseButtonEvent(SDL_MouseButtonEvent &event)
{
}





// ############################################################################





void Core::processMouseMotionEvent(SDL_MouseMotionEvent &event)
{
}	


