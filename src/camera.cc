#include "camera.hh"



Camera::Camera(VideoDevice* device , std::string path) :
	Calibration(path),
	_device(device),
	_orientation(cv::Matx33f(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0))
{
}

Camera::Camera(VideoDevice* device , cv::Matx33f orientation, std::string path) :
	Calibration(path),
	_device(device),
	_orientation(orientation)
{
}

bool Camera::isready() const
{
	return _device->isopen() && Calibration::iscalibrated();
}











bool Camera::calibrate(double scale, unsigned int nb, Calibration::Pattern pattern, cv::Size size)
{
	return Calibration::calibrate(*this, scale, nb, pattern, size);
}
		
