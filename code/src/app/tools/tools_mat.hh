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

#ifndef TOOLS_MAT_HH
#define TOOLS_MAT_HH

#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include "Transform.h"


bool						isNull(const cv::Matx33f&);
bool						isNull(const cv::Matx44f&);

cv::Matx33f 		Matx44to33(const cv::Matx44f&);
cv::Matx44f 		Matx33to44(const cv::Matx33f&);

cv::Matx33f 		parseMatx33f(const std::string&);
cv::Matx44f 		parseMatx33f_tr(const std::string&, const cv::Matx31f& = cv::Matx31f());
cv::Matx44f 		parseMatx44f(const std::string&);

cv::Matx44f 		parseSymbolToModel(const std::string& str, float scale);
cv::Matx44f 		viewFromSymbol(const cv::Matx31f&, const cv::Matx31f&);

cv::Matx44f 		projectionFromIntrinsic(const cv::Matx33f& A, float w, float h, float near, float far);

gk::Transform		cv2gkit(const cv::Matx33f&);
gk::Transform		cv2gkit(const cv::Matx44f&);

#endif