#include "envmap.hh"

EnvMap::EnvMap(cv::Size s) :
	_size(s),
	_time(cv::Mat::zeros(_size, CV_32S)),
	_wght(cv::Mat::zeros(_size, CV_32S)),
	_color(cv::Mat::zeros(_size, CV_32FC3)),
	_lumin(cv::Mat::zeros(_size, CV_32F)),
	_current_time(0)
{
}




void EnvMap::addFrame(Camera& camera, cv::Matx33f& modelview)
{
	cv::Mat frame = cv::Mat(camera.frame());
	bool found = false;
	
	const	cv::Matx33f	toGL	= cv::Matx33f(1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, -1.0);
	const cv::Matx33f repro	= (camera.A() * toGL * camera.orientation() * modelview).inv();
	
	
	for (int u=0; u<frame.size().height; ++u)
		for (int v=0; v<frame.size().width; ++v)
		{
			cv::Matx31f pt = repro * cv::Matx31f(v, u, 1.0);					
			
			float phi		= atan2f(pt(0), pt(2));		 													// -Pi   .. Pi
			float theta	= atan2f(pt(1), sqrt(pt(0)*pt(0) + pt(2)*pt(2)));		// -Pi/2 .. Pi/2
			
			int		i = (0.5 + theta/3.14158265)	* _size.height;
			int		j = (0.5 + phi/6.28318531)		* _size.width;
			
			if (_time.at<int>(i,j) != _current_time)
			{
				_time.at<int>(i,j)				= _current_time;
				_wght.at<int>(i,j)				= 0;

				_color.at<cv::Vec3f>(i,j)	= cv::Vec3f(0.f, 0.f, 0.f);
				_lumin.at<float>(i,j)			= 0.f;
			}
			
			_color.at<cv::Vec3f>(i,j)	+= cv::Vec3f(frame.at<cv::Vec3b>(u, v)) / 255;
			
			// ====== TODO ======
			// _lumin.at<float>(i,j) 			+= camera.ldrToHdr(frame.at<cv::Vec3b>(u, v));
			
			_wght.at<int>(i,j)++;
		}
		
	for (int i=0; i<_size.height; ++i)
		for (int j=0; j<_size.width; ++j)
			if (_time.at<int>(i,j) == _current_time)
			{
				_color.at<cv::Vec3f>(i,j)	/= _wght.at<int>(i,j);
				_lumin.at<float>(i,j) 		/= _wght.at<int>(i,j);
			}
	
	_current_time++;
}



void EnvMap::clear()
{
	for (int i=0; i<_size.height; ++i)
		for (int j=0; j<_size.width; ++j)
		{
			_color.at<cv::Vec3f>(i,j)	= cv::Vec3f(0.f, 0.f, 0.f);	
			_lumin.at<float>(i,j)			= 0.f;	
		}
}





void EnvMap::save(const cv::Mat& img, const std::string& path)
{
	cv::Mat tone(img.size(), CV_8UC3);
	for (int i=0; i<img.size().height; ++i)
		for (int j=0; j<img.size().width; ++j)
			for (int k=0; k<3; ++k)
				tone.at<cv::Vec3b>(i,j)[k] = (uchar) (255.0 * img.at<cv::Vec3f>(i,j)[k]);
	imwrite(path, tone);
}



