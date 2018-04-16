#ifndef __READCONFIG__
#define __READCONFIG__
#include <map>
class ReadConfig  
{
public:
    
	int get_analog_count();
	int get_point_count();
	static ReadConfig*instance();
private:
	ReadConfig();
	virtual ~ReadConfig();
	bool read_config();
	int m_analog_count;
	int m_point_count;
	static ReadConfig * m_singleton;
	
};

#endif 
