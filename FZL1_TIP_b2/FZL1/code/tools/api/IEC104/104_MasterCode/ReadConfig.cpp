// readconfig.cpp: implementation of the readconfig class.
//
//////////////////////////////////////////////////////////////////////

#include "ReadConfig.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ReadConfig*ReadConfig::m_singleton=NULL;
ReadConfig::ReadConfig()
{	
	m_analog_count=0;
	m_point_count=0;

}

ReadConfig::~ReadConfig()
{
	read_config();

}
int ReadConfig::get_analog_count(){
	return m_analog_count;

}
int ReadConfig::get_point_count(){
	return m_point_count;
	
}
ReadConfig*ReadConfig::instance(){
	if(m_singleton==NULL)
        m_singleton=new ReadConfig();
	return m_singleton;

}
bool ReadConfig::read_config(){
	return true;
}