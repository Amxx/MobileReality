#ifndef ENVMAP_HH
#define ENVMAP_HH

#include "camera.hh"
#include "scanner.hh"

#include "tools_mat.hh"

class EnvMap
{
	private:
		cv::Size	_size;
		cv::Mat		_time;
		cv::Mat 	_wght;
		cv::Mat		_color;
		cv::Mat		_lumin;
	
		int 			_current_time;
	
	public:
		EnvMap(cv::Size = cv::Size(640, 480));
		void 						addFrame(Camera&, cv::Matx33f&);
		
		const cv::Mat&	color() const { return _color; }
		const cv::Mat&	lumin() const { return _lumin; }
		void 						clear();

		static void 		save(const cv::Mat&, const std::string&);
	
};



#endif