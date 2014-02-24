#ifndef SYMBOL_HH
#define SYMBOL_HH

#include <vector>
#include <opencv2/core/core.hpp>
#include "opencv2/calib3d/calib3d.hpp"

#include "scanner.hh"

class Symbol
{
	public:
		Symbol();	
		void extrinsic(const cv::Matx33f&, const cv::Mat&, const std::vector<cv::Matx31f>& = Scanner::pattern());
		
		std::vector<cv::Matx21f>	pts;
		cv::Matx31f								rvec;
		cv::Matx31f								tvec;
		std::string								data;
};

#endif