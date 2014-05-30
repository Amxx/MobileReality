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

#ifndef OPENCV_DEVICE_HH
#define OPENCV_DEVICE_HH

#include "modules/module.hh"
#include "modules/video/videodevice.hh"

#include <iostream>
#include <opencv2/highgui/highgui.hpp>


namespace videodevices
{
	class OpenCV : public Module<VideoDevice>, private cv::VideoCapture
	{
		private:
			cv::Mat 	_cvframe;
			IplImage*	_iplframe;
		
		public:
			OpenCV();
			~OpenCV();

			bool			open(int idx = 0);
			void			close();
			bool			isopen();
			void			grabFrame();
			IplImage*	getFrame();
			IplImage*	frame();
				
			int 			getParameter(control);
			void			setParameter(control, int);
			void			resetParameter(control);
			void			showParameters();
		
		private:
			int 			OpenCVCtrl(control c);
	};
};

#endif