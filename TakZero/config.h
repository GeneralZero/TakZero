#pragma once
#include <map>

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
	std::map<std::string, uint64_t> ints;
	std::map<std::string, bool> bools;
	std::map<std::string, double> doubles;
private:
	ConfigStore() {};
	ConfigStore(const ConfigStore&);
	ConfigStore& operator=(const ConfigStore&);

};