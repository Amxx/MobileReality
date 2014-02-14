#include <iostream>
#include <iomanip>


#include <GL/glut.h>

#include "opencv_device.hh"
#include "zbar_scanner.hh"

#include "calibration.hh"
#include "mattools.hh"

#include "envmap.hh"



static videodevices::OpenCV 	video;
static scanners::ZBar					scanner;
static Calibration						params;

static GLuint                	cameraImageTextureID;
static GLuint                	windowID;
static int                    window_width      = 640;
static int                    window_height     = 480;

static float									scale = 5.0;
static float									obj = 6.0;
static float          			  obj_c[3]          = {0.482, 0.627, 0.357};
static float                 	light_spe[4]     	= {1.0f,  1.0f, 1.0f, 1.0f};
static float                 	light_dif[4]      = {1.0f,  1.0f, 1.0f, 1.0f};
static float                 	light_pos[4]      = {0.0f, 30.0f, 0.0f, 1.0f};




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
		case 27: // Q (Escape) - We're outta here.
			std::cout << std::endl;
			glutDestroyWindow(windowID);
			exit(1);
			break;
		default:
			printf ("KP: No action for %d.\n", key);
			break;
	}
}

void Draw()
{	
	assert(video.isopen());
	
	IplImage*									frame		= video.getImage();
	std::vector<ScannedInfos> symbols = scanner.scan(frame);
	
	// CLEAR
	glClearColor(0.1, 0.1, 0.1, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	
	// IMAGE DE FOND
	renderImg(frame);
	
	// BORDURES DES QRCODES DETECTES
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble) frame->width, (GLdouble)frame->height, 0.0);
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
	
	
	static 				int						display	= 0;
	static 				cv::Matx44f		view		= cv::Matx44f();
	static 				cv::Matx44f		model		= cv::Matx44f();
	static const	cv::Matx44f		toGL		= cv::Matx44f(1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0).t();
	static const	cv::Matx44f		proj		= projectionFromIntrinsic(params.A(), window_width, window_height, 1.0, 10000.0).t();
	
	// CALCUL
	for (ScannedInfos& symbol : symbols)
		try {
			symbol.extrinsic(scanner.pattern(scale), params.A(), params.K());
			model		= parseMatx33f_tr(symbol.data, cv::Matx31f(scale/2, scale/2, scale/2)).t();
			view		=	viewFromSymbol(symbol.rvec, symbol.tvec).t();
			display	= 10;
		} catch (...) {}
	
	if (display)
	{
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
		
		display--;
	}
	
	glutSwapBuffers();
}




	



int main(int argc, char* argv[])
{	
	// Opening WebCam
	while (!video.isopen())
	{
		int video_idx;
		std::cout << "Webcam identifier (-1 for auto) : ";
		std::cin >> video_idx;
		std::cout << "Oppening video device ... " << std::flush;
		video.open(video_idx);
		std::cout << "done (" << video.isopen() << ")" << std::endl;
	}
	
	// Calibrating WebCam
	if (!params.load("params/c910.xml"))
	{
		// params.calibrate(video, 30, 30, Calibration::CHESSBOARD, cv::Size(7,4)); 	// With chessboard
		params.calibrate(video, 100, 30);																					// With QRCode
		params.save("params/c910.xml");
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
	
	// glutMainLoop();
	
	
	bool run = true;
	EnvMap environnement(cv::Size(640, 480));
	while (run)
	{
		environnement.addFrame(video.getImage(), scanner, params);
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
	
	
	// Cleanup
	std::cout << "Closing video device ... " << std::flush;
	video.close();
	std::cout << "done (" << video.isopen() << ")" << std::endl;

	return 0;
}