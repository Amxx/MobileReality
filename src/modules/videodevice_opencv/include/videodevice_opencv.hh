#ifndef OPENCV_DEVICE_HH
#define OPENCV_DEVICE_HH


#include <opencv2/highgui/highgui.hpp>
#include "videodevice.hh"


namespace videodevices
{
	class OpenCV : public VideoDevice
	{
		private:
			CvCapture* capture;
		
		public:
			OpenCV();
			~OpenCV();

			bool			open(int idx = 0);
			void			close();
			bool			isopen();
			void			grabFrame();
			IplImage*	getFrame();
			IplImage*	frame();
	};
};

#endif