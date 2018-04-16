
#pragma once

#include <string>
#include <vector>

class IIniConfigHandler
{
	public:
		virtual std::string getValueFromKeyAndSection (std::string strSection,std::string strKey) = 0;
		virtual std::vector <std::string> getKeysFromSection (std::string strSection) = 0;

		virtual bool setValueFromKeyAndSection (std::string strSection,
												std::string strKey,
												std::string strValue) = 0;
		
};