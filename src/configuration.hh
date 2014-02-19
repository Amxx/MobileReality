#ifndef CONFIGURATION_HH
#define CONFIGURATION_HH


#include <map>
#include <string>

class Configuration : private std::map<std::string, std::string>
{
	public:
		std::string& operator() (const std::string& k)
		{
			return std::map<std::string, std::string>::operator[] (k);
		}
};


#endif