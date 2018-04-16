#ifndef _STUB_CONTROL_H__
#define _STUB_CONTROL_H__
#include<map>
#include<string>

class StubControl
{
public:
	bool isSet(const std::string& parameter);
	bool isSet(char *pStr);
	void set(const std::string parameter);
	void clearAll();
	void clear(std::string parameter);
	static StubControl& getInstance();
private:
	
	std::map<std::string,int> m_parameters;
};

#endif