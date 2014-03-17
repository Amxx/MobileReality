#include "envmap.hh"

Face::Face(cv::Size s) :
	cv::Mat(cv::Mat::zeros(s, CV_32FC3)),
	_time(cv::Mat::zeros(s, CV_32S)),
	_wght(cv::Mat::zeros(s, CV_32S))
{
};

EnvMap::EnvMap(cv::Size s) :
	std::vector<Face*>(),
	_size(s),
	_current_time(0)
{
	std::vector<Face*>::push_back(new Face(s));
	std::vector<Face*>::push_back(new Face(s));
	std::vector<Face*>::push_back(new Face(s));
	std::vector<Face*>::push_back(new Face(s));
	std::vector<Face*>::push_back(new Face(s));
	std::vector<Face*>::push_back(new Face(s));
}

void EnvMap::addFrame(Camera& camera, cv::Matx44f modelview)
{
	cv::Mat frame = cv::Mat(camera.frame());
	
	static const cv::Matx44f toGL(1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
	cv::Matx33f transform = Matx44to33(modelview.inv() * camera.orientation().inv() * toGL) * camera.A().inv();
	
	for (int u=0; u<frame.size().height; ++u)
		for (int v=0; v<frame.size().width; ++v)
		{
			if (camera.radius() != -1)
			{
				float ru = u-frame.size().height/2;
				float rv = v-frame.size().width/2;
				if (ru*ru+rv*rv>camera.radius()*camera.radius()) continue;
			}
			
			cv::Matx31f pt = transform * cv::Matx31f(v, u, 1.0);					
			
			float n = sqrt(pt(0)*pt(0) + pt(1)*pt(1) + pt(2)*pt(2));			
			pt(0) /= n;			pt(1) /= n;			pt(2) /= n;
			
			int i;
			float sc, tc, m = 0.;
			if (+pt(0) > m) { sc = - pt(2); tc = - pt(1); m = +pt(0); i = 0; }
			if (-pt(0) > m) { sc = + pt(2); tc = - pt(1); m = -pt(0); i = 1; }
			if (+pt(1) > m) { sc = + pt(0); tc = + pt(2); m = +pt(1); i = 2; }
			if (-pt(1) > m) { sc = + pt(0); tc = - pt(2); m = -pt(1); i = 3; }
			if (+pt(2) > m) { sc = + pt(0); tc = - pt(1); m = +pt(2); i = 4; }
			if (-pt(2) > m) { sc = - pt(0); tc = - pt(1); m = -pt(2); i = 5; }
			
			int x = 0.5 * (tc / m + 1.0) * _size.width;
			int y = 0.5 * (sc / m + 1.0) * _size.height;
			
			Face*	face = (*this)[i];
			if (face->_time.at<int>(x,y) != _current_time)
			{
				face->_time.at<int>(x,y)				= _current_time;
				face->_wght.at<int>(x,y)				= 0;
				face->at<cv::Vec3f>(x,y)				= cv::Vec3f(0.f, 0.f, 0.f);
			}
			face->_wght.at<int>(x,y)++;
			face->at<cv::Vec3f>(x,y)					+= cv::Vec3f(frame.at<cv::Vec3b>(u, v)) / 255;
			
		}
		
	for (Face* face : *this)
		for (int x=0; x<_size.height; ++x)
			for (int y=0; y<_size.width; ++y)
				if (face->_time.at<int>(x,y) == _current_time)
					face->at<cv::Vec3f>(x,y) /= face->_wght.at<int>(x,y);
	
	_current_time++;
}



void EnvMap::clear()
{
	for (Face* face : *this)
		for (int x=0; x<_size.height; ++x)
			for (int y=0; y<_size.width; ++y)
				face->at<cv::Vec3f>(x,y)				= cv::Vec3f(0.f, 0.f, 0.f);
}





void EnvMap::save(const std::string& path)
{
	cv::Mat tone(cv::Size(_size.width*3, _size.height*4), CV_8UC3);
	for (int i=0; i<6; ++i)
	{
		int dx, dy;
		switch (i)
		{
			case 0: dx = 1 * _size.height; dy = 2 * _size.width; break;
			case 1: dx = 1 * _size.height; dy = 0 * _size.width; break;
			case 2: dx = 1 * _size.height; dy = 1 * _size.width; break;
			case 3: dx = 3 * _size.height; dy = 1 * _size.width; break;
			case 4: dx = 2 * _size.height; dy = 1 * _size.width; break;
			case 5: dx = 0 * _size.height; dy = 1 * _size.width; break;		
		}
		Face*	face = (*this)[i];
		for (int x=0; x<_size.height; ++x)
			for (int y=0; y<_size.width; ++y)
				for (int k=0; k<3; ++k)
					tone.at<cv::Vec3b>(x+dx,y+dy)[k] = (uchar) (255.0 * face->at<cv::Vec3f>(x,y)[k]);
	}
	imwrite(path, tone);
}



