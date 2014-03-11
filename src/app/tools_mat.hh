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