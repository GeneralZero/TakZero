#pragma once
#include <map>

#ifdef _WIN32
#undef HAVE_SELECT
#define NOMINMAX
#else
#define HAVE_SELECT
#endif


#define PROGRAM_NAME "TakZero"
#define PROGRAM_VERSION "0.9.1"

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
	std::map<std::string, std::uint64_t> ints;
	std::map<std::string, bool> bools;
	std::map<std::string, double> doubles;
private:
	ConfigStore() {};
	ConfigStore(const ConfigStore&);
	ConfigStore& operator=(const ConfigStore&);

};