#pragma once

#include "../IParser.h"

class CsvParser : public IParser
{
	public:
		CsvParser(void);
		~CsvParser(void);
		
		virtual std::vector <std::string> getSeparatedStringValues (std::string strValue);		
		virtual std::vector <long> getSeparatedNumberValues (std::string strValue);		

	private:
		long CsvParser::hexStringToLong (const std::string & strData);
};
