#ifndef SCANNER_HH
#define SCANNER_HH

#include <vector>
#include <opencv2/core/core.hpp>
#include "opencv2/calib3d/calib3d.hpp"

struct ScannedInfos
{
	ScannedInfos();	
	void extrinsic(const std::vector<cv::Matx31f>&, const cv::Matx33f&, const cv::Mat&);
	
	std::vector<cv::Matx21f>	pts;
	cv::Matx31f								rvec;
	cv::Matx31f								tvec;
	std::string								data;
};

class Scanner
{	
	public:
		virtual std::vector<ScannedInfos> scan(IplImage*) 					= 0;
		virtual	std::vector<cv::Matx31f>	pattern(float = 1)	const = 0;
	
};


#endif