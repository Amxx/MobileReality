#ifndef OPENCV_DEVICE_HH
#define OPENCV_DEVICE_HH

#include "module.hh"
#include "videodevice.hh"

#include <iostream>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "uvc-release.h"


namespace videodevices
{
	class UVC : public ModuleT<VideoDevice>
	{
		private:
			struct vdIn*	_videoIn;
			IplImage*			_frame;
		
		public:
			UVC();
			~UVC();

			bool			open(int idx = 0);
			void			close();
			bool			isopen();
			void			grabFrame();
			IplImage*	getFrame();
			IplImage*	frame();
		
			int 			getParameter(control);
			void			setParameter(control, int);
			void			resetParameter(control);
			void			showParameters();
		
		private:
			static int V4L2Ctrl(VideoDevice::control);
	};
};

#endif