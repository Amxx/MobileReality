#ifndef CONFIGURATION_HH
#define CONFIGURATION_HH


#include <iostream>


#include <string>
#include <opencv2/core/core.hpp>

class Configuration
{	
	public:
		// ============ Object ============
		std::string		object_file;
		float					object_scale;
		float					object_material_kd;
		float					object_material_ks;
		float					object_material_ns;
		// =========== Devices ============
		int						devices_front_id;
		std::string		devices_front_params;
		int						devices_back_id;
		std::string		devices_back_params;
		// =========== Markers ============
		float					markers_size;
		float					markers_scale;
		// ============= Libs =============
		std::string		libs_scanner;
		std::string		libs_video;
	
	public:			
		Configuration()
		{
			object_file						= "cube.obj";
			object_scale					=	1;
			object_material_kd		= 0.8;
			object_material_ks		= 0.2;
			object_material_ns		= 64;
			devices_front_id			=	-1;
			devices_front_params	= "params/default-front.xml";
			devices_back_id				= -1;
			devices_back_params		= "params/default-back.xml";
			markers_size					= 1;
			markers_scale					= 1;
			libs_scanner					= "install/share/libscanner_zbar.so";
			libs_video						= "install/share/libvideodevice_uvc.so";
		}
	
	
		bool load(const std::string& path)
		{
			cv::FileStorage fs(path, cv::FileStorage::READ);
			if (!fs.isOpened()) return false;
			if (!fs["object"]["file"].empty())							fs["object"]["file"]							>> object_file;
			if (!fs["object"]["scale"].empty())							fs["object"]["scale"]							>> object_scale;
			if (!fs["object"]["material"]["kd"].empty())		fs["object"]["material"]["kd"]		>> object_material_kd;
			if (!fs["object"]["material"]["ks"].empty())		fs["object"]["material"]["ks"]		>> object_material_ks;
			if (!fs["object"]["material"]["ns"].empty())		fs["object"]["material"]["ns"]		>> object_material_ns;			
			if (!fs["devices"]["front"]["id"].empty())			fs["devices"]["front"]["id"]			>> devices_front_id;
			if (!fs["devices"]["front"]["params"].empty())	fs["devices"]["front"]["params"]	>> devices_front_params;
			if (!fs["devices"]["back"]["id"].empty())				fs["devices"]["back"]["id"]				>> devices_back_id;
			if (!fs["devices"]["back"]["params"].empty())		fs["devices"]["back"]["params"]		>> devices_back_params;
			if (!fs["markers"]["size"].empty())							fs["markers"]["size"]							>> markers_size;
			if (!fs["markers"]["scale"].empty())						fs["markers"]["scale"]						>> markers_scale;
			if (!fs["libs"]["scanner"].empty())							fs["libs"]["scanner"]							>> libs_scanner;
			if (!fs["libs"]["video"].empty())								fs["libs"]["video"]								>> libs_video;
			fs.release();
			return true;
		}
		
		bool save(const std::string& path) const
		{
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
		}		
		
		void view() const
		{
			std::cout << "obj_file             : " << object_file						<< std::endl;
			std::cout << "obj_scale            : " << object_scale					<< std::endl;
			std::cout << "devices_front_id     : " << devices_front_id			<< std::endl;
			std::cout << "devices_front_params : " << devices_front_params	<< std::endl;
			std::cout << "devices_back_id      : " << devices_back_id				<< std::endl;
			std::cout << "devices_back_params  : " << devices_back_params		<< std::endl;
			std::cout << "markers_size         : " << markers_size					<< std::endl;
			std::cout << "markers_scale        : " << markers_scale					<< std::endl;
			std::cout << "libs_scanner         : " << libs_scanner					<< std::endl;
			std::cout << "libs_video           : " << libs_video						<< std::endl;
		}
};


#endif