#include "symbol.hh"

Symbol::Symbol()
{
}

void Symbol::extrinsic(const std::vector<cv::Matx31f>& pattern, const cv::Matx33f& A, const cv::Mat& K)
{
	cv::Mat R, T;
	if (pattern.size() == 4)
		cv::solvePnPRansac(pattern, pts, A, K, R, T, false, CV_P3P);		
	else
		cv::solvePnPRansac(pattern, pts, A, K, R, T);		
	rvec = cv::Matx31f(R);
	tvec = cv::Matx31f(T);
}

