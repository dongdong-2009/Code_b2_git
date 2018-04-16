#pragma once
#include <string>
#include <iostream>
using namespace std;

class PointDataTable
{
public:
	PointDataTable(void);
	~PointDataTable(void);
	string getPointData(const string& fileName);
private:
	string m_fileName;
};
