#ifndef CONFIGURATION_HH
#define CONFIGURATION_HH


#include <iostream>


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
		struct Material
		{
			Material() : kd(0.8f), ks(0.2f), ns(128.f) {}
			float				kd;
			float				ks;
			float				ns;
		};
		Object() : file("cube.obj"), scale(1.f), material() {}
		std::string		file;
		float					scale;
		Material			material;
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
			Envmap() : type(DYNAMIC), size(256,256), dual(false) {}
			Type				type;
			cv::Size		size;
			bool				dual;
		};
		struct Localisation
		{
			Localisation() : type(DYNAMIC) {};
			Type				type;
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
		General() : verbose(false), defaultValues(), envmap(), localisation(), rendering(), modules() {}
		bool					verbose;
		DefaultValues	defaultValues;
		Envmap				envmap;
		Localisation	localisation;
		Rendering			rendering;
		Modules				modules;
	};
};

class Configuration
{	
	public:
		// ============ Object ============
		Options::Object				object;
		// =========== Devices ============
		Options::Devices			devices;
		// =========== Markers ============
		Options::Markers			markers;
		// =========== General ============
		Options::General			general;

	public:
		Configuration() : object(), devices(), markers(), general() {}
		bool load(const std::string& path)
		{
			// ------------------------------------------------------------------------------------------------------------
			cv::FileStorage fs(path, cv::FileStorage::READ);
			if (!fs.isOpened()) return false;
			// ------------------------------------------------------------------------------------------------------------
			if (!fs["object"]["file"].empty())																					fs["object"]["file"]																								>> object.file;
			if (!fs["object"]["scale"].empty())																					fs["object"]["scale"]																								>> object.scale;
			if (!fs["object"]["material"]["kd"].empty())																fs["object"]["material"]["kd"]																			>> object.material.kd;
			if (!fs["object"]["material"]["ks"].empty())																fs["object"]["material"]["ks"]																			>> object.material.ks;
			if (!fs["object"]["material"]["ns"].empty())																fs["object"]["material"]["ns"]																			>> object.material.ns;			
			// ------------------------------------------------------------------------------------------------------------
			if (fs["devices"]["front"].empty() || !strcmp(((std::string) fs["devices"]["front"]["enable"]).c_str(), "off"))
				devices.front.enable				= false;
			else
			{
				devices.front.enable				= true;
				if (fs["devices"]["front"]["id"].isInt())																	fs["devices"]["front"]["id"]																				>> devices.front.id;
				else if	(fs["devices"]["front"]["id"].isString())													devices.front.id = strcmp(((std::string) fs["devices"]["front"]["id"]).c_str(), "auto")?-2:-1;
				else																																			devices.front.id = -2;
				if (!fs["devices"]["front"]["params"].empty())														fs["devices"]["front"]["params"]																		>> devices.front.params;
			}			
			if (fs["devices"]["back"].empty() || !strcmp(((std::string) 								fs["devices"]["back"]["enable"]).c_str(), "off"))
				devices.back.enable 				= false;
			else
			{
				devices.back.enable = true;
				if (fs["devices"]["back"]["id"].isInt())																	fs["devices"]["back"]["id"]																				>> devices.back.id;
				else if	(fs["devices"]["back"]["id"].isString())													devices.back.id = strcmp(((std::string) fs["devices"]["back"]["id"]).c_str(), "auto")?-2:-1;
				else																																			devices.back.id = -2;
				if (!fs["devices"]["back"]["params"].empty())															fs["devices"]["back"]["params"]																		>> devices.back.params;
			}
			// ------------------------------------------------------------------------------------------------------------
			if (!fs["markers"]["size"].empty())																					fs["markers"]["size"]																							>> markers.size;
			if (!fs["markers"]["scale"].empty())																				fs["markers"]["scale"]																						>> markers.scale;
			// ------------------------------------------------------------------------------------------------------------
			general.verbose								= strcmp(((std::string) fs["generalParameters"]["verbose"]).c_str(), "off");
			if (!fs["generalParameters"]["defaultValues"]["brightness"].empty())				fs["generalParameters"]["defaultValues"]["brightness"]						>> general.defaultValues.brightness;
			if (!fs["generalParameters"]["defaultValues"]["gain"].empty())							fs["generalParameters"]["defaultValues"]["gain"]									>> general.defaultValues.gain;
			if (!fs["generalParameters"]["defaultValues"]["persistency"].empty())				fs["generalParameters"]["defaultValues"]["persistency"]						>> general.defaultValues.persistency;
			general.envmap.type						= strcmp(((std::string) fs["generalParameters"]["envmap"]["type"]).c_str(), "debug")?Options::DYNAMIC:Options::DEBUG;
			general.envmap.dual						= !strcmp(((std::string) fs["generalParameters"]["envmap"]["dual"]).c_str(), "on");
			if (!fs["generalParameters"]["envmap"]["size"].empty())											fs["generalParameters"]["envmap"]["size"]													>> general.envmap.size;
			general.localisation.type			= strcmp(((std::string) fs["generalParameters"]["localisation"]["type"]).c_str(), "debug")?Options::DYNAMIC:Options::DEBUG;
			general.rendering.background	= strcmp(((std::string) fs["generalParameters"]["rendering"]["background"]).c_str(), "off");
			general.rendering.scene				= strcmp(((std::string) fs["generalParameters"]["rendering"]["scene"]).c_str(), "off");
			general.rendering.view				= strcmp(((std::string) fs["generalParameters"]["rendering"]["view"]).c_str(), "off");
			if (!fs["generalParameters"]["modules"]["scanner"].empty())									fs["generalParameters"]["modules"]["scanner"]											>> general.modules.scanner;
			if (!fs["generalParameters"]["modules"].empty())														fs["generalParameters"]["modules"]["video"]												>> general.modules.video;
			// ------------------------------------------------------------------------------------------------------------
			fs.release();
			return true;
		}
		
		bool save(const std::string& path) const
		{
			return false;
		/*
			cv::FileStorage fs(path, cv::FileStorage::WRITE);
			if (!fs.isOpened()) return false;
			fs	<< "object" << "{"
						<< "file"					<< object_file
						<< "scale"				<< object_scale
						<< "material" << "{"
							<< "kd"					<< object_material_kd
							<< "ks"					<< object_material_ks
							<< "ns"					<< object_material_ns
						<< "}"
					<< "}"
					<< "devices" << "{"
						<< "front" << "{"
							<< "id"					<< devices_front_id
							<< "params"			<< devices_front_params
						<< "}"
						<< "back" << "{"
							<< "id"					<< devices_back_id
							<< "params"			<< devices_back_params
						<< "}"
					<< "}"
					<< "markers" << "{"
						<< "size"					<< markers_size
						<< "scale"				<< markers_scale
					<< "}"
					<< "libs" << "{"
						<< "scanner"			<< libs_scanner
						<< "video"				<< libs_video
					<< "}";			
			fs.release();
			return true;
		*/
		}
		
		void view()
		{
			printf("┌─────────────────────────────────────────────────────────────────────────┐\n");
			printf("│               C O N F I G U R A T I O N   O V E R V I E W               │\n");
			printf("├─────────────────────────────────────────────────────────────────────────┤\n");
			printf("│ object.file                       : %-35s │\n",																object.file.c_str());
			printf("│ object.scale                      : %-35f │\n",																object.scale);
			printf("│ object.material.kd                : %-35f │\n",																object.material.kd);
			printf("│ object.material.ks                : %-35f │\n",																object.material.ks);
			printf("│ object.material.ns                : %-35f │\n",																object.material.ns);
			printf("│                                                                         │\n");
			printf("│ devices.front.enable              : %-35s │\n",																(devices.front.enable?std::string("on"):std::string("off")).c_str());
			printf("│ devices.front.id                  : %-35d │\n",																devices.front.id);
			printf("│ devices.front.params              : %-35s │\n",																devices.front.params.c_str());
			printf("│ devices.back.enable               : %-35s │\n",																(devices.back.enable?std::string("on"):std::string("off")).c_str());
			printf("│ devices.back.id                   : %-35d │\n",																evices.back.id);
			printf("│ devices.back.params               : %-35s │\n",																devices.back.params.c_str());
			printf("│                                                                         │\n");
			printf("│ markers.size                      : %-35f │\n",																markers.size);
			printf("│ markers.scale                     : %-35f │\n",																markers.scale);
			printf("│                                                                         │\n");
			printf("│ general.verbose                   : %-35s │\n",																(general.verbose?std::string("on"):std::string("off")).c_str());
			printf("│ general.defaultValues.brightness  : %-35d │\n",																general.defaultValues.brightness );
			printf("│ general.defaultValues.gain        : %-35d │\n",																general.defaultValues.gain);
			printf("│ general.defaultValues.persistency : %-35d │\n",																general.defaultValues.persistency);
			printf("│ general.envmap.type               : %-35d │\n",																general.envmap.type);
			printf("│ general.envmap.size               : [%4d x %-4d]                       │\n",	general.envmap.size.width, general.envmap.size.height);
			printf("│ general.envmap.dual               : %-35s │\n",																(general.envmap.dual?std::string("on"):std::string("off")).c_str());
			printf("│ general.localisation.type         : %-35d │\n",																(general.localisation.type?std::string("on"):std::string("off")).c_str());
			printf("│ general.rendering.background      : %-35s │\n",																(general.rendering.background?std::string("on"):std::string("off")).c_str());
			printf("│ general.rendering.scene           : %-35s │\n",																(general.rendering.scene?std::string("on"):std::string("off")).c_str());
			printf("│ general.rendering.view            : %-35s │\n",																(general.rendering.view?std::string("on"):std::string("off")).c_str());
			printf("│ general.modules.scanner           : %-35s │\n",																general.modules.scanner.c_str());
			printf("│ general.modules.video             : %-35s │\n",																general.modules.video.c_str());
			printf("└─────────────────────────────────────────────────────────────────────────┘\n");
		}
};
#endif