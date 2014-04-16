#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <ratio>
#include <chrono>

#include "Mesh.h"
#include "MeshIO.h"

#include "Eigen/Dense"

using namespace gk;
using namespace Eigen;

// ############################################################################

#define			LOGHERE					std::cout << "[HERE] " << __FILE__ << " : " << __LINE__ << std::endl;
#define			NOW()						std::chrono::steady_clock::now();
typedef			std::chrono::steady_clock::time_point													timer;
typedef			std::chrono::duration<long int, std::ratio<1l, 1000000000l>>	duration;

std::string formatTimer(duration dt)
{
	char buffer[1024];
	sprintf(buffer, "%3d ms %03d µs %03d ns",
					std::chrono::duration_cast<std::chrono::milliseconds>(dt).count(),
					std::chrono::duration_cast<std::chrono::microseconds>(dt).count()%1000,
					std::chrono::duration_cast<std::chrono::nanoseconds>(dt).count()%1000);
	return std::string(buffer);
}

// ############################################################################

class Sample  : public Point {
	public:
		Sample()                          : Point(),  group(0)    {};
		Sample(const Point & p)           : Point(p), group(0)    {};
		Sample(const Point & p, int g)    : Point(p), group(g)    {};
		int   group;
};
class Cluster : public Point {
	public:
		Cluster()                         : Point(),  radius(0.f) {};
		Cluster(const Point & p)          : Point(p), radius(0.f) {};
		Cluster(const Point & p, float r) : Point(p), radius(r)   {};
	
		float weight(const Point& p) const { return abs(Vector(*this - p).Length() - radius); }
		
		float radius;
};

std::vector<Sample>		samples;
std::vector<Cluster>	clusters;
int 									step;

// ############################################################################

bool fit(const std::vector<Sample>& smpls, Cluster& cluster)
{
	Point c = Point(0.f, 0.f, 0.f);
	float r = 0.f;
	
	if (size_t  n = smpls.size())
	{
		MatrixXf A(n, 4);
		VectorXf B(n);
		for (size_t i = 0; i<n; ++i)
		{
			A(i,0) = -2.f*smpls[i].x;
			A(i,1) = -2.f*smpls[i].y;
			A(i,2) = -2.f*smpls[i].z;
			A(i,3) = +1.f;
			B(i)   = Vector(smpls[i]).LengthSquared();
		}
		MatrixXf S = A.jacobiSvd(ComputeThinU | ComputeThinV).solve(B);
	
		// std::cout << "["<<step<<"]-----------------------------------------------" << std::endl;
		// std::cout << S << std::endl;
		
		c = Point(S(0), S(1), S(2));
		r = sqrtf(S(3)-Vector(c).LengthSquared());
		cluster = Cluster(c, r);
		return true;
	}
	else return false;
}

void computefit()
{
	std::vector<std::vector<Sample>> smpls(clusters.size());
	for (const Sample& smpl : samples)
		smpls[smpl.group].push_back(smpl);
	for (size_t i=0; i<clusters.size(); ++i)
		fit(smpls[i], clusters[i]);
}



void initiate(const std::string path, int verbose = 0)
{
	timer begin = NOW();
	
	Mesh*	mesh = MeshIO::readOBJ(path);
	std::vector<double> importance(mesh->triangleCount());
	for (int i=0; i<mesh->triangleCount(); ++i) importance[i]  = mesh->triangle(i).area();
	for (int i=1; i<mesh->triangleCount(); ++i) importance[i] += importance[i-1];
	for (Sample& smpl : samples)
	{
		double seed = drand48() * importance.back();
		int idx=0;
		for (double imp : importance) { if (imp>seed) break; idx++; }
		mesh->triangle(idx).sampleUniform(drand48(), drand48(), smpl);
		if (verbose>1) fprintf(stderr, "[Random Point] seed = %12.7f - triangle = %4d\n", seed, idx);
	}
	for(size_t i=0; i<clusters.size(); ++i) clusters[i] = Cluster(samples[i]);
	delete mesh;
	
	computefit();
	
	timer end = NOW();
	if (verbose>0) fprintf(stderr, "Initialisation %s\n", formatTimer(end-begin).c_str());
}

void run(int max_step = -1)
{
	timer begin = NOW();
	
	step=0;
	bool loop = true;
	while (loop && max_step != step++)
	{
		loop = false;
		for (size_t i=0; i<samples.size(); ++i)
		{
			int old=samples[i].group;
			for (size_t k=0; k<clusters.size(); ++k)
				if (clusters[k].weight(samples[i])<clusters[samples[i].group].weight(samples[i])) samples[i].group=k;	
			if (old!=samples[i].group) loop = true;
		}
		if (loop) computefit();
	}
	
	timer end = NOW();
	fprintf(stderr, "Convergence in %d iterations : %s\n", step, formatTimer(end-begin).c_str());
	fprintf(stderr, "Stopped by : %s\n", ((loop)?std::string("step limitation"):std::string("matching stability")).c_str());
	
	std::cout	<< "====================================" << std::endl;
	for (Cluster cluster: clusters) printf("%10f %10f %10f %10f\n", cluster.x, cluster.y, cluster.z, cluster.radius);
	std::cout	<< "====================================" << std::endl;
	
	
}







	



int main(int argc, char* argv[])
{	
	srand48(time(nullptr));

	if (argc<2) { printf("Usage: %s <file.obj>\n", argv[0]); exit(1); }
	
	
	size_t	samples_nb	= 100000;
	size_t	clusters_nb	= 10;
	int			max_step		= 1000;
	
	
	samples.resize(samples_nb);
	clusters.resize(clusters_nb);
	initiate(argv[1], 1);
	run(max_step);
	
  return 0;
}
