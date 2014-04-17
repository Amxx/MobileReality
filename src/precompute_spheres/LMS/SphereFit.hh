#ifndef SPHEREFIT_HH
#define SPHEREFIT_HH

#include "LMS.hh"

class SphereFit : public LMS<PN, P, float>
{	
	public:
		SphereFit(int snb=10000, int cnb=10) : LMS(snb, cnb) {};
		void init(gk::Mesh*);
	
	private:
		bool  fitfunc   (      clustertype& cluster, const std::vector<sampletype>& samples) const;
		float weightfunc(const clustertype& cluster, const             sampletype & sample ) const;
};

#endif