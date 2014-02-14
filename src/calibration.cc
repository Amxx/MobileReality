#include "calibration.hh"

using namespace cv;
using namespace std;

static void calcBoardCornerPositions(Size size, float scale, vector<Matx31f>& corners, Calibration::Pattern pattern)
{
  corners.clear();
  switch(pattern)
  {
		case Calibration::QRCODE:
			corners = scanners::ZBar().pattern(scale);
			break;
		
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

Calibration::Calibration() {}
Calibration::Calibration(const string path) { load(path); }

bool Calibration::load(const string path)
{
	FileStorage fs(path, FileStorage::READ);
	if (!fs.isOpened()) return false;
	cv::Mat _tA;
	fs["Camera_Matrix"]						>> _tA;
	fs["Distortion_Coefficients"] >> _K;
	fs["Image_Size"] >> _size;
	fs["Root_Mean_Square"] 				>> _rms;
	fs.release();
	_A = cv::Matx33f(_tA);
	return true;
}
bool Calibration::save(const string path) const
{	
	FileStorage fs(path, FileStorage::WRITE);
	if (!fs.isOpened()) return false;
	cv::Mat _tA(_A);
	fs << "Camera_Matrix"						<< _tA;
  fs << "Distortion_Coefficients" << _K;
  fs << "Image_Size"							<< _size;
	fs << "Root_Mean_Square"				<< _rms;
  fs.release();
	return true;
}


bool Calibration::calibrate(VideoDevice& video, double scale, unsigned int nb, Calibration::Pattern pattern, Size size)
{
	vector<vector<Matx21f>> imagePoints;
	Mat view;
	unsigned int frame_nb = 0;
	while(imagePoints.size() < nb)
	{
		IplImage* frame = video.getImage();
		view = Mat(frame);
		vector<Matx21f> pointBuf;
		bool blink = false;
		
		switch (pattern)
		{
			case QRCODE:
			{
				scanners::ZBar zbar;
				for (ScannedInfos& infos : zbar.scan(frame))
				{
					drawChessboardCorners(view, size, Mat(infos.pts), true);
					if ((blink = !(frame_nb++ % 10)))	imagePoints.push_back(infos.pts);
				}
				break;
			}
			
			case CHESSBOARD:
			{
				if (findChessboardCorners(view, size, pointBuf, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE))
				{
					Mat viewGray;
					cvtColor(view, viewGray, COLOR_BGR2GRAY);
					cornerSubPix(viewGray, pointBuf, Size(11,11), Size(-1,-1), TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1));
	
					drawChessboardCorners(view, size, Mat(pointBuf), true);
					if ((blink = !(frame_nb++ % 10)))	imagePoints.push_back(pointBuf);
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
		}
		imshow("Calibration", view);
		if (cv::waitKey(30) == 27) return false;
	}
	destroyWindow("Calibration");
	_size = view.size();
	
	vector<Matx31f> pattern_pts;
	calcBoardCornerPositions(size, scale, pattern_pts, pattern);	
	vector<vector<Matx31f>> objectPoints(imagePoints.size(), pattern_pts);	
	
	vector<Mat>	rvecs, tvecs;
	_rms = calibrateCamera(objectPoints, imagePoints, _size, _A, _K, rvecs, tvecs);
	return true;
}































/*
float calibration(VideoDevice& video, Scanner& scanner, Mat& A, Mat& K, unsigned int view_nb, bool verbose)
{
	IplImage*															frame;
	Mat 															cvframe;
	vector<Point3f>							pattern({Point3f(+1., +1., 0.), Point3f(+1., -1., 0.), Point3f(-1., -1., 0.), Point3f(-1., +1., 0.)});
	vector<vector<Point3f>> objPts;
	vector<vector<Point2f>> imgPts;
	
	unsigned int frame_idx = 0;
	unsigned int view_cnt	= 0;
		
	while (view_cnt < view_nb)
	{
		frame = video.getImage();		
		cvframe = Mat(frame);
		
		if (!(frame_idx++ % 10))
		{
			for (ScannedInfos& symbol : scanner.scan(frame))
			{
				if (symbol.pts.size() != 4) continue;
				objPts.push_back(pattern);
				imgPts.push_back(symbol.pts);
				view_cnt++;
				
				for (int i = 0; i<4; ++i) line(cvframe, Point(symbol.pts[i].x, symbol.pts[i].y), Point(symbol.pts[(i+1)%4].x, symbol.pts[(i+1)%4].y), Scalar(0, 255, 0), 2, 8, 0);
				if (verbose) cout << "Calibration : " << view_cnt << "/" << view_nb << endl;
				
			}
		}
		imshow("Calibration", cvframe);
		if (waitKey(30) == 27) exit(0);
	}
		
	vector<Mat>	rvecs, tvecs;
	
	if (verbose)
		cout << "Computing camera parameters ... " << flush;
	float error = calibrateCamera(objPts, imgPts, Size(view_cnt, 4), A, K, rvecs, tvecs);
	if (verbose)
		cout << "done" << endl
							<< "Error : " << error << endl
							<< "A : "	<< cout << A << endl
							<< "K : "	<< cout << K << endl;

	destroyWindow("Calibration");

	return error;
}
*/