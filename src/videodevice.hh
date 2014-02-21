#ifndef DEVICE_HH
#define DEVICE_HH

#include <opencv2/core/core.hpp>

class VideoDevice
{
	public:
		virtual bool				open(int = 0)		= 0;
		virtual void				close()					= 0;
		virtual bool 				isopen()				= 0;
		virtual void				grabFrame() 		= 0;
		virtual IplImage*		getFrame()			= 0;
		virtual IplImage*		frame()					= 0;
	
		static VideoDevice* load(std::string);
};


#endif