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
		
		bool load(const std::string& path)
		{
			cv::FileStorage fs(path, cv::FileStorage::READ);
			if (!fs.isOpened()) return false;
			cv::FileNode fn = fs.root();
			for(cv::FileNodeIterator fit = fn.begin(); fit != fn.end(); ++fit)
				(*this)((*fit).name()) = (std::string) (*fit);
			fs.release();
			return true;
		}
		
		bool save (const std::string& path) const
		{
			cv::FileStorage fs(path, cv::FileStorage::WRITE);
			if (!fs.isOpened()) return false;
			for (auto& it : *this)
				fs << it.first << it.second;
			fs.release();
			return true;
		}
		
		void display() const
		{
			std::cout << "CONFIG : " << std::endl;
			for (auto& it : *this) 
				std::cout << " - " << it.first << " : " << it.second << std::endl	;
		}
		
};


#endif