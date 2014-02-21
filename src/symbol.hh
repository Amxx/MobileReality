#ifndef SYMBOL_HH
#define SYMBOL_HH

#include <vector>
#include <opencv2/core/core.hpp>
#include "opencv2/calib3d/calib3d.hpp"

struct Symbol
{
	Symbol();	
	void extrinsic(const std::vector<cv::Matx31f>&, const cv::Matx33f&, const cv::Mat&);
	std::vector<cv::Matx21f>	pts;
	cv::Matx31f								rvec;
	cv::Matx31f								tvec;
	std::string								data;
};

#endif