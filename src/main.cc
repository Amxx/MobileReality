#include <iostream>
#include <iomanip>

// ========= DEPENDENCIES =========
#include <GL/glut.h>

// ============= CORE =============
#include "camera.hh"
#include "configuration.hh"
#include "mattools.hh"
#include "envmap.hh"

// ============ MACROS ============
#define LOGHERE  std::cout << "[HERE] " << __FILE__ << " : " << __LINE__ << std::endl;





static Camera									*camera;
static VideoDevice						*device;
static Scanner								*scanner;


static GLuint                	cameraImageTextureID;
static GLuint                	windowID;
static int                    window_width      = 640;
static int                    window_height     = 480;

static float									scale = 3.0;
static float									obj = 6.0;
static float          			  obj_c[3]          = { 0.482f,  0.627f, 0.357f };
static float                 	light_spe[4]     	= { 1.000f,  1.000f, 1.000f, 1.000f };
static float                 	light_dif[4]      = { 1.000f,  1.000f, 1.000f, 1.000f };
static float                 	light_pos[4]      = { 0.000f, 30.000f, 0.000f, 1.000f };




void Reshape(int width, int height)
{ 	 
	window_width = width;
	window_height = height;
  glViewport(0, 0, width, height);
}
void renderImg(IplImage* img)
{
  glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)img->width, 0.0, (GLdouble)img->height);	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, cameraImageTextureID);
	if(img->nChannels == 3)
	  glTexImage2D( GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB, img->width, img->height, 0, GL_BGR, GL_UNSIGNED_BYTE, img->imageData);
	else if(img->nChannels == 4)
		glTexImage2D( GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, img->width, img->height, 0, GL_BGRA, GL_UNSIGNED_BYTE, img->imageData);
  glBegin(GL_QUADS);
		glTexCoord2i(0,          img->height);  glVertex2i(0,             0);
		glTexCoord2i(img->width, img->height);  glVertex2i(window_width,  0);
		glTexCoord2i(img->width, 0);            glVertex2i(window_width,  window_height);
		glTexCoord2i(0,          0);            glVertex2i(0,             window_height);
  glEnd();
	glDisable(GL_TEXTURE_RECTANGLE_ARB);  
	glEnable(GL_DEPTH_TEST);
};
void keyboard_event(unsigned char key, int x, int y)
{ 
	switch (key) 
	{
		case 27: // Escape - We're outta here.
			glutDestroyWindow(windowID);
			exit(1);
			break;
		default:
			//printf ("KP: No action for %d.\n", key);
			break;
	}
}

void Draw()
{	
	assert(camera->isready());

	camera->grabFrame();
	std::vector<ScannedInfos> symbols = scanner->scan(camera->frame());
	


	// CLEAR
	glClearColor(0.1, 0.1, 0.1, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	
	// IMAGE DE FOND
	renderImg(camera->frame());
	
	
	
	// BORDURES DES QRCODES DETECTES
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble) camera->frame()->width, (GLdouble)camera->frame()->height, 0.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	for (ScannedInfos& symbol : symbols)
	{
		glColor3f(0.5f, 1.0f, 0.5f);
		glBegin(GL_LINE_LOOP);
		for (const cv::Matx21f& pt : symbol.pts)
			glVertex2f(pt(0), pt(1));
		glEnd();
	}
	
	
	//static 				int						display	= 0;
	static 				cv::Matx44f		view		= cv::Matx44f();
	static 				cv::Matx44f		model		= cv::Matx44f();
	static const	cv::Matx44f		toGL		= cv::Matx44f(1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0).t();
	static const	cv::Matx44f		proj		= projectionFromIntrinsic(camera->A(), window_width, window_height, 1.0, 10000.0).t();
	
	// CALCUL
	for (ScannedInfos& symbol : symbols)
		try {
			// LEGACY MODE FOR CUBE
			try 				{	model = parseSymbolToModel(symbol.data, scale).t();																}
			catch (...)	{ model = parseMatx33f_tr(symbol.data, cv::Matx31f(scale/2, scale/2, scale/2)).t(); }
			
			symbol.extrinsic(	scanner->pattern(scale), camera->A(), camera->K() );
			view =	viewFromSymbol(symbol.rvec, symbol.tvec).t();
			
			// display	= 10;
		
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glMultMatrixf(&proj(0,0));
		
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glMultMatrixf(&view(0, 0));
			glMultMatrixf(&model(0, 0));
			glMultMatrixf(&toGL(0, 0));
		
			glColor3f(obj_c[0], obj_c[1], obj_c[2]);
			glEnable(GL_LIGHTING);
			glutSolidTeapot(obj);
			
	  } catch (...) {
			std::cout << "Invalid symbol, could not extract model informations from `" << symbol.data << "`" << std::endl;
		}
	
	// if (display)
	// {
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
		
		// display--;
	// }
	
	glutSwapBuffers();
}



	


int main(int argc, char* argv[])
{	
	
	Configuration config;
	config("-params")	= "params/default.xml";
	config("-video")	= "-1";
	
	for (int i=1; i<argc-1; i+=2)
		config(argv[i]) = argv[i+1];
	
	
	device	= VideoDevice::load("install/share/libvideodevice_opencv.so"); 
	scanner	= Scanner::load("install/share/libscanner_zbar.so");
	if (device == nullptr || scanner == nullptr) return 0;
	
	
	camera	= new Camera(device, config("-params"));
	
	
	if (config("-scale").size())
		scale = atof(config("-scale").c_str());
	if (config("-video").size())
		camera->open(atoi(config("-video").c_str()));
	
	
	// Opening WebCam
	while (!camera->isopen())
	{
		int video_idx;
		std::cout << "Webcam identifier (-1 for auto) : ";
		std::cin >> video_idx;
		std::cout << "Oppening video device ... " << std::flush;
		camera->open(video_idx);
		std::cout << "done (" << camera->isopen() << ")" << std::endl;
	}
	
	// Calibrating WebCam
	if (!camera->iscalibrated())
	{
	//camera->calibrate(scale, 3.0, Calibration::CHESSBOARD, cv::Size(7,4));	// With chessboard
		camera->calibrate(scale, 3.0);																					// With QRCode
		camera->save(config("-params"));
	}
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(window_width, window_height);
	windowID = glutCreateWindow("Visualisation");
	glutDisplayFunc(&Draw);
	glutIdleFunc(&Draw);
	glutKeyboardFunc(&keyboard_event);
	
	glClearDepth(1.0);
	glDepthFunc(GL_LESS); 
	
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glGenTextures(1, &cameraImageTextureID);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, cameraImageTextureID);
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glEnable(GL_LIGHT0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, light_spe);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100.0f);
  glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
  glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0f);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_dif);
  glEnable(GL_COLOR_MATERIAL);
	
	
	glutMainLoop();
	/*
	
	bool run = true;
	EnvMap environnement(cv::Size(640, 480));
	while (run)
	{
		camera->grabFrame();
		
		environnement.addFrame(*camera, *scanner);
		cv::imshow("Environnement", environnement.data());
		
		switch(cv::waitKey(30))
		{
			case 27:		run = false;						break;
			case 'c':		environnement.clear();	break;
			case 's':		
			{
				static unsigned int save_id = 0;
				std::stringstream path;
				path << "env/envmap_" << std::setfill ('0') << std::setw(6) << save_id++ << ".png";
				std::cout << "- Saving envmap to file '" << path.str() << "' ... " << std::flush;
				environnement.save(path.str());
				std::cout << "done" << std::endl;
				break;
			}
			default:														break;
		}
	}
	
	*/
	
	// Cleanup
	std::cout << "Closing video device ... " << std::flush;
	camera->close();
	std::cout << "done (" << camera->isopen() << ")" << std::endl;

	return 0;
}