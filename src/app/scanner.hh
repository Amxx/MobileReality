#ifndef SCANNER_HH
#define SCANNER_HH

#include <cstdio>
#include <dlfcn.h>
#include <vector>
#include <opencv2/core/core.hpp>


class Symbol;
class Scanner
{	
	public:
		virtual std::vector<Symbol>				scan(IplImage*) = 0;
		static	std::vector<cv::Matx31f>	pattern(float = 1.0f, float = 1.0f);
};


static Scanner* loadScanner(std::string path)
{
	typedef Scanner*(*loader_fun_t)();
	
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
