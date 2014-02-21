#include "videodevice_opencv.hh"


extern "C" {
	VideoDevice* maker() { return new videodevices::OpenCV(); }
}




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
void videodevices::OpenCV::grabFrame()
{
	if (capture) cvGrabFrame(capture);
}
IplImage* videodevices::OpenCV::getFrame()
{
	if (capture) return cvQueryFrame(capture);
	return nullptr;
}
IplImage* videodevices::OpenCV::frame()
{
	if (capture) return cvRetrieveFrame(capture);
	return nullptr;
}



