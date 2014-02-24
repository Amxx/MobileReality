#include "videodevice_opencv.hh"


extern "C" {
	VideoDevice* maker() { return new videodevices::OpenCV(); }
}




videodevices::OpenCV::OpenCV() : VideoCapture()
{
}
videodevices::OpenCV::~OpenCV()
{
	close();
}
bool videodevices::OpenCV::open(int idx)
{
	return cv::VideoCapture::open(idx);
}
void videodevices::OpenCV::close()
{
	cv::VideoCapture::release();
}
bool videodevices::OpenCV::isopen()
{
	return cv::VideoCapture::isOpened();
}
void videodevices::OpenCV::grabFrame()
{
	cv::VideoCapture::grab();
}
IplImage* videodevices::OpenCV::getFrame()
{
	cv::VideoCapture::read(_cvframe);
	if (_iplframe) delete _iplframe;
	_iplframe = new IplImage(_cvframe);
	return _iplframe;
}
IplImage* videodevices::OpenCV::frame()
{
	cv::VideoCapture::retrieve(_cvframe);
	if (_iplframe) delete _iplframe;
	_iplframe = new IplImage(_cvframe);
	return _iplframe;
}




void videodevices::OpenCV::lockParameters()
{
	cv::VideoCapture::set(CV_CAP_PROP_BRIGHTNESS,			0.50);
	cv::VideoCapture::set(CV_CAP_PROP_CONTRAST,				0.00);
	cv::VideoCapture::set(CV_CAP_PROP_SATURATION,			0.45);
	cv::VideoCapture::set(CV_CAP_PROP_HUE,						0.50);
	cv::VideoCapture::set(CV_CAP_PROP_GAIN,						0.50);
	cv::VideoCapture::set(CV_CAP_PROP_EXPOSURE,				0.00);
}
void videodevices::OpenCV::showParameters()
{
	std::cout << "CV_CAP_PROP_BRIGHTNESS :    " << cv::VideoCapture::get(CV_CAP_PROP_BRIGHTNESS)		<< std::endl
						<< "CV_CAP_PROP_CONTRAST :      " << cv::VideoCapture::get(CV_CAP_PROP_CONTRAST)			<< std::endl
						<< "CV_CAP_PROP_SATURATION :    " << cv::VideoCapture::get(CV_CAP_PROP_SATURATION)		<< std::endl
						<< "CV_CAP_PROP_HUE :           " << cv::VideoCapture::get(CV_CAP_PROP_HUE)						<< std::endl
						<< "CV_CAP_PROP_GAIN :          " << cv::VideoCapture::get(CV_CAP_PROP_GAIN)					<< std::endl
						<< "CV_CAP_PROP_EXPOSURE :      " << cv::VideoCapture::get(CV_CAP_PROP_EXPOSURE)			<< std::endl;
}


