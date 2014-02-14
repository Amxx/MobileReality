#include "zbar_scanner.hh"
#include <iostream>

scanners::ZBar::ZBar():
	scanner()
{
	scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);
}

std::vector<ScannedInfos> scanners::ZBar::scan(IplImage* img)
{
	cv::Mat frame(img), frame_grayscale;
	cvtColor(frame, frame_grayscale, CV_BGR2GRAY);		
	int width = frame_grayscale.cols;
	int height = frame_grayscale.rows;
	uchar *raw = (uchar *)(frame_grayscale.data);
	
	zbar::Image zframe(width, height, "Y800", raw, width * height);
	scanner.scan(zframe);
		
	std::vector<ScannedInfos> results;
	
	for (zbar::Image::SymbolIterator symbol = zframe.symbol_begin(); symbol != zframe.symbol_end(); ++symbol)
	{
		ScannedInfos symbolinfos;
		for (int i = 0; i<symbol->get_location_size(); ++i)
			symbolinfos.pts.push_back(cv::Matx21f(symbol->get_location_x(i), symbol->get_location_y(i)));
		symbolinfos.data = symbol->get_data();	
		results.push_back(symbolinfos);
	}
	return results;
}

std::vector<cv::Matx31f> scanners::ZBar::pattern(float scale) const
{
	std::vector<cv::Matx31f> results(4);
	results[0] = cv::Matx31f(0.0, 	0.0, 		0.0);
	results[1] = cv::Matx31f(0.0, 	scale,	0.0);
	results[2] = cv::Matx31f(scale,	scale,	0.0);
	results[3] = cv::Matx31f(scale,	0.0,		0.0);
	return results;
}