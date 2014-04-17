#ifndef SPHEREFIT_HH
#define SPHEREFIT_HH

#include "LMS.hh"

// =================================
// =   G K I T   I N C L U D E S   =
// =================================
#include "Mesh.h"
#include "MeshIO.h"



struct P : public gk::Point
{
	P()                   : gk::Point()  {}
	P(const gk::Point& p) : gk::Point(p) {}
};
struct PN : public gk::Point
{
	PN()                                       : gk::Point(),  normal()  {}
	PN(const gk::Point& p)                     : gk::Point(p), normal()  {}
	PN(const gk::Point& p, const gk::Point& n) : gk::Point(p), normal(n) {}
	//------------------------------------------------
	gk::Point normal;
};




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