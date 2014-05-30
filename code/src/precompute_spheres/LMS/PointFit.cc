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

#include "PointFit.hh"

void PointFit::finalize()
{
	std::vector<gk::Normal> normals(clusters.size());
	std::vector<int>				count(clusters.size());
	for (sampletype sample : samples) 
	{
		normals[sample.group] += sample.normal;
		count[sample.group]++;
	}
	for (size_t i=0; i<clusters.size(); ++i)
		clusters[i] -= clusters[i].parameter * gk::Vector(normals[i])/count[i];	
}


bool PointFit::fitfunc(clustertype& cluster, const std::vector<sampletype>& samples) const
{	
	gk::Point c(0.f, 0.f, 0.f);
	float 		r(0.f);
	for (sampletype sample : samples) c += gk::Vector(sample)            / samples.size();
	for (sampletype sample : samples) r += gk::Vector(c-sample).Length() / samples.size();
	// for (sampletype sample : samples) r = std::max(r, gk::Vector(c-sample).Length());
	cluster = clustertype(c, r);
	return true;
}

float PointFit::weightfunc(const clustertype& cluster, const sampletype& sample) const
{
	return gk::Vector(cluster-sample).LengthSquared();
}