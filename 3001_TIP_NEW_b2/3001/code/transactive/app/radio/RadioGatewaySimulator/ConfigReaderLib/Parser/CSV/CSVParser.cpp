
#include "CsvParser.h"

CsvParser::CsvParser(void)
{
	
}

CsvParser::~CsvParser(void)
{
}


std::vector <std::string> CsvParser::getSeparatedStringValues (std::string strValue)
{
	std::vector <std::string> vctrReturn;
	
	std::string strTemp = strValue;

	int nPos = strTemp.find(",");
	
	while (nPos != strTemp.npos)
	{
		// get the first value

		std::string strVal = strTemp.substr(0,nPos);
 		strTemp.erase(0,  nPos + 1);
 
 		vctrReturn.push_back (strVal);
 
 		nPos = strTemp.find(",");
 
 		// add the last item on the vector
 		if (nPos == strTemp.npos)
 		{
 			vctrReturn.push_back (strTemp);
 		}		
	}

	return vctrReturn;
}

std::vector <long> CsvParser::getSeparatedNumberValues (std::string strValue)
{
	std::vector <long> vctrReturn;
	std::string strTemp = strValue;

	int nPos = strTemp.find(",");

	while (nPos != strTemp.npos)
	{
		// get the first value

		std::string strVal = strTemp.substr(0,nPos);
		strTemp.erase(0,  nPos + 1);

		vctrReturn.push_back (hexStringToLong(strVal));

		nPos = strTemp.find(",");

		// add the last item on the vector
		if (nPos == strTemp.npos)
		{
			vctrReturn.push_back ((unsigned char)hexStringToLong(strTemp));
		}		
	}

	return vctrReturn;
}

long CsvParser::hexStringToLong (const std::string & strData)
{
	std::string::const_iterator di = strData.begin();
	int count(0);
	unsigned char cc,ci;

	for(;di!=strData.end();di++)
	{ 
		cc = static_cast<char>(*di) &0xff;
		if(cc>='a' && cc<='f')
			cc=cc-'a'+'A';
		if(!(cc>='0'&&cc<='9'||cc>='A'&&cc<='F'))
			continue;
		if(count%2 == 0) //using 2 chars to represent 1 hex byte
			ci = (cc>='0'&&cc<='9'?cc-'0':cc-'A'+10);
		else
		{
			ci =ci<< 4;
			ci += (cc>='0'&&cc<='9'?cc-'0':cc-'A'+10);
		}
		count++;
	}

	return (long)ci;
}
