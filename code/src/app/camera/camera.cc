/*
Copyright or © or Copr. Hadrien Croubois (2014)

hadrien.croubois@ens-lyon.fr

This software is a computer program whose purpose is to render, in real
time, virtual objects usgin dynamically acquired environnement.

This software is governed by the CeCILL-B license under French law and
abiding by the rules of distribution of free software.  You can  use, 
modify and/ or redistribute the software under the terms of the CeCILL-B
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info". 

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability. 

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or 
data to be ensured and,  more generally, to use and operate it in the 
same conditions as regards security. 

The fact that you are presently reading this means that you have had
knowledge of the CeCILL-B license and that you accept its terms.
*/

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