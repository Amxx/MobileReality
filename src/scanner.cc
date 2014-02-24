#include <dlfcn.h>

#include "scanner.hh"

std::vector<cv::Matx31f> Scanner::pattern(float scale, float p)
{
	float a = scale * (1-p) / 2;
	float b = scale * (1+p) / 2;

	std::vector<cv::Matx31f> results(4);
	results[0] = cv::Matx31f(a, a, 0.0);
	results[1] = cv::Matx31f(a, b, 0.0);
	results[2] = cv::Matx31f(b,	b, 0.0);
	results[3] = cv::Matx31f(b,	a, 0.0);
	return results;
}
