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

#include "videodevice_opencv.hh"

// ==================================================================================================
extern "C" Module<VideoDevice>* maker() { return new videodevices::OpenCV(); }
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
	int c0 = OpenCVCtrl(c);
	return (c0==-1)?-1:(255.0 * cv::VideoCapture::get(c0));
}
void videodevices::OpenCV::setParameter(control c, int v)
{
	int c0 = OpenCVCtrl(c);
	if (c0==-1) return;
	cv::VideoCapture::set(c0, (double) v / 255.0);
}
void videodevices::OpenCV::resetParameter(control c)
{
	std::cout << __FILE__ << " : " << __FUNCTION__ << " - Function not yet implemeted in this module" << std::endl;
}
void videodevices::OpenCV::showParameters()
{
	std::cout << __FILE__ << " : " << __FUNCTION__ << " - Function not yet implemeted in this module" << std::endl; 
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
		case MODE :				
			fprintf(stderr, "[WARNING] Auto exposure not supported by opencv module\n");
			return -1;
		default:
			fprintf(stderr, "[WARNING] Unknown control command %d\n", c);
			return -1;
	}
}

