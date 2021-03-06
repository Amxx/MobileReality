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

#include "calibration.hh"

using namespace cv;
using namespace std;

static void calcBoardCornerPositions(Size size, float scale, vector<Matx31f>& corners, Calibration::Pattern pattern)
{
  corners.clear();
  switch(pattern)
  {
		case Calibration::CHESSBOARD:
		case Calibration::CIRCLES_GRID:
			for (int i=0; i<size.height; ++i)
        for (int j=0; j<size.width; ++j)
					corners.push_back(Matx31f(float(j*scale), float(i*scale), 0));
			break;
		
		case Calibration::ASYMMETRIC_CIRCLES_GRID:
			for (int i=0; i<size.height; ++i)
				for (int j=0; j<size.width; ++j)
					corners.push_back(Matx31f(float((2*j + i % 2)*scale), float(i*scale), 0));
			break;
		default:
			break;
	}
}


// ==================================== Methods ====================================

Calibration::Calibration() :
	_radius(-1),
	_ready(false)
{
}
Calibration::Calibration(const string& path) :
	_radius(-1),
	_ready(false)
{
	load(path);
}

bool Calibration::load(const string& path)
{
	FileStorage fs(path, FileStorage::READ);
	if (!fs.isOpened()) return false;
	Mat _tA;
	if (!fs["Camera_Matrix"].empty())							fs["Camera_Matrix"]						>> _tA;
	if (!fs["Distortion_Coefficients"].empty())		fs["Distortion_Coefficients"] >> _K;
	if (!fs["Image_Size"].empty())								fs["Image_Size"]							>> _size;
	if (!fs["Root_Mean_Square"].empty())					fs["Root_Mean_Square"] 				>> _rms;
	if (!fs["Fisheye_Radius"].empty())						fs["Fisheye_Radius"] 					>> _radius;
	if (!fs["Responce_Function"].empty())					fs["Responce_Function"]				>> _response;
	fs.release();
	_A = Matx33f(_tA);
	std::cout << "======> LOADING : " << path << std::endl;
	return (_ready = true);
}
bool Calibration::save(const string& path) const
{	
	FileStorage fs(path, FileStorage::WRITE);
	if (!fs.isOpened()) return false;
	Mat _tA(_A);
	fs << "Camera_Matrix"						<< _tA;
  fs << "Distortion_Coefficients" << _K;
  fs << "Image_Size"							<< _size;
	fs << "Root_Mean_Square"				<< _rms;
	fs << "Fisheye_Radius" 					<< _radius;
	fs << "Responce_Function"				<< _response;
  fs.release();
	return true;
}




/* ================================================================================================== *
 * =																		INSTRINSIC CALIBRATION																			= *
 * ================================================================================================== */

bool Calibration::intrinsic_calibrate(VideoDevice& video, Scanner& scanner, double scale, unsigned int nb)
{
	vector<vector<Matx21f>> imagePoints;
	Mat view;
	unsigned int frame_nb = 0;
	while(imagePoints.size() < nb && ++frame_nb)
	{
		IplImage* frame = video.getFrame();
		view = Mat(frame);
		vector<Matx21f> pointBuf;
		bool blink = false;
		
		for (Symbol& symbol : scanner.scan(frame))
		{
			drawChessboardCorners(view, Size(2, 2), Mat(symbol.pts), true);
			if ((blink = (frame_nb++ > 10)))	imagePoints.push_back(symbol.pts);
		}
		
		if (blink)
		{
			cout << "Calibration " << imagePoints.size() << " / " << nb << endl; 
			bitwise_not(view, view);
			frame_nb = 0;
		}
		imshow("Calibration", view);
		if (waitKey(30) == 27) return false;
	}
	destroyWindow("Calibration");
	_size = view.size();
	vector<Matx31f> pattern_pts = Scanner::pattern(scale);
	vector<vector<Matx31f>> objectPoints(imagePoints.size(), pattern_pts);	
	
	vector<Mat>	rvecs, tvecs;
	_rms = calibrateCamera(objectPoints, imagePoints, _size, _A, _K, rvecs, tvecs);
	return (_ready = true);

}

bool Calibration::intrinsic_calibrate(VideoDevice& video, double scale, unsigned int nb, Calibration::Pattern pattern, Size size)
{
	vector<vector<Matx21f>> imagePoints;
	Mat view;
	unsigned int frame_nb = 0;
	while(imagePoints.size() < nb && ++frame_nb)
	{
		IplImage* frame = video.getFrame();
		view = Mat(frame);
		vector<Matx21f> pointBuf;
		bool blink = false;
		
		switch (pattern)
		{
			case CHESSBOARD:
			{
				if (findChessboardCorners(view, size, pointBuf, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE))
				{
					Mat viewGray;
					cvtColor(view, viewGray, COLOR_BGR2GRAY);
					cornerSubPix(viewGray, pointBuf, Size(11,11), Size(-1,-1), TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1));
	
					drawChessboardCorners(view, size, Mat(pointBuf), true);
					if ((blink = (frame_nb > 10)))	imagePoints.push_back(pointBuf);
				}
				break;
			}
			
			case CIRCLES_GRID:
			{
				if(findCirclesGrid(view, size, pointBuf))
				{
					drawChessboardCorners(view, size, Mat(pointBuf), true);
					if ((blink = !(frame_nb++ % 10)))	imagePoints.push_back(pointBuf);
				}
				break;
			}
			
			case ASYMMETRIC_CIRCLES_GRID:
			{
				if (findCirclesGrid(view, size, pointBuf, CALIB_CB_ASYMMETRIC_GRID))
				{
					drawChessboardCorners(view, size, Mat(pointBuf), true);
					if ((blink = !(frame_nb++ % 10)))	imagePoints.push_back(pointBuf);
				}
				break;
			}
			default:
				break;
		}
		
		if (blink)
		{
			cout << "Calibration " << imagePoints.size() << " / " << nb << endl; 
			bitwise_not(view, view);
			frame_nb = 0;
		}
		imshow("Calibration", view);
		if (waitKey(30) == 27) return false;
	}
	destroyWindow("Calibration");
	_size = view.size();
	
	vector<Matx31f> pattern_pts;
	calcBoardCornerPositions(size, scale, pattern_pts, pattern);	
	vector<vector<Matx31f>> objectPoints(imagePoints.size(), pattern_pts);	
	
	vector<Mat>	rvecs, tvecs;
	_rms = calibrateCamera(objectPoints, imagePoints, _size, _A, _K, rvecs, tvecs);
	return (_ready = true);
}

/* ================================================================================================== *
 * =																				HDR CALIBRATION																					= *
 * ================================================================================================== */

// bool hdr_calibration(VideoDevice& video)
// {
	// ====== WILL BECOME ARGUMENTS ======
	// int nb_exposures;
	// int nb_points;
	// ===================================
		
	// std::vector<float>		input_time(nb_exposures);
	// std::vector<Mat>	input_frame(nb_exposures);
	// for (int i=0; i<nb_exposures; ++i)
	// {
		// float time = 1; // TODO SET EXPOSURE
		// camera.setExposure(exposure);
		// input_time[i]		= time;
		// input_frame[i]	= Mat(video.getFrame());
	// }
	
	// void* calibrate = createCalibrateDebevec();
	// calibrate->process(input_frame, _response, input_time);
	

// }













// float Calibration::ldrToHdr(const Vec3b& color, float exposure)
// {
	// Vec3f hdr;
	// hdr(0) = _response.at<float>((int) color(0))  - log(exposure);
	// hdr(1) = _response.at<float>((int) color(1))  - log(exposure);
	// hdr(2) = _response.at<float>((int) color(2))  - log(exposure);
	
	// TODO COMPUTE VALUE
	
	// return 0.f;
// }
