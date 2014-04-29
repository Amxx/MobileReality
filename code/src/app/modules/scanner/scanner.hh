#ifndef SCANNER_HH
#define SCANNER_HH

#include <cstdio>
#include <dlfcn.h>
#include <vector>
#include <opencv2/core/core.hpp>

class Symbol;
class Scanner
{	
	public:
		virtual std::vector<Symbol>				scan(IplImage*) = 0;
		static	std::vector<cv::Matx31f>	pattern(float = 1.0f, float = 1.0f);
};

#endif
