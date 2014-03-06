#ifndef OPENCV_DEVICE_HH
#define OPENCV_DEVICE_HH

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include "videodevice.hh"


namespace videodevices
{
	class OpenCV : public VideoDevice, private cv::VideoCapture
	{
		private:
			cv::Mat 	_cvframe;
			IplImage*	_iplframe;
		
		public:
			OpenCV();
			~OpenCV();

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
			int 			OpenCVCtrl(control c);
	};
};

#endif