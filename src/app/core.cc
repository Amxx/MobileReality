#include "core.hh"

// ============ MACROS ============
#define  LOGHERE  std::cout << "[HERE] " << __FILE__ << " : " << __LINE__ << std::endl;


Core::Core(int argc, char* argv[]) :
	gk::App(),
	
	_cameras(2),
	_envmap(cv::Size(640, 480)),
	
	_scale(6.0),
	_subscale(236.0/300.0),
	_objsize(6.0)
{
	
	gk::AppSettings settings;
	settings.setGLVersion(3,1);
	// settings.setFullscreen();
	if(createWindow(800, 600, settings) < 0) closeWindow();
	
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
	#endif
	#ifndef DISABLE_CAMERA_1
	videodevice = loadVideoDevice(_config("video"));
	if (videodevice == nullptr) exit(1);
	_cameras[1] = new Camera(videodevice,  cv::Matx33f(-1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, -1.0), _config("params-back"));
	if (_config("video-back-id").size()) _cameras[1]->open(atoi(_config("video-back-id").c_str()));
	_cameras[1]->openAndCalibrate(_config("params-back"), *_scanner);
	#endif		
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


cv::Matx33f ModelView(Camera& camera, Scanner& scanner)
{
	cv::Matx33f				view	= cv::Matx33f();
	cv::Matx33f				model	= cv::Matx33f();
	for (Symbol& symbol : scanner.scan(camera.frame()))
		try {
			try 				{	model = Matx44to33(parseSymbolToModel(symbol.data, 0.0));		}
			catch (...)	{ model = parseMatx33f(symbol.data); 													}
			symbol.extrinsic(camera.A(), camera.K());
			view = Matx44to33(viewFromSymbol(symbol.rvec, symbol.tvec));
		} catch (const std::exception& e) {
			std::cout << "Invalid symbol, could not extract model informations from `" << symbol.data << "`" << std::endl;
			std::cout << "Exception : " << e.what() << std::endl;
		}	
	return camera.orientation().inv() * view * model;
}


int Core::draw()
{
	if (_cameras[0]) _cameras[0]->grabFrame();
	if (_cameras[1]) _cameras[1]->grabFrame();
	
	#ifndef DISABLE_RENDERING
	// glClearColor(0.1, 0.1, 0.1, 1.0);
	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	
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
	// present();
	#endif
	
	#ifndef DISABLE_VIEWSCREEN
	// cv::Matx33f modelview = ModelView(*_cameras[0], *_scanner);
	// environnement.addFrame(*_cameras[0], modelview);
	// _envmap.addFrame(*_cameras[1], modelview);
	cv::imshow("Front",	cv::Mat(_cameras[0]->frame()));
	cv::imshow("Back",	cv::Mat(_cameras[1]->frame()));
	// cv::imshow("Environnement Color", _envmap.color());
	// cv::imshow("Environnement Lumin", environnement.lumin());
	cv::waitKey(30);
	#endif

	
	return 1;
}










void Core::processKeyboardEvent(SDL_KeyboardEvent& event)
{
	if (event.state == SDL_PRESSED)
		switch(event.keysym.sym)
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
			
			
			case SDLK_ESCAPE:
				closeWindow();
				break;
			default:
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