#include "PointFit.hh"

bool PointFit::fitfunc(clustertype& cluster, const std::vector<sampletype>& samples) const
{	
	gk::Point c(0.f, 0.f, 0.f);
	float 		r(0.f);
	for (sampletype sample : samples) c += gk::Vector(sample)            / samples.size();
	for (sampletype sample : samples) r += gk::Vector(c-sample).Length() / samples.size();
	cluster = clustertype(c, r);
	return true;
}

float PointFit::weightfunc(const clustertype& cluster, const sampletype& sample) const
{
	return gk::Vector(cluster-sample).LengthSquared();
}