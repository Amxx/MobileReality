#include "light.hh"

std::vector<Light> makelights(unsigned int nb, float dist)
{
	std::vector<Light> r;
	
	for (unsigned int i=0; i<nb; ++i)
	{
		float u = 2.f * drand48() - 1.0f;												// -1 ... 1
		float v = 2.f * drand48() * 3.1415926535897932384626;		//  0 ... 2PI
		float x = sqrt(1-u*u)*cos(v);
		float y = sqrt(1-u*u)*sin(v);
		float z = u;
		r.push_back(Light(i, dist * gk::Point(x, y, z)));
	}
	return r;
}