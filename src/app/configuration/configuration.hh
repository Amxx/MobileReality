#ifndef CONFIGURATION_HH
#define CONFIGURATION_HH

#include <iostream>

#include "options.hh"

class Configuration
{	
	public:
		// ============ Object ============
		Options::Object				object;
		// =========== Devices ============
		Options::Devices			devices;
		// =========== Markers ============
		Options::Markers			markers;
		// =========== General ============
		Options::General			general;

	public:
		Configuration();
		Configuration& load(const std::string& path);
		Configuration& check();
		Configuration& display();
		const Configuration& display() const;
};
#endif