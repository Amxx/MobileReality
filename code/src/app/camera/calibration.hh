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

#ifndef CALIBRATION_HH
#define CALIBRATION_HH

#include <iostream>
#include <sstream>

#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/photo/photo.hpp>

#include "../modules/video/videodevice.hh"
#include "../modules/scanner/scanner.hh"
#include "../modules/scanner/symbol.hh"

class Calibration
{
	public:
		enum Pattern { CHESSBOARD, CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID };
		
	public:
		Calibration();
		Calibration(const std::string&);	
		bool load(const std::string&);
		bool save(const std::string&) 			const;

	
		bool intrinsic_calibrate(VideoDevice&, Scanner&, double, unsigned int = 30);
		bool intrinsic_calibrate(VideoDevice&, double, unsigned int = 30, Pattern = CHESSBOARD, cv::Size = cv::Size(7, 4));
	
		const cv::Matx33f&	A()							const	{ return _A; 			}
		const cv::Mat& 			K() 						const	{ return _K; 			}
		const cv::Size&			size()					const { return _size;		}
		const double&				rms()						const { return _rms;		}
		const double&				radius()				const { return _radius;	}
		
		bool								iscalibrated()	const	{ return _ready; 	}
		
		// float ldrToHdr(const cv::Vec3b&, float = 1.0); // TODO
		
	private:
		cv::Matx33f				_A;
		cv::Mat						_K;
		cv::Size					_size;
		double						_rms;
		double						_radius;
		bool							_ready;
	
		cv::Mat						_response;
};

#endif