#ifndef ZBAR_SCANNER_HH
#define ZBAR_SCANNER_HH

#include "modules/module.hh"
#include "modules/scanner/scanner.hh"
#include "modules/scanner/symbol.hh"

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
