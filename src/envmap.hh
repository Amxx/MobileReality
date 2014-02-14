#ifndef ENVMAP_HH
#define ENVMAP_HH

#include "scanner.hh"
#include "calibration.hh"
#include "mattools.hh"

class EnvMap
{
	private:
		cv::Size	_size;
		cv::Mat		_time;
		cv::Mat 	_wght;
		cv::Mat		_data;
		int 			_current_time;
	
	public:
		EnvMap(cv::Size = cv::Size(640, 480));
		void 						addFrame(IplImage*, Scanner&, Calibration&);
		const cv::Mat&	data();
		void 						clear();
		void 						save(const std::string&);
	
};



#endif