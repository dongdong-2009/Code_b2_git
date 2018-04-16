#pragma once
#include <string>
#include <iostream>
using namespace std;

class HexStringHelper
{
public:
	HexStringHelper(void);
	~HexStringHelper(void);
	static void HexString(const string& data,string& output );
	static void StringHex(const string& data,string& output );
};
