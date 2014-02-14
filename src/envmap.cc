#include "envmap.hh"

EnvMap::EnvMap(cv::Size s) :
	_size(s),
	_time(cv::Mat::zeros(_size, CV_32S)),
	_wght(cv::Mat::zeros(_size, CV_32S)),
	_data(cv::Mat::zeros(_size, CV_32FC3)),
	_current_time(0)
{
}


void EnvMap::addFrame(IplImage* frame, Scanner& scanner, Calibration& params)
{
	cv::Mat view = cv::Mat(frame);
	bool found = false;

	// Preparing transformation
	cv::Matx33f cvToGl;
	cvToGl(0, 0) = -1.0f;
	cvToGl(1, 1) = +1.0f;
	cvToGl(2, 2) = -1.0f;

	// Getting rotation and model matrice
	cv::Matx33f R;
	cv::Matx33f M;

	for (ScannedInfos& symbol : scanner.scan(frame))
		try {
			symbol.extrinsic(scanner.pattern(1.0), params.A(), params.K());
			M = parseMatx33f(symbol.data);
			cv::Rodrigues(symbol.rvec, R);
			found = true;
			break;
		} catch(...) {}
	
	// Reprojection matrice
	cv::Matx33f reproject = (params.A() * cvToGl * R * M).inv();
	
		
	if (found)
	{
		for (int u=0; u<view.size().height; ++u)
			for (int v=0; v<view.size().width; ++v)
			{
				int x = 										 v;
				int y = view.size().height - u;
				cv::Matx31f pt = reproject * cv::Matx31f(x, y, 1.0);					
				float p = atan2f(pt(0), - pt(2)); 												// -Pi   .. Pi
				float t = atan2f(pt(1), sqrt(pt(0)*pt(0) + pt(2)*pt(2))); // -Pi/2 .. Pi/2
				int		i = (0.5 - t/3.14158265) * _size.height;
				int		j = (0.5 - p/6.28318531) * _size.width;
				if (_time.at<int>(i,j) != _current_time)
				{
					_time.at<int>(i,j)				= _current_time;
					_data.at<cv::Vec3f>(i,j)	= cv::Vec3f(0.f, 0.f, 0.f);
					_wght.at<int>(i,j)				= 0;
				}
				_data.at<cv::Vec3f>(i,j) += cv::Vec3f(view.at<cv::Vec3b>(u, v)) / 255;
				_wght.at<int>(i,j)++;
			}
		for (int i=0; i<_size.height; ++i)
			for (int j=0; j<_size.width; ++j)
				if (_time.at<int>(i,j) == _current_time)
					_data.at<cv::Vec3f>(i,j) /= _wght.at<int>(i,j);
	}
	_current_time++;
}

const cv::Mat& EnvMap::data()
{
	return _data;
}

void EnvMap::clear()
{
	for (int i=0; i<_size.height; ++i)
		for (int j=0; j<_size.width; ++j)
			_data.at<cv::Vec3f>(i,j) = cv::Vec3f(0.0, 0.0, 0.0);	
}

void EnvMap::save(const std::string& path)
{
	cv::Mat tone(_size, CV_8UC3);
	for (int i=0; i<_size.height; ++i)
		for (int j=0; j<_size.width; ++j)
			for (int k=0; k<3; ++k)
				tone.at<cv::Vec3b>(i,j)[k] = (uchar) (255.0 * _data.at<cv::Vec3f>(i,j)[k]);
	imwrite(path, tone);
}



