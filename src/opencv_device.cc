#include "opencv_device.hh"

videodevices::OpenCV::OpenCV() : capture(nullptr)
{
}
videodevices::OpenCV::~OpenCV()
{
	close();
}
bool videodevices::OpenCV::open(int idx)
{
	capture = cvCaptureFromCAM(idx);
	return capture;
}

void videodevices::OpenCV::close()
{
	if (capture) cvReleaseCapture(&capture);
	capture = nullptr;
}
bool videodevices::OpenCV::isopen()
{
	return capture;
}

IplImage* videodevices::OpenCV::getImage()
{
	if (capture) return cvQueryFrame(capture);
	return nullptr;
}
