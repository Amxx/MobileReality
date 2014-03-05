#ifndef MATRICEOPERATION_HH
#define MATRICEOPERATION_HH

#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>


cv::Matx33f Matx44to33(const cv::Matx44f&);
cv::Matx44f Matx33to44(const cv::Matx33f&);


cv::Matx33f parseMatx33f(const std::string&);
cv::Matx44f parseMatx33f_tr(const std::string&, const cv::Matx31f& = cv::Matx31f());
cv::Matx44f parseMatx44f(const std::string&);

cv::Matx44f parseSymbolToModel(const std::string& str, float scale);
cv::Matx44f viewFromSymbol(const cv::Matx31f&, const cv::Matx31f&);

cv::Matx44f projectionFromIntrinsic(const cv::Matx33f& A, float w, float h, float near, float far);

#endif