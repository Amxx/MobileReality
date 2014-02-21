#include "scanner.hh"

#include <dlfcn.h>

ScannedInfos::ScannedInfos()
{
}

void ScannedInfos::extrinsic(const std::vector<cv::Matx31f>& pattern, const cv::Matx33f& A, const cv::Mat& K)
{
	cv::Mat R, T;
	if (pattern.size() == 4)
		cv::solvePnPRansac(pattern, pts, A, K, R, T, false, CV_P3P);		
	else
		cv::solvePnPRansac(pattern, pts, A, K, R, T);		
	rvec = cv::Matx31f(R);
	tvec = cv::Matx31f(T);
}


// SCANNER STATIC

Scanner* Scanner::load(std::string path)
{
	typedef Scanner*(*loader_fun_t)();
	
	void *handler = dlopen(path.c_str(), RTLD_NOW);
	if (handler == nullptr)
	{
		printf("[ERROR] could not open shared library %s\n", path.c_str());
		return nullptr;
	}

	loader_fun_t maker = (loader_fun_t) dlsym(handler, "maker");
	if (maker == nullptr)
	{
		printf("[ERROR] could not find maker symbol for shared library %s\n", path.c_str());
		return nullptr;
	}
		
	return (maker)();
}

std::vector<cv::Matx31f> Scanner::pattern(float scale)
{
	std::vector<cv::Matx31f> results(4);
	results[0] = cv::Matx31f(0.0, 	0.0, 		0.0);
	results[1] = cv::Matx31f(0.0, 	scale,	0.0);
	results[2] = cv::Matx31f(scale,	scale,	0.0);
	results[3] = cv::Matx31f(scale,	0.0,		0.0);
	return results;
}