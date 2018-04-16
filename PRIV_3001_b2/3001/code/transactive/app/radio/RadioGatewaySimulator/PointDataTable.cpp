#include "StdAfx.h"
#include "PointDataTable.h"
#include "HexStringHelper.h"

PointDataTable::PointDataTable(void)
:m_fileName("")
{
	
}

PointDataTable::~PointDataTable(void)
{
}

string PointDataTable::getPointData(const string& fileName)
{
	FILE* fp;
	fp =fopen(fileName.c_str(),"rt");
	if(fp==NULL)
	{
		AfxMessageBox( L"can not open PointTable file ");
		return "";
	}
	string strData="",strHexData="";
	char dataLine[256];
	memset(dataLine,0,sizeof(dataLine));
	while( fgets(dataLine,sizeof(dataLine),fp)!=NULL)
	{
		strData += dataLine;
		memset(dataLine,0,sizeof(dataLine));
	}
	fclose(fp);
	HexStringHelper hexHelper;
	hexHelper.StringHex(strData,strHexData);
	m_fileName=fileName;
	return strHexData;
}
