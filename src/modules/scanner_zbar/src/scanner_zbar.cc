#include "scanner_zbar.hh"


extern "C" {
	Scanner* maker() { return new scanners::ZBar(); }
}




scanners::ZBar::ZBar():
	scanner()
{
	scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);
}
std::vector<Symbol> scanners::ZBar::scan(IplImage* img)
{
	cv::Mat frame(img), frame_grayscale;
	cvtColor(frame, frame_grayscale, CV_BGR2GRAY);		
	
	int width		= frame_grayscale.cols;
	int height	= frame_grayscale.rows;
	uchar *raw	= (uchar *)(frame_grayscale.data);
	
	zbar::Image zframe(width, height, "Y800", raw, width * height);
	scanner.scan(zframe);
		
	std::vector<Symbol> results;
	
	for (zbar::Image::SymbolIterator symbol = zframe.symbol_begin(); symbol != zframe.symbol_end(); ++symbol)
	{
		Symbol symbolinfos;
		for (int i = 0; i<symbol->get_location_size(); ++i)
			symbolinfos.pts.push_back(cv::Matx21f(symbol->get_location_x(i), symbol->get_location_y(i)));
		symbolinfos.data = symbol->get_data();	
		results.push_back(symbolinfos);
	}
	return results;
}