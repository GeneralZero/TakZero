#pragma once
#include <fstream>
#include <map>
#include <string>

class ConfigStore
{
public:
	static ConfigStore& get()
	{
		static ConfigStore instance;
		return instance;
	}
	void parseFile(std::ifstream& inStream);
	template<typename _T>
	_T getValue(std::string key);

	std::map<std::string, std::string> strings;
	std::map<std::string, uint32_t> ints;
	std::map<std::string, double> doubles;
	std::map<std::string, bool> bools;
	std::fstream cfg_logfile_handle;
private:
	ConfigStore() {};
	ConfigStore(const ConfigStore&);
	ConfigStore& operator=(const ConfigStore&);
};