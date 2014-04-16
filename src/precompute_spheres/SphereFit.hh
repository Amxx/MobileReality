#ifndef SPHEREFIT_HH
#define SPHEREFIT_HH

#include "LMS.hh"

// =================================
// =   G K I T   I N C L U D E S   =
// =================================
#include "Mesh.h"
#include "MeshIO.h"


class SphereFit : public LMS<gk::Point, float>
{	
	public:
		typedef Sample<gk::Point>         SampleT;
		typedef Cluster<gk::Point, float> ClusterTP;
	
	public:
		SphereFit(int snb=10000, int cnb=10) : LMS(snb, cnb) {};
		void init(gk::Mesh*);
	
	private:
		bool  fitfunc   (      ClusterTP& cluster, const std::vector<SampleT>& samples) const;
		float weightfunc(const ClusterTP& cluster, const             SampleT & sample ) const;
};

#endif