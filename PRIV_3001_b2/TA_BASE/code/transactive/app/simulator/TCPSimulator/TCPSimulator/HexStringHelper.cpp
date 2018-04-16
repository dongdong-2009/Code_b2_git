#include "StdAfx.h"
#include "HexStringHelper.h"

HexStringHelper::HexStringHelper(void)
{
}

HexStringHelper::~HexStringHelper(void)
{
}

void HexStringHelper::HexString(const string& data,string& output )
{
	string::const_iterator di=data.begin();
	output.erase(output.begin(),output.end());
	int dd;
	for(;di!=data.end();di++)
	{
		dd=static_cast<int>(*di) &0xff;
		 
		output+=dd /16 +((dd /16)<10? '0':'A'-10);
		output+=dd %16 +((dd %16)<10? '0':'A'-10);
		output+=' ';

	}

}

void HexStringHelper::StringHex(const string& data,string& output )
{
	
	string::const_iterator di=data.begin();
	output.erase(output.begin(),output.end());
	int count(0);
	unsigned char cc,ci;
	for(;di!=data.end();di++)
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
			output.push_back(ci);
		}
		count++;
	}
	if(count>0 && count%2 == 1)
		output.push_back(ci);
}
