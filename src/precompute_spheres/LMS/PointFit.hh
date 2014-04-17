#ifndef POINTFIT_HH
#define POINTFIT_HH

#include "LMS.hh"

class PointFit : public LMS<PN, P, float>
{	
	public:
		PointFit(int snb=10000, int cnb=10) : LMS(snb, cnb) {};
		void finalize();
	private:
		bool  fitfunc   (      clustertype& cluster, const std::vector<sampletype>& samples) const;
		float weightfunc(const clustertype& cluster, const             sampletype & sample ) const;
};

#endif