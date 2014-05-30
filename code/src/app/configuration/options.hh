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

#ifndef OPTIONS_HH
#define OPTIONS_HH

#include <string>
#include <opencv2/core/core.hpp>

namespace Options
{
	enum Type
	{
		DEBUG = 0,
		DYNAMIC = 1,
	};
	struct Object
	{
		
		Object() : obj_file("cube.obj"), spheres_file(), scale(1.f) {}
		std::string		obj_file;
		std::string		spheres_file;
		float					scale;
	};
	struct Devices
	{
		struct Device
		{
			Device() : enable(true), id(-2), params("tmp_params.xml") {}
			bool				enable;
			int					id;
			std::string	params;
		};
		Devices()	: front(), back() {}
		Device				front;
		Device				back;
	};
	struct Markers
	{
		Markers() : size(1.f), scale(1.f) {}
		float					size;
		float					scale;
	};
	struct General
	{
		struct DefaultValues
		{
			DefaultValues() : brightness(-1), gain(-1), persistency(0) {}
			int						brightness;
			int						gain;
			int						persistency;
		};
		struct Envmap
		{
			Envmap() : build(true), size(256,256), dual(false), path("") {}
			bool				build;
			cv::Size		size;
			bool				dual;
			std::string	path;
		};
		struct Localisation
		{
			Localisation() : type(DYNAMIC), size(0.f) {};
			Type				type;
			float				size;
		};
		struct Rendering
		{
			Rendering() : background(true), scene(true), view(false) {}
			bool				background;
			bool				scene;
			bool				view;
		};
		struct Modules
		{
			Modules() : scanner("install/share/libscanner_zbar.so"), video("install/share/libvideodevice_uvc.so") {}
			std::string	scanner;
			std::string	video;
		};
		General() : verbose(0), defaultValues(), envmap(), localisation(), rendering(), modules() {}
		int						verbose;
		DefaultValues	defaultValues;
		Envmap				envmap;
		Localisation	localisation;
		Rendering			rendering;
		Modules				modules;
	};
};

#endif