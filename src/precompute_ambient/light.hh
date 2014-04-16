#ifndef LIGHT_HH
#define LIGHT_HH

#include "Geometry.h"

struct Light : public gk::Point
{
	Light()                   : gk::Point()  {}
	Light(const gk::Point& p) : gk::Point(p) {}

	static Light Random();
};

std::vector<Light> makelights(unsigned int nb, float dist = 1.0);

#endif