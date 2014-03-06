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
	
	// =================================
	// =      P A R A M E T E R S      =
	// =================================
			unsigned int width	= 640;
			unsigned int height	= 480;
			unsigned int fRate	= 30;
	// =================================
	unsigned int format	= V4L2_PIX_FMT_YUYV;

	// =================================
	// =        TODO : CASE -1         =
	// =================================
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
   	goto error;
	}

	if (uvcGrab(_videoIn) < 0 || !_videoIn->framebuffer)
	{
		fprintf( stderr, "WARNING: could not get values from camera UVC %d.\n", idx);
   	goto error;
	}
	
	_frame = cvCreateImage(cvSize(_videoIn->width, _videoIn->height), IPL_DEPTH_8U, 3);
	if (!_frame)
	{
		fprintf( stderr, "ERROR: camera %d : cvCreateImage failed.\n", idx);
   	goto error;
	}
	
	return true;
	
	error:
		delete _videoIn;
		_videoIn = NULL;
		return false;
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
		#define CLIP(min, X, max) ( (X) > max ? max : (X) < min ? min : X)
		
		float y0 = (float) _videoIn->framebuffer[4*i + 0];
		float cb = (float) _videoIn->framebuffer[4*i + 1];
		float y1 = (float) _videoIn->framebuffer[4*i + 2];
		float cr = (float) _videoIn->framebuffer[4*i + 3];
		
		static float YCbCr2RGB[9] = {	+1.067, +0.000, +1.596, \
																	+1.067, -0.392, -0.813, \
																	+1.067, +2.017, +0.000 };
																	
		float r0 = YCbCr2RGB[0]*(y0-16) + YCbCr2RGB[1]*(cb-128) + YCbCr2RGB[2]*(cr-128);
		float g0 = YCbCr2RGB[3]*(y0-16) + YCbCr2RGB[4]*(cb-128) + YCbCr2RGB[5]*(cr-128);
		float b0 = YCbCr2RGB[6]*(y0-16) + YCbCr2RGB[7]*(cb-128) + YCbCr2RGB[8]*(cr-128);
		float r1 = YCbCr2RGB[0]*(y1-16) + YCbCr2RGB[2]*(cb-128) + YCbCr2RGB[2]*(cr-128);
		float g1 = YCbCr2RGB[3]*(y1-16) + YCbCr2RGB[4]*(cb-128) + YCbCr2RGB[5]*(cr-128);
		float b1 = YCbCr2RGB[6]*(y1-16) + YCbCr2RGB[7]*(cb-128) + YCbCr2RGB[8]*(cr-128);
		
		_frame->imageData[6*i + 0] = (char) CLIP(0, b0, 255); // BLUE
		_frame->imageData[6*i + 1] = (char) CLIP(0, g0, 255); // GREEN
		_frame->imageData[6*i + 2] = (char) CLIP(0, r0, 255); // RED
		_frame->imageData[6*i + 3] = (char) CLIP(0, b1, 255); // BLUE
		_frame->imageData[6*i + 4] = (char) CLIP(0, g1, 255); // GREEN
		_frame->imageData[6*i + 5] = (char) CLIP(0, r1, 255); // RED
		
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
			

