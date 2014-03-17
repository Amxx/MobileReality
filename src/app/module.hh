#ifndef MODULE_HH
#define MODULE_HH

#include "scanner.hh"
#include "videodevice.hh"


template<class T>
class ModuleT;

class Module
{
	// Classes authorized to create module
	friend ModuleT<Scanner>;
	friend ModuleT<VideoDevice>;	
	
	public:
		enum moduletype
		{
			NONE				= 0x0,
			SCANNER			= 0x1,
			VIDEODEVICE = 0x2,
			DYNAMIC			=	0x4
		};
		
	private: 
		Module(moduletype type = NONE) : _type(type) {}
			
	public:
		moduletype type() { return _type; }
		static Module* load(std::string path)
		{
			typedef Module*(*loader_fun_t)();
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
			return (maker)();
		}
		
	private:
		moduletype _type;
};

template<class T>
class ModuleT : public Module, public T
{
	public:
		static T* extract(Module* module) { return dynamic_cast<T*>((ModuleT<T>*) module); }
};



#endif