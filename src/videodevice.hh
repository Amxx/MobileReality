#ifndef DEVICE_HH
#define DEVICE_HH

#include <cstdio>
#include <dlfcn.h>
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
	
	
		virtual void				showParameters()	= 0;
		virtual void				lockParameters()	= 0;
};


static VideoDevice* loadVideoDevice(std::string path)
{
	typedef VideoDevice*(*loader_fun_t)();
	
	void *handler = dlopen(path.c_str(), RTLD_NOW);
	if (handler == nullptr)
	{
		printf("[ERROR] could not open shared library %s\n", path.c_str());
		return nullptr;
	}
	
	loader_fun_t maker = (loader_fun_t) dlsym(handler, "maker");
	if (maker == nullptr)
	{
		printf("[ERROR] could not find maker symbol for shared library %s\n", path.c_str());
		return nullptr;
	}
	
	return (maker)();
}


#endif