#ifndef DEVICE_HH
#define DEVICE_HH

#include <cstdio>
#include <dlfcn.h>
#include <opencv2/core/core.hpp>

class VideoDevice
{
	public:
		enum control
		{
			MODE,
			EXPOSURE,
			BRIGHTNESS,
			CONTRAST,
			SATURATION,
			GAIN
		};
		enum mode
		{
			AUTOEXPOSURE		= 0,
			MANUALEXPOSURE	= 1
		};
	
	public:		
		virtual bool				open(int = 0)								= 0;
		virtual void				close()											= 0;
		virtual bool 				isopen()										= 0;
		virtual void				grabFrame() 								= 0;
		virtual IplImage*		getFrame()									= 0;
		virtual IplImage*		frame()											= 0;	
	
		virtual int 				getParameter(control)				= 0;
		virtual void				setParameter(control, int)	= 0;
		virtual void				resetParameter(control)			= 0;
		virtual void				showParameters()						= 0;
};

#endif