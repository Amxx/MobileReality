#include "core.hh"

// ============ MACROS ============
#define  LOGHERE  std::cout << "[HERE] " << __FILE__ << " : " << __LINE__ << std::endl;


// #define DISABLE_CAMERA_0
// #define DISABLE_CAMERA_1
#define DISABLE_ENVMAP


Core::Core(int argc, char* argv[]) :
	gk::App(),
	
	_cameras(2),
	_envmap(cv::Size(640, 480)),
	
	_scale(6.0),
	_subscale(236.0/300.0),
	_objsize(6.0)
{
	// ===============================================================
	// =   C O N F I G U R A T I O N   I N I T I A L I S A T I O N   =
	// ===============================================================
		
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
		
	VideoDevice* videodevice;
	
	#ifndef DISABLE_CAMERA_0
		videodevice = loadVideoDevice(_config("video"));
		if (videodevice == nullptr) exit(1);
		_cameras[0] = new Camera(videodevice, cv::Matx33f(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0), _config("params-front"));
		if (_config("video-front-id").size()) _cameras[0]->open(atoi(_config("video-front-id").c_str()));
		_cameras[0]->openAndCalibrate(_config("params-front"), *_scanner);
	
		_cameras[0]->setParameter(VideoDevice::MODE,				VideoDevice::MANUALEXPOSURE);
		_cameras[0]->setParameter(VideoDevice::BRIGHTNESS,	_cameras[0]->getParameter(VideoDevice::BRIGHTNESS	));
		_cameras[0]->setParameter(VideoDevice::GAIN,				_cameras[0]->getParameter(VideoDevice::GAIN				));
		// _cameras[0]->showParameters();
	
	#endif
	#ifndef DISABLE_CAMERA_1
		videodevice = loadVideoDevice(_config("video"));
		if (videodevice == nullptr) exit(1);
		_cameras[1] = new Camera(videodevice,  cv::Matx33f(-1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, -1.0), _config("params-back"));
		if (_config("video-back-id").size()) _cameras[1]->open(atoi(_config("video-back-id").c_str()));
		_cameras[1]->openAndCalibrate(_config("params-back"), *_scanner);
	
		_cameras[1]->setParameter(VideoDevice::MODE,				VideoDevice::MANUALEXPOSURE);
		_cameras[1]->setParameter(VideoDevice::BRIGHTNESS,	_cameras[1]->getParameter(VideoDevice::BRIGHTNESS	));
		_cameras[1]->setParameter(VideoDevice::GAIN,				_cameras[1]->getParameter(VideoDevice::GAIN				));
		// _cameras[1]->showParameters();
	#endif		
	
	// ===============================================================
	// =                   R E A D Y   T O   R U N                   =
	// ===============================================================
	
	gk::AppSettings settings;
	settings.setGLVersion(3,3);
	// settings.setFullscreen();
	if(createWindow(800, 600, settings) < 0) closeWindow();
	
}





Core::~Core()
{
	for (Camera* camera : _cameras)
		if (camera)
			camera->close();
}

int Core::init()
{
	return 1;
}
  

int Core::quit()
{
	return 1;
}

int Core::draw()
{
	if (_cameras[0]) _cameras[0]->grabFrame();
	if (_cameras[1]) _cameras[1]->grabFrame();
	
	#ifndef DISABLE_RENDERING
	glClearColor(0.1, 0.1, 0.1, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	
	// IMAGE DE FOND
	// renderImg(cameras[0]->frame());
	
	// BORDURES DES QRCODES DETECTES
	// glMatrixMode(GL_PROJECTION);
	// glLoadIdentity();
	// gluOrtho2D(0.0, (GLdouble) cameras[0]->frame()->width, (GLdouble)cameras[0]->frame()->height, 0.0);
	// glMatrixMode(GL_MODELVIEW);
	// glLoadIdentity();
	
	// for (Symbol& symbol : symbols)
	// {
		// glColor3f(0.5f, 1.0f, 0.5f);
		// glBegin(GL_LINE_LOOP);
		// for (const cv::Matx21f& pt : symbol.pts)
			// glVertex2f(pt(0), pt(1));
		// glEnd();
	// }
	
	
	// static 				int						display	= 0;
	// static 				cv::Matx44f		view		= cv::Matx44f();
	// static 				cv::Matx44f		model		= cv::Matx44f();
	// static const	cv::Matx44f		toGL		= cv::Matx44f(1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0).t();
	// static const	cv::Matx44f		proj		= projectionFromIntrinsic(cameras[0]->A(), window_width, window_height, 1.0, 10000.0).t();
	
	// CALCUL
	// for (Symbol& symbol : symbols)
		// try {
			// LEGACY MODE FOR CUBE
			// try 				{	model = parseSymbolToModel(symbol.data, scale).t();																}
			// catch (...)	{ model = parseMatx33f_tr(symbol.data, cv::Matx31f(scale/2, scale/2, scale/2)).t(); }
			
			// symbol.extrinsic(cameras[0]->A(), cameras[0]->K(), Scanner::pattern(scale, subscale));
			// view =	viewFromSymbol(symbol.rvec, symbol.tvec).t();
			
			// glMatrixMode(GL_PROJECTION);
			// glLoadIdentity();
			// glMultMatrixf(&proj(0,0));
		
			// glMatrixMode(GL_MODELVIEW);
			// glLoadIdentity();
			// glMultMatrixf(&view(0, 0));
			// glMultMatrixf(&model(0, 0));
			// glMultMatrixf(&toGL(0, 0));
		
			// glColor3f(obj_c[0], obj_c[1], obj_c[2]);
			// glEnable(GL_LIGHTING);
			// glutSolidTeapot(obj);
			
	  // } catch (...) {
			// std::cout << "Invalid symbol, could not extract model informations from `" << symbol.data << "`" << std::endl;
		// }
	present();
	#endif
	
	#ifndef DISABLE_ENVMAP
	cv::Matx33f modelview = ModelView(*_cameras[0], *_scanner);
	_envmap.addFrame(*_cameras[0], modelview);
	_envmap.addFrame(*_cameras[1], modelview);
	#endif
	
	#ifndef DISABLE_VIEW
		#ifndef DISABLE_CAMERA_0
			cv::imshow("Front",								cv::Mat(_cameras[0]->frame()));
		#endif
		#ifndef DISABLE_CAMERA_1
			cv::imshow("Back",								cv::Mat(_cameras[1]->frame()));
		#endif
		#ifndef DISABLE_ENVMAP
			cv::imshow("Environnement Color", _envmap.color());
			// cv::imshow("Environnement Lumin", environnement.lumin());
		#endif
		cv::waitKey(3);
	#endif
	
	return 1;
}










void Core::processKeyboardEvent(SDL_KeyboardEvent& event)
{
	if (event.state == SDL_PRESSED)
		switch (event.keysym.sym)
		{
			case 'c':		_envmap.clear();	break;
			case 's':		
			{
				time_t now = time(0);
				std::stringstream path_color;
				path_color << "env/envmap_" << now << ".png";
				std::cout << "- Saving envmap to file '" << path_color.str() << "' ... " << std::flush;
				EnvMap::save(_envmap.color(), path_color.str());				
				std::cout << "done" << std::endl;
				break;
			}
			
			#define CONTROL VideoDevice::BRIGHTNESS
			case SDLK_F1:		_cameras[0]->setParameter(CONTROL, 0);		break;
			case SDLK_F2:		_cameras[0]->setParameter(CONTROL, 25);		break;
			case SDLK_F3:		_cameras[0]->setParameter(CONTROL, 51);		break;
			case SDLK_F4:		_cameras[0]->setParameter(CONTROL, 76);		break;
			case SDLK_F5:		_cameras[0]->setParameter(CONTROL, 102);	break;
			case SDLK_F6:		_cameras[0]->setParameter(CONTROL, 127);	break;
			case SDLK_F7:		_cameras[0]->setParameter(CONTROL, 153);	break;
			case SDLK_F8:		_cameras[0]->setParameter(CONTROL, 178);	break;
			case SDLK_F9:		_cameras[0]->setParameter(CONTROL, 204);	break;
			case SDLK_F10:	_cameras[0]->setParameter(CONTROL, 229);	break;
			case SDLK_F11:	_cameras[0]->setParameter(CONTROL, 255);	break;
			case 13:				_cameras[0]->resetParameter(CONTROL);			break;
			
			case SDLK_ESCAPE:
				closeWindow();
				break;
			default:
				printf("Key %d unmapped\n", (int) event.keysym.sym);
				break;
		}
}
void Core::processWindowResize(SDL_WindowEvent &event)
{
}
void Core::processMouseButtonEvent(SDL_MouseButtonEvent &event)
{
}
void Core::processMouseMotionEvent(SDL_MouseMotionEvent &event)
{
}	