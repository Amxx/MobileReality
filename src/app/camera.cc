#include "camera.hh"

Camera::Camera(VideoDevice* device , std::string path) :
	Calibration(),
	_device(device),
	_orientation(cv::Matx44f(	1.0, 0.0, 0.0, 0.0,
														0.0, 1.0, 0.0, 0.0,
														0.0, 0.0, 1.0, 0.0,
														0.0, 0.0, 0.0, 1.0 ))
{
	if (!path.empty()) load(path);
}
Camera::Camera(VideoDevice* device , cv::Matx44f orientation, std::string path) :
	Calibration(),
	_device(device),
	_orientation(orientation)
{
	if (!path.empty()) load(path);
}



// Calibration Methods
bool Camera::intrinsic_calibrate(Scanner& scanner, double scale, unsigned int nb)
{
	return Calibration::intrinsic_calibrate(*this, scanner, scale, nb);
}
bool Camera::intrinsic_calibrate(double scale, unsigned int nb, Calibration::Pattern pattern, cv::Size size)
{
	return Calibration::intrinsic_calibrate(*this, scale, nb, pattern, size);
}



// Camera Methods
bool Camera::isready() const
{
	return _device->isopen() && Calibration::iscalibrated();
}
const cv::Matx44f&	Camera::orientation() const
{
	return _orientation;
}











void Camera::UIOpen()
{
	// Opening WebCam
	while (!isopen())
	{
		int video_idx;
		std::cout << "Webcam identifier (-1 for auto) : ";
		std::cin >> video_idx;
		std::cout << "Oppening video device ... " << std::flush;
		open(video_idx);
		std::cout << "done (" << isopen() << ")" << std::endl;
	}
}
void Camera::UICalibrate(std::string path) // With chessboard
{
	load(path);
	if (!iscalibrated())
	{
		intrinsic_calibrate(1.0, 30, Calibration::CHESSBOARD, cv::Size(7,4));
		save(path);
	}
}
void Camera::UICalibrate(std::string path, Scanner& scanner) // With QRCode
{
	load(path);
	if (!iscalibrated())
	{
		intrinsic_calibrate(scanner, 1.0, 30);
		save(path);
	}
}