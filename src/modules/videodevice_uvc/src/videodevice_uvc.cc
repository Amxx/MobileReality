#include "videodevice_uvc.hh"

#define  LOGHERE  std::cout << "[HERE] " << __FILE__ << " : " << __LINE__ << std::endl;

// ==================================================================================================
extern "C" VideoDevice* maker() { return new videodevices::UVC(); }
// ==================================================================================================

videodevices::UVC::UVC() : 
	_videoIn(nullptr)
{
}
videodevices::UVC::~UVC()
{
	close();
}



bool videodevices::UVC::open(int idx)
{
	if (isopen()) return false;
	
	unsigned int width	= 640;
	unsigned int height	= 480;
	unsigned int fRate	= 30;
	unsigned int format	= V4L2_PIX_FMT_YUYV;
	
	char videodevice[32];
	sprintf(videodevice, "/dev/video%d", idx);

	FILE* fd;
	fd = fopen(videodevice, "r+");
	
	if (!fd)
	{
		fprintf(stderr, "WARNING: Can't open device %s.\n", videodevice);
		return false;
	}
	fclose(fd);

	_videoIn = new struct vdIn;
	if(init_videoIn(_videoIn, (char*) videodevice, width, height, fRate, format, 1, NULL) < 0 )
	{
		fprintf( stderr, "WARNING: camera UVC %d not detected.\n", idx);
   		delete _videoIn;
		_videoIn = NULL;
		return false;
	}

	if (uvcGrab(_videoIn) < 0 || !_videoIn->framebuffer)
	{
		 fprintf( stderr, "WARNING: could not get values from camera UVC %d.\n", idx);
   		delete _videoIn;
		_videoIn = NULL;
		 return false;
	}
	
	_frame = cvCreateImage(cvSize(_videoIn->width, _videoIn->height), IPL_DEPTH_8U, 3);
	if (!_frame)
	{
		fprintf( stderr, "ERROR: camera %d : cvCreateImage failed.\n", idx);
		return -1;
	}
	
	return true;
}

void videodevices::UVC::close()
{
	if (isopen())
	{
		close_v4l2(_videoIn);
		delete _videoIn;
		_videoIn = nullptr;
	}
}
bool videodevices::UVC::isopen()
{
	return _videoIn != nullptr;
}

void videodevices::UVC::grabFrame()
{
	if (uvcGrab(_videoIn) < 0 || !_videoIn->framebuffer)
	{
		 fprintf( stderr, "getRawDataPtr failed.\n");
		 return;
	}
	
	for(int i=0; i<_videoIn->height*_videoIn->width/2; ++i)
	{
		#define CLIP(X) ( (X) > 255 ? 255 : (X) < 0 ? 0 : X)
		
		int y0 = _videoIn->framebuffer[4*i + 0];
		int cb = _videoIn->framebuffer[4*i + 1];
		int y1 = _videoIn->framebuffer[4*i + 2];
		int cr = _videoIn->framebuffer[4*i + 3];
		
		float r0 = 1.067*(y0 - 16) + 1.596*(cr - 128);
		float g0 = 1.067*(y0 - 16) - 0.813*(cr - 128) - 0.392*(cb - 128);
		float b0 = 1.067*(y0 - 16) + 2.017*(cb - 128);
		
		float r1 = 1.067*(y1 - 16) + 1.596*(cr - 128);
		float g1 = 1.067*(y1 - 16) - 0.813*(cr - 128) - 0.392*(cb - 128);
		float b1 = 1.067*(y1 - 16) + 2.017*(cb - 128);
			
		_frame->imageData[6*i + 0] = (char) CLIP(b0); // BLUE
		_frame->imageData[6*i + 1] = (char) CLIP(g0); // GREEN
		_frame->imageData[6*i + 2] = (char) CLIP(r0); // RED
		_frame->imageData[6*i + 3] = (char) CLIP(b1); // BLUE
		_frame->imageData[6*i + 4] = (char) CLIP(g1); // GREEN
		_frame->imageData[6*i + 5] = (char) CLIP(r1); // RED
		
	}
}



IplImage* videodevices::UVC::getFrame()
{
	grabFrame();
	return _frame;
}
IplImage* videodevices::UVC::frame()
{
	return _frame;
	
}


int videodevices::UVC::getParameter(control c)
{
	return v4l2GetControl(_videoIn, V4L2Ctrl(c));
}
void videodevices::UVC::setParameter(control c, int v)
{
	v4l2SetControl(_videoIn, V4L2Ctrl(c), v);
}
void videodevices::UVC::resetParameter(control c)
{
	v4l2ResetControl(_videoIn,  V4L2Ctrl(c));
}
void videodevices::UVC::showParameters()
{
	enum_controls(_videoIn->fd);
}





int videodevices::UVC::V4L2Ctrl(control c)
{
	switch(c)
	{
		case EXPOSURE :		return V4L2_CID_EXPOSURE;
		case BRIGHTNESS:	return V4L2_CID_BRIGHTNESS;
		case CONTRAST:		return V4L2_CID_CONTRAST;
		case SATURATION:	return V4L2_CID_SATURATION;
		case GAIN:				return V4L2_CID_GAIN;
		default:					return -1;
	}
}
			

