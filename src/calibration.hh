#ifndef CALIBRATION_HH
#define CALIBRATION_HH

#include <iostream>
#include <sstream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "videodevice.hh"
#include "scanner.hh"

#include "zbar_scanner.hh"

class Calibration
{
	public:
		enum Pattern { QRCODE, CHESSBOARD, CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID };
		
	public:
		Calibration();
		Calibration(const std::string);	
		bool load(const std::string);
		bool save(const std::string) const;

		bool calibrate(VideoDevice&, double, unsigned int = 30, Pattern = QRCODE, cv::Size = cv::Size(2, 2));
	
		const cv::Matx33f&	A() { return _A; }
		const cv::Mat& 			K() { return _K; }
	
	private:
		cv::Matx33f		_A;
		cv::Mat				_K;
		cv::Size			_size;
		double				_rms;
};

#endif