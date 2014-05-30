/*
Copyright or © or Copr. Hadrien Croubois (2014)

hadrien.croubois@ens-lyon.fr

This software is a computer program whose purpose is to render, in real
time, virtual objects usgin dynamically acquired environnement.

This software is governed by the CeCILL-B license under French law and
abiding by the rules of distribution of free software.  You can  use, 
modify and/ or redistribute the software under the terms of the CeCILL-B
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info". 

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability. 

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or 
data to be ensured and,  more generally, to use and operate it in the 
same conditions as regards security. 

The fact that you are presently reading this means that you have had
knowledge of the CeCILL-B license and that you accept its terms.
*/

#ifndef LMS_HH
#define LMS_HH

#include "Eigen/Dense"
#include "../tools/P.hh"
#include "../tools/timers.hh"

// =================================
// =   G K I T   I N C L U D E S   =
// =================================
#include "Mesh.h"
#include "MeshIO.h"


typedef bool UNUSEDTYPE;

template<typename S>
struct Sample  : public S
{
	Sample()                  : S(),  group(0) {};
	Sample(const S& s)        : S(s), group(0) {};
	Sample(const S& s, int g) : S(s), group(g) {};
	int group;
};

template<typename C, typename P>
struct Cluster : public C
{
	Cluster()                : C(),  parameter()  {};
	Cluster(const C& c)      : C(c), parameter()  {};
	Cluster(const C& c, P p) : C(c), parameter(p) {};
	P parameter;
};

template<typename S, typename C, typename P>		
class LMS
{
	public:
		typedef Sample<S>    sampletype;
		typedef Cluster<C,P> clustertype;
		
		LMS(int snb=100000, int cnb=10) : samples(snb), clusters(cnb) {};
		void run(int max_step = -1);
		void step();
		virtual void finalize();
	protected:
		bool computegroups();
		void computefit();
		virtual bool  fitfunc   (      clustertype&, const std::vector<sampletype>&) const = 0;
		virtual float weightfunc(const clustertype&, const             sampletype &) const = 0;

	public:
		std::vector<sampletype>   samples;
		std::vector<clustertype> clusters;
};


void sampleMesh(LMS<PN,P,float>&, gk::Mesh&);







template<typename S, typename C, typename P>		
void LMS<S,C,P>::run(int max_step)
{
	timer begin = now();
	
	step(); // needed for one single cluster

	int step = 1;
	while (computegroups() && max_step != step++)
	{		
		computefit();
			
		float sf = (float) step / max_step;
		int   si = 50*sf;
		fprintf(stdout, "\rIteration %4d/%d [", step-1, max_step);
		for (int i=0; i<si; ++i)  fprintf(stdout, "#");
		for (int i=si; i<50; ++i) fprintf(stdout, " ");
		fprintf(stdout, "] %3d%%", (int)(sf*100));
		fflush(stdout);
	}
	
	timer end = now();
	fprintf(stdout, "\n");
	fprintf(stdout, "Stopped after %d iterations : %s\n", step-1, formatTimer(end-begin).c_str());
	fprintf(stdout, "Stopped by : %s\n", ((max_step<step)?std::string("step limitation"):std::string("matching stability")).c_str());
}

template<typename S, typename C, typename P>	
void LMS<S,C,P>::step()
{
	computegroups();
	computefit();
}

template<typename S, typename C, typename P>	
void LMS<S,C,P>::finalize()
{
}





template<typename S, typename C, typename P>	
bool LMS<S,C,P>::computegroups()
{
	bool changes = false;
	for (sampletype& sample : samples)
	{
		int oldgroup = sample.group;
		for (size_t k=0; k<clusters.size(); ++k)
			if (weightfunc(clusters[k], sample) < weightfunc(clusters[sample.group], sample))
				sample.group=k;	
		if (oldgroup!=sample.group)
			changes = true;
	}
	return changes;
}

template<typename S, typename C, typename P>
void LMS<S,C,P>::computefit()
{
	std::vector<std::vector<sampletype>> smpls(clusters.size());
	for (const sampletype& smpl : samples)
		smpls[smpl.group].push_back(smpl);
	for (size_t i=0; i<clusters.size(); ++i)
		fitfunc(clusters[i], smpls[i]);
}
#endif
