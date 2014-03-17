#ifndef ZBAR_SCANNER_HH
#define ZBAR_SCANNER_HH

#include "scanner.hh"
#include "symbol.hh"
#include "module.hh"

#include <opencv2/imgproc/imgproc.hpp>
#include <zbar.h>

namespace scanners
{
	class ZBar : public Module<Scanner>
	{
		private:
			zbar::ImageScanner	scanner;
		public:
			ZBar();
			std::vector<Symbol>	scan(IplImage*);
	};
};

#endif
