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
