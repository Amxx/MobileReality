#ifndef OPTIONS_HH
#define OPTIONS_HH

#include <string>
#include <opencv2/core/core.hpp>

namespace Options
{
	enum Type
	{
		DEBUG = 0,
		DYNAMIC = 1,
	};
	struct Object
	{
		
		Object() : obj_file("cube.obj"), spheres_file(), scale(1.f) {}
		std::string		obj_file;
		std::string		spheres_file;
		float					scale;
	};
	struct Devices
	{
		struct Device
		{
			Device() : enable(true), id(-2), params("tmp_params.xml") {}
			bool				enable;
			int					id;
			std::string	params;
		};
		Devices()	: front(), back() {}
		Device				front;
		Device				back;
	};
	struct Markers
	{
		Markers() : size(1.f), scale(1.f) {}
		float					size;
		float					scale;
	};
	struct General
	{
		struct DefaultValues
		{
			DefaultValues() : brightness(-1), gain(-1), persistency(0) {}
			int						brightness;
			int						gain;
			int						persistency;
		};
		struct Envmap
		{
			Envmap() : build(true), size(256,256), dual(false), path("") {}
			bool				build;
			cv::Size		size;
			bool				dual;
			std::string	path;
		};
		struct Localisation
		{
			Localisation() : type(DYNAMIC), size(0.f) {};
			Type				type;
			float				size;
		};
		struct Rendering
		{
			Rendering() : background(true), scene(true), view(false) {}
			bool				background;
			bool				scene;
			bool				view;
		};
		struct Modules
		{
			Modules() : scanner("install/share/libscanner_zbar.so"), video("install/share/libvideodevice_uvc.so") {}
			std::string	scanner;
			std::string	video;
		};
		General() : verbose(0), defaultValues(), envmap(), localisation(), rendering(), modules() {}
		int						verbose;
		DefaultValues	defaultValues;
		Envmap				envmap;
		Localisation	localisation;
		Rendering			rendering;
		Modules				modules;
	};
};

#endif