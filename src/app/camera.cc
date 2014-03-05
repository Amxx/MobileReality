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
const cv::Matx33f&	Camera::orientation() const
{
	return _orientation;
}







void Camera::openAndCalibrate(std::string path)	// With chessboard
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

	// Calibrating WebCam
	load(path);
	if (!iscalibrated())
	{
		intrinsic_calibrate(1.0, 30, Calibration::CHESSBOARD, cv::Size(7,4));
		save(path);
	}
}

void Camera::openAndCalibrate(std::string path, Scanner& scanner) // With QRCode
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

	// Calibrating WebCam
	load(path);
	if (!iscalibrated())
	{
		intrinsic_calibrate(scanner, 1.0, 30);
		save(path);
	}
}