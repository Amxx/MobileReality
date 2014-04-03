#ifndef LIGHT_HH
#define LIGHT_HH

#include "Geometry.h"

struct Light
{
	Light(unsigned int _id, const gk::Point& _position) : id(_id), position(_position) {}
	unsigned int		id;
	gk::Point				position;
};

std::vector<Light> makelights(unsigned int nb, float dist = 1.0);

#endif