#include "videodevice_opencv.hh"

#define  LOGHERE  std::cout << "[HERE] " << __FILE__ << " : " << __LINE__ << std::endl;

// ==================================================================================================
extern "C" VideoDevice* maker() { return new videodevices::OpenCV(); }
// ==================================================================================================

videodevices::OpenCV::OpenCV() :
	VideoCapture(),
	_iplframe(nullptr)
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



int videodevices::OpenCV::getParameter(control c)
{
	return (int) (255.0 * cv::VideoCapture::get(OpenCVCtrl(c)));
}
void videodevices::OpenCV::setParameter(control c, int v)
{
	cv::VideoCapture::set(OpenCVCtrl(c), (double) v / 255.0);
}
void videodevices::OpenCV::resetParameter(control c)
{
	std::cout << __FILE__ << " : " << __FUNCTION__ << " - Function not yet implemeted in this module" << std::endl;
}
void videodevices::OpenCV::showParameters()
{
	std::cout << __FILE__ << " : " << __FUNCTION__ << " - Function not yet implemeted in this module" << std::endl; 
	// std::cout << "CV_CAP_PROP_BRIGHTNESS :    " << cv::VideoCapture::get(CV_CAP_PROP_BRIGHTNESS)	<< std::endl
						// << "CV_CAP_PROP_CONTRAST :      " << cv::VideoCapture::get(CV_CAP_PROP_CONTRAST)		<< std::endl
						// << "CV_CAP_PROP_SATURATION :    " << cv::VideoCapture::get(CV_CAP_PROP_SATURATION)	<< std::endl
						// << "CV_CAP_PROP_HUE :           " << cv::VideoCapture::get(CV_CAP_PROP_HUE)					<< std::endl
						// << "CV_CAP_PROP_GAIN :          " << cv::VideoCapture::get(CV_CAP_PROP_GAIN)				<< std::endl
						// << "CV_CAP_PROP_EXPOSURE :      " << cv::VideoCapture::get(CV_CAP_PROP_EXPOSURE)		<< std::endl;
}

int videodevices::OpenCV::OpenCVCtrl(control c)
{
	switch(c)
	{
		case EXPOSURE :		return CV_CAP_PROP_EXPOSURE;
		case BRIGHTNESS:	return CV_CAP_PROP_BRIGHTNESS;
		case CONTRAST:		return CV_CAP_PROP_CONTRAST;
		case SATURATION:	return CV_CAP_PROP_SATURATION;
		case GAIN:				return CV_CAP_PROP_GAIN;
		default:					return -1;
	}
}

