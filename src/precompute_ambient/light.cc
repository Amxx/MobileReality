#include "light.hh"

Light Light::Random()
{
	float u = 2.f * drand48() - 1.0f;												// -1 ... 1
	float v = 2.f * drand48() * 3.1415926535897932384626;		//  0 ... 2PI
	float x = sqrt(1-u*u)*cos(v);
	float y = u;
	float z = sqrt(1-u*u)*sin(v);
	
	return gk::Point(x, y, z);
}

std::vector<Light> makelights(unsigned int nb, float dist)
{
	std::vector<Light> lights(nb);
	for (Light& light : lights) light = Light::Random() * dist;
	return lights;
}