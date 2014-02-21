#include "videodevice.hh"

#include <cstdio>
#include <dlfcn.h>

VideoDevice* VideoDevice::load(std::string path)
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
