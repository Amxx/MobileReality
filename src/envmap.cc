#include "envmap.hh"

EnvMap::EnvMap(cv::Size s) :
	_size(s),
	_time(cv::Mat::zeros(_size, CV_32S)),
	_wght(cv::Mat::zeros(_size, CV_32S)),
	_data(cv::Mat::zeros(_size, CV_32FC3)),
	_current_time(0)
{
}








void EnvMap::addFrame(Camera& camera, cv::Matx33f& modelview)
{
	cv::Mat frame = cv::Mat(camera.frame());
	bool found = false;
	
	const	cv::Matx33f	toGL	= cv::Matx33f(1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, -1.0);
	cv::Matx33f reproject = ( camera.A() * toGL * camera.orientation() * modelview).inv();
	
	for (int u=0; u<frame.size().height; ++u)
		for (int v=0; v<frame.size().width; ++v)
		{
			cv::Matx31f pt = reproject * cv::Matx31f(v, u, 1.0);					
			
			float phi		= atan2f(pt(0), pt(2));		 													// -Pi   .. Pi
			float theta	= atan2f(pt(1), sqrt(pt(0)*pt(0) + pt(2)*pt(2)));		// -Pi/2 .. Pi/2
			
			int		i = (0.5 + theta/3.14158265)	* _size.height;
			int		j = (0.5 + phi/6.28318531)		* _size.width;
			
			if (_time.at<int>(i,j) != _current_time)
			{
				_time.at<int>(i,j)				= _current_time;
				_data.at<cv::Vec3f>(i,j)	= cv::Vec3f(0.f, 0.f, 0.f);
				_wght.at<int>(i,j)				= 0;
			}
			_data.at<cv::Vec3f>(i,j) += cv::Vec3f(frame.at<cv::Vec3b>(u, v)) / 255;
			_wght.at<int>(i,j)++;
		}
	for (int i=0; i<_size.height; ++i)
		for (int j=0; j<_size.width; ++j)
			if (_time.at<int>(i,j) == _current_time)
				_data.at<cv::Vec3f>(i,j) /= _wght.at<int>(i,j);
	
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



