#include <dlfcn.h>

#include "scanner.hh"

std::vector<cv::Matx31f> Scanner::pattern(float scale)
{
	std::vector<cv::Matx31f> results(4);
	results[0] = cv::Matx31f(0.0, 	0.0, 		0.0);
	results[1] = cv::Matx31f(0.0, 	scale,	0.0);
	results[2] = cv::Matx31f(scale,	scale,	0.0);
	results[3] = cv::Matx31f(scale,	0.0,		0.0);
	return results;
}