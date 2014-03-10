#ifndef TOOLS_HH
#define TOOLS_HH

#include "camera.hh"
#include "scanner.hh"
#include "tools_mat.hh"

cv::Matx33f ModelView(Camera&, Scanner&);
cv::Matx33f ModelView(Camera&, std::vector<Symbol>&);

#endif