#include "mattools.hh"

cv::Matx33f parseMatx33f(const std::string& str)
{
	cv::Matx33f m;
	if (sscanf(	str.c_str(),
							"[ %f, %f, %f; %f, %f, %f; %f, %f, %f ]",
							&m(0,0), &m(0,1), &m(0,2),
							&m(1,0), &m(1,1), &m(1,2),
							&m(2,0), &m(2,1), &m(2,2)) != 9)
		throw std::invalid_argument("Error parsing data as 3x3 matrice");
	return m;
}

cv::Matx44f parseMatx33f_tr(const std::string& str, const cv::Matx31f& t)
{
	cv::Matx44f m;
	if (sscanf(	str.c_str(),
							"[ %f, %f, %f; %f, %f, %f; %f, %f, %f ]",
							&m(0,0), &m(0,1), &m(0,2),
							&m(1,0), &m(1,1), &m(1,2),
							&m(2,0), &m(2,1), &m(2,2)) != 9)
		throw std::invalid_argument("Error parsing data as 3x3 matrice");
	m(0,3) = t(0); m(1,3) = t(1); m(2,3) = t(2);
	m(3,0) = 0; m(3,1) = 0; m(3,2) = 0; m(3,3) = 1;	
	return m;
}

cv::Matx44f parseMatx44f(const std::string& str)
{
	cv::Matx44f m;
	if (sscanf(	str.c_str(),
							"[ %f, %f, %f, %f; %f, %f, %f, %f; %f, %f, %f, %f; %f, %f, %f, %f ]",
							&m(0,0), &m(0,1), &m(0,2), &m(0,3),
							&m(1,0), &m(1,1), &m(1,2), &m(1,3),
							&m(2,0), &m(2,1), &m(2,2), &m(2,3),
							&m(3,0), &m(3,1), &m(3,2), &m(3,3)) != 16)
		throw std::invalid_argument("Error parsing data as 4x4 matrice");
	return m;
}

cv::Matx44f viewFromSymbol(const cv::Matx31f& rvec, const cv::Matx31f& t)
{
	cv::Matx33f r;
	cv::Rodrigues(rvec, r);
	
	cv::Matx44f cvToGl;
	cvToGl(0, 0) = +1.0f;
	cvToGl(1, 1) = -1.0f;
	cvToGl(2, 2) = -1.0f;
	cvToGl(3, 3) = +1.0f;
	
	cv::Matx44f view;
	view(0,0) = r(0, 0);	view(0,1) = r(0, 1);	view(0,2) = r(0, 2);	view(0,3) = t(0);
  view(1,0) = r(1, 0);	view(1,1) = r(1, 1);	view(1,2) = r(1, 2);	view(1,3) = t(1);
  view(2,0) = r(2, 0);	view(2,1) = r(2, 1);	view(2,2) = r(2, 2);	view(2,3) = t(2);
  view(3,0) = 0.0f;   	view(3,1) = 0.0f;   	view(3,2) = 0.0f; 		view(3,3) = 1.0f;	
		
	return cvToGl * view;
}

cv::Matx44f projectionFromIntrinsic(const cv::Matx33f& A, float w, float h, float near, float far)
{
  cv::Matx31f p		= A.inv() * cv::Matx31f(w/2, h/2, near);
  cv::Matx31f p1	= A.inv() * cv::Matx31f(  0, h/2, near);
  cv::Matx31f p2	= A.inv() * cv::Matx31f(  w, h/2, near);
  cv::Matx31f p3	= A.inv() * cv::Matx31f(w/2,   0, near);
  cv::Matx31f p4	= A.inv() * cv::Matx31f(w/2,   h, near);
	
	float left		= -cv::norm(p - p1);
	float right		= +cv::norm(p - p2);
	float bottom	= -cv::norm(p - p3);
	float top			= +cv::norm(p - p4);
	
	cv::Matx44f frustum;
	frustum(0,0) = 2 * near/(right - left);
	frustum(1,1) = 2 * near/(top - bottom);
	frustum(0,2) = (right + left) / (right - left);
	frustum(1,2) = (top + bottom) / (top - bottom);
	frustum(2,2) = - 			(far + near) / (far - near);
	frustum(3,2) = -1.f;
	frustum(2,3) = -2.0 * far * near / (far - near);
	
	return frustum;
}
