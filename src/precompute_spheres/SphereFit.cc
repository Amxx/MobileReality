#include "SphereFit.hh"


void SphereFit::init(gk::Mesh* mesh)
{
	std::vector<double> importance(mesh->triangleCount());
	for (int i=0; i<mesh->triangleCount(); ++i) importance[i]  = mesh->triangle(i).area();
	for (int i=1; i<mesh->triangleCount(); ++i) importance[i] += importance[i-1];
	for (sampletype& sample : samples)
	{
		double seed = drand48() * importance.back();
		int idx=0;
		for (double imp : importance) { if (imp>seed) break; idx++; }
		mesh->triangle(idx).sampleUniform(drand48(), drand48(), sample);
	//fprintf(stderr, "[Random Point] seed = %12.7f - triangle = %4d\n", seed, idx);
	}
	for(size_t i=0; i<clusters.size(); ++i)
		clusters[i] = clustertype(samples[i], 1.f);
}






bool SphereFit::fitfunc(clustertype& cluster, const std::vector<sampletype>& samples) const
{
	
	gk::Point c = gk::Point(0.f, 0.f, 0.f);
	float r = 0.f;
	if (size_t  n = samples.size())
	{
		Eigen::MatrixXf A(n, 4);
		Eigen::VectorXf B(n);
		for (size_t i = 0; i<n; ++i)
		{
			A(i,0) = -2.f*samples[i].x;
			A(i,1) = -2.f*samples[i].y;
			A(i,2) = -2.f*samples[i].z;
			A(i,3) = +1.f;
			B(i)   = gk::Vector(samples[i]).LengthSquared();
		}
		
		Eigen::MatrixXf S = A.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(B);
		
		// std::cout << "------------------------------------------------" << std::endl;
		// std::cout << S																									<< std::endl;
		
		c = gk::Point(S(0), S(1), S(2));
		r = S(3)-gk::Vector(c).LengthSquared();
		
		cluster = clustertype(c, sqrtf(abs(r)));
		// cluster = clustertype(c, (r<0)?0:sqrtf(r));
		return true;
	}
	else 
	{
		cluster = clustertype(gk::Point(0.f, 0.f, 0.f), 0.f);
		return false;
	}	
	//cluster.parameter = 1.f;
}

float SphereFit::weightfunc(const clustertype& cluster, const sampletype& sample) const
{
	return abs(gk::Vector(cluster-sample).Length()-cluster.parameter);
}