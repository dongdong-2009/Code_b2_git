#pragma once

#include "IParser.h"

#include <vector>
#include <string>


class IParser
{
	public:
		virtual std::vector <std::string> getSeparatedStringValues (std::string strValue) = 0;		
		virtual std::vector <long> getSeparatedNumberValues (std::string strValue) = 0;
};
