#ifndef CAMERA_HH
#define CAMERA_HH

#include "videodevice.hh"
#include "calibration.hh"
#include "scanner.hh"

class Camera : public VideoDevice, public Calibration
{
	
	private:
		VideoDevice* 	_device;
		cv::Matx33f		_orientation;
	
	
	
	
	public:
		Camera(VideoDevice* = nullptr, std::string = "");
		Camera(VideoDevice*, cv::Matx33f, std::string = "");
		
		// VideoDevice Methods
		bool				open(int idx = 0)								{ _device->open(idx); 							}
		void				close()													{ _device->close();									}
		bool 				isopen() 												{ return _device->isopen();					}
		void 				grabFrame()											{ _device->grabFrame();							}
		IplImage*		getFrame()											{	return _device->getFrame();				}
		IplImage*		frame() 												{	return _device->frame();					}
		int 				getParameter(control c)					{ return _device->getParameter(c);	}
		void				setParameter(control c, int v)	{ _device->setParameter(c, v);			}
		void				resetParameter(control c)				{	_device->resetParameter(c);				}
		void				showParameters()								{	_device->showParameters();				}
		
		// Calibration Methods
		bool intrinsic_calibrate(Scanner&, double, unsigned int = 30);
		bool intrinsic_calibrate(double, unsigned int = 30, Pattern = CHESSBOARD, cv::Size = cv::Size(2, 2));

		
		// Camera Methods
		bool 								isready()						const;		
		const cv::Matx33f&	orientation()				const;

		void								openAndCalibrate(std::string path);
		void								openAndCalibrate(std::string path, Scanner&);
	
};


#endif