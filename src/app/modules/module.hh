#ifndef MODULE_HH
#define MODULE_HH

#include <dlfcn.h>
#include <string>

template<class T>
struct Module : public T
{
	static T* load(const std::string& path)
	{
		typedef Module<T>*(*loader_fun_t)();
		void *handler = dlopen(path.c_str(), RTLD_NOW);
		if (handler == nullptr)
		{
			fprintf(stderr, "[ERROR] could not open shared library %s\n", path.c_str());
			return nullptr;
		}
		loader_fun_t maker = (loader_fun_t) dlsym(handler, "maker");
		if (maker == nullptr)
		{
			fprintf(stderr, "[ERROR] could not find maker symbol for shared library %s\n", path.c_str());
			return nullptr;
		}
		return dynamic_cast<T*>((maker)());
	}		
};

#endif