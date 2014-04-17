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
	
	int step = 0;
	while (computegroups() && max_step != step++)
	{		
		computefit();
			
		float sf = (float) step / max_step;
		int   si = 80*sf;
		fprintf(stdout, "\rIteration %4d/%d [", step-1, max_step);
		for (int i=0; i<si; ++i)  fprintf(stdout, "#");
		for (int i=si; i<80; ++i) fprintf(stdout, " ");
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
	printf("computing step ... "); fflush(stdout);
	computegroups();
	computefit();
	printf("done\n");
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