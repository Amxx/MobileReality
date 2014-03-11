#ifndef ENVMAP_HH
#define ENVMAP_HH


#include "Image.h"
#include <ImageArray.h>

#include "camera.hh"
#include "scanner.hh"

#include "tools_mat.hh"




class Face : public cv::Mat
{
	public:
		Face(cv::Size s);
	public:
		cv::Mat		_time;
		cv::Mat 	_wght;
};


class EnvMap : public std::vector<Face*>
{
	private:
		cv::Size	_size;
		int 			_current_time;
	
	public:
		EnvMap(cv::Size = cv::Size(512, 512));
	
		void 						addFrame(Camera&, cv::Matx33f);
		const cv::Mat&	color(int id = 0) const { return *((*this)[id]); }
		void 						clear();
		static void 		save(const cv::Mat&, const std::string&);
	
		
		
};



#endif