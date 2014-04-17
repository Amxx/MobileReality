#include "LMS.hh"

void sampleMesh(LMS<PN,P,float>& lms, gk::Mesh& mesh)
{
	std::vector<double> importance(mesh.triangleCount());
	for (int i=0; i<mesh.triangleCount(); ++i) importance[i]  = mesh.triangle(i).area();
	for (int i=1; i<mesh.triangleCount(); ++i) importance[i] += importance[i-1];
	for (LMS<PN,P,float>::sampletype& sample : lms.samples)
	{
		double seed = drand48() * importance.back();
		int idx=0;
		for (double imp : importance) { if (imp>seed) break; idx++; }
		float u, v;
		mesh.pntriangle(idx).sampleUniformUV(drand48(), drand48(), u, v);
		sample = LMS<PN,P,float>::sampletype(PN(mesh.pntriangle(idx).point(u,v),mesh.pntriangle(idx).normal(u,v)));
	//fprintf(stderr, "[Random Point] seed = %12.7f - triangle = %4d\n", seed, idx);
	}
	for(size_t i=0; i<lms.clusters.size(); ++i)
		lms.clusters[i] = LMS<PN,P,float>::clustertype(lms.samples[i]);
}
