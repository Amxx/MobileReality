#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <ratio>
#include <chrono>

#include "Mesh.h"
#include "MeshIO.h"


// ############################################################################

#define			LOGHERE					std::cout << "[HERE] " << __FILE__ << " : " << __LINE__ << std::endl;
#define			NOW()						std::chrono::steady_clock::now();

typedef			std::chrono::steady_clock::time_point														timer;
typedef			std::chrono::duration<long int, std::ratio<1l, 1000000000l>>		duration;

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

#define			SAMPLING				10000
#define			KERNEL					10
std::vector<gk::Point> sampling(SAMPLING);

// ############################################################################



void initiate(const std::string path, int verbose = 0)
{
	timer begin = NOW();
	gk::Mesh*	mesh = gk::MeshIO::readOBJ(path);
	std::vector<double> importance(mesh->triangleCount());
	for (int i=0; i<mesh->triangleCount(); ++i) importance[i] =  mesh->triangle(i).area();
	for (int i=1; i<mesh->triangleCount(); ++i) importance[i] += importance[i-1];
	for (gk::Point& pt : sampling)
	{
		double seed = drand48() * importance.back();
		int idx=0;
		for (double imp : importance) { if (imp>seed) break; idx++; }
		mesh->triangle(idx).sampleUniform(drand48(), drand48(), pt);
		if (verbose>1) printf("[Random Point] seed = %12.7f - triangle = %4d\n", seed, idx);
	}
	timer end = NOW();
	if (verbose>0) printf("Initialisaiton %s\n", formatTimer(end-begin).c_str());
	delete mesh;
}
	
	



int main(int argc, char* argv[])
{	
	srand48(time(nullptr));


	if (argc<2) { printf("Usage: %s <file.obj>\n", argv[0]); exit(1); }
	initiate(argv[1], 1);
	
	
	
	
	
	
  return 0;
}
