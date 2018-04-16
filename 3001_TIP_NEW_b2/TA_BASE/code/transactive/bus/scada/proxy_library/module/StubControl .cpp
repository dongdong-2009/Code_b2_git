#include "StubControl.h"
	

	bool StubControl::isSet(const std::string& parameter)
	{
		bool rtn = false;
		if(parameter.size()==0)
		{
			rtn = false;
		}
		else
		{
			std::map<std::string,int>::iterator itr;
			if( (itr = m_parameters.find(parameter)) != m_parameters.end())
			{
				rtn = true;
			}
		}
		return rtn;
	}
	bool StubControl::isSet(char *pStr)
	{
		bool rtn = false;
		if(pStr == NULL)
		{
			rtn = false;
		}
		else
		{
			std::string str = pStr;
			rtn = this->isSet(str);
		}
		return rtn;
	}
	void StubControl::set(const std::string parameter)
	{
		std::map<std::string,int>::iterator itr =  m_parameters.find(parameter);
		if( itr == m_parameters.end())
		{
			m_parameters[parameter] = 0;
		}
		else
		{
			m_parameters[parameter]++;
		}
	}
	
	void StubControl::clearAll()
	{
		m_parameters.clear();
	}
	
	void StubControl::clear(std::string parameter)
	{
		m_parameters.erase(parameter);
	}

	StubControl& StubControl::getInstance()
	{
		static	StubControl m_instance;
		return m_instance;
	}