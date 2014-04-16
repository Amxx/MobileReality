#ifndef LMS_HH
#define LMS_HH

#include "Eigen/Dense"

#include "timers.hh"


template<typename T>
class Sample  : public T {
	public:
		Sample()                   : T(),  group(0) {};
		Sample(const T & t)        : T(t), group(0) {};
		Sample(const T & t, int g) : T(t), group(g) {};
		//------------------------------------------------
		int group;
};

template<typename T, typename P>
class Cluster : public T {
	public:
		Cluster()                 : T(),  parameter()  {};
		Cluster(const T & t)      : T(t), parameter()  {};
		Cluster(const T & t, P p) : T(t), parameter(p) {};
		//------------------------------------------------
		P parameter;
};

template<typename T, typename P>		
class LMS
{
	public:
		typedef Sample<T>    SampleT;
		typedef Cluster<T,P> ClusterTP;
		
		LMS(int snb=100000, int cnb=10) : samples(snb), clusters(cnb) {};
		void init();
		void run(int max_step = -1);
	
	private:
		void computefit();
		virtual bool  fitfunc(ClusterTP&, const std::vector<SampleT>&) const = 0;
		virtual float weightfunc(const ClusterTP&, const SampleT&) 		 const = 0;

	public:
		std::vector<SampleT>   samples;
		std::vector<ClusterTP> clusters;
	
};










template<typename T, typename P>		
void LMS<T,P>::init()
{
	//SET samples
	//SET clusters
	computefit();
}

template<typename T, typename P>		
void LMS<T,P>::run(int max_step)
{
	timer begin = now();
	
	int		step = 0;
	bool	loop = true;
	while (loop && max_step != step++)
	{
		printf("%d\n", step);
		loop = false;
		for (size_t i=0; i<samples.size(); ++i)
		{
			int old=samples[i].group;
			for (size_t k=0; k<clusters.size(); ++k)
				if (weightfunc(clusters[k], samples[i]) < weightfunc(clusters[samples[i].group], samples[i])) samples[i].group=k;	
			if (old!=samples[i].group) loop = true;
		}
		if (loop) computefit();
	}
	
	timer end = now();
	fprintf(stderr, "Convergence in %d iterations : %s\n", step, formatTimer(end-begin).c_str());
	fprintf(stderr, "Stopped by : %s\n", ((loop)?std::string("step limitation"):std::string("matching stability")).c_str());
	
	// std::cout	<< "====================================" << std::endl;
	// for (ClusterTP cluster: clusters) printf("%10f %10f %10f %10f\n", cluster.x, cluster.y, cluster.z, cluster.radius);
	// std::cout	<< "====================================" << std::endl;
}

template<typename T, typename P>	
void LMS<T,P>::computefit()
{
	std::vector<std::vector<SampleT>> smpls(clusters.size());
	for (const SampleT& smpl : samples)
		smpls[smpl.group].push_back(smpl);
	for (size_t i=0; i<clusters.size(); ++i)
		fitfunc(clusters[i], smpls[i]);
}
#endif