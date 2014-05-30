/*
Copyright or © or Copr. Hadrien Croubois (2014)

hadrien.croubois@ens-lyon.fr

This software is a computer program whose purpose is to render, in real
time, virtual objects usgin dynamically acquired environnement.

This software is governed by the CeCILL-B license under French law and
abiding by the rules of distribution of free software.  You can  use, 
modify and/ or redistribute the software under the terms of the CeCILL-B
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info". 

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability. 

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or 
data to be ensured and,  more generally, to use and operate it in the 
same conditions as regards security. 

The fact that you are presently reading this means that you have had
knowledge of the CeCILL-B license and that you accept its terms.
*/

#include "videodevice_uvc.hh"

// ==================================================================================================
extern "C" Module<VideoDevice>* maker() { return new videodevices::UVC(); }
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
	char videodevice[16];
	sprintf(videodevice, "/dev/video%d", idx);
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
		 fprintf(stderr, "getRawDataPtr failed.\n");
		 return;
	}
	
	for(int i=0; i<_videoIn->height*_videoIn->width/2; ++i)
	{
		#define CLIP(min, X, max) ( (X) > max ? max : (X) < min ? min : X)
		
		float y0 = (float) CLIP(16, _videoIn->framebuffer[4*i + 0], 235);	y0 = (255/219)*(y0-16);
		float cb = (float) CLIP(16, _videoIn->framebuffer[4*i + 1], 240);	cb = (127/112)*(cb-128);
		float y1 = (float) CLIP(16, _videoIn->framebuffer[4*i + 2], 235);	y1 = (255/219)*(y1-16);
		float cr = (float) CLIP(16, _videoIn->framebuffer[4*i + 3], 240);	cr = (127/112)*(cr-128);

		static float YCbCr2RGB[9] = {	+1.000, +0.000, +1.402, \
																	+1.000, -0.344, -0.714, \
																	+1.000, +1.772, +0.000 };
																	
		float r0 = YCbCr2RGB[0]*y0 + YCbCr2RGB[1]*cb + YCbCr2RGB[2]*cr;
		float g0 = YCbCr2RGB[3]*y0 + YCbCr2RGB[4]*cb + YCbCr2RGB[5]*cr;
		float b0 = YCbCr2RGB[6]*y0 + YCbCr2RGB[7]*cb + YCbCr2RGB[8]*cr;
		
		float r1 = YCbCr2RGB[0]*y1 + YCbCr2RGB[2]*cb + YCbCr2RGB[2]*cr;
		float g1 = YCbCr2RGB[3]*y1 + YCbCr2RGB[4]*cb + YCbCr2RGB[5]*cr;
		float b1 = YCbCr2RGB[6]*y1 + YCbCr2RGB[7]*cb + YCbCr2RGB[8]*cr;
		
		_frame->imageData[6*i + 0] = (char) CLIP(0, b0, 255);
		_frame->imageData[6*i + 1] = (char) CLIP(0, g0, 255);
		_frame->imageData[6*i + 2] = (char) CLIP(0, r0, 255);
		_frame->imageData[6*i + 3] = (char) CLIP(0, b1, 255);
		_frame->imageData[6*i + 4] = (char) CLIP(0, g1, 255);
		_frame->imageData[6*i + 5] = (char) CLIP(0, r1, 255);
		
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
	v4l2ResetControl(_videoIn, V4L2Ctrl(c));
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
		case MODE	:				return V4L2_CID_EXPOSURE_AUTO;
		default:
			fprintf(stderr, "[WARNING] Unknown control command %d\n", c);
			return -1;
	}
}
			

