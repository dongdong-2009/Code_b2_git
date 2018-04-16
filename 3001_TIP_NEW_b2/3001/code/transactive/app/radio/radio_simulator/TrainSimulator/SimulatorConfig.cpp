#include "StdAfx.h"
#include "SimulatorConfig.h"

#include "core/synchronisation/src/ThreadGuard.h"

#include <Windows.h>

//ATL Conversion Macro
#include <atlbase.h>
#include <atlconv.h>

#include <sstream>
#include <string>

TA_Base_Core::ReEntrantThreadLockable SimulatorConfig::m_ObjectLock;

std::string SimulatorConfig::m_SimConfigName = "\\RadioIDConfig.cfg";

SimulatorConfig* SimulatorConfig::m_simulatorConfig = NULL;
SimulatorConfig::SimulatorConfig(void)
:m_ip("127.0.0.1"),m_port(7600),m_connectIp(""),m_clientCommandFile(""),m_serverCommandFile(""),
m_connectPort(0),m_bLoadingExtInfo(false),m_trainsList(""),m_heartBeatDelay(""),
m_libraryVersion("0.0.0.0"),m_logStatus(false),m_pecSource("1"),
m_currentLibVersion("0.0.0.0"),m_nextLibVersion("0.0.0.0"),
m_preRecordLibVersion("0.0.0.0")
{
	m_extInfo.clear();
}

SimulatorConfig::~SimulatorConfig(void)
{
	m_extInfo.clear();
}

std::string SimulatorConfig::getFileNamepath()
{
	TCHAR path[1024];
	::GetModuleFileName(NULL,path,1024);

	//Convert it to Ascii
	CW2A _convert(path);
	std::string _strPath(_convert);

	std::string::size_type pos = std::string(_strPath).find_last_of("\\");
	return std::string(_strPath).substr(0,pos);
}

std::map<int,int> SimulatorConfig::getRadioIDMap()
{
	std::map<int,int> _retMap;
	for (int x=1; x<=150; x++)
	{
		int _radioID = getRadioID(x);
		_retMap.insert(std::pair<int,int>(x,_radioID));
	}
	return _retMap;
}

int SimulatorConfig::getRadioID(int trainID, bool bPrimaryTsi)
{
	int _ret = 0;

	std::ostringstream _str;
	_str << "TrainID_"
		 << trainID;

	std::string strRet = getValueFromSectionKey(_str.str().c_str(),"RadioID").c_str();

	// parse comma separated value

	int nIndex = strRet.find(",");

	if (nIndex != std::string::npos)
	{
		std::string strMid;

		if (bPrimaryTsi)
		{
			
			strMid = strRet.substr(0,nIndex);
		}
		else
		{
			strMid = strRet.substr(nIndex + 1);
		}
		_ret = atol (strMid.c_str());
	}
	else
	{
		_ret = atol (strRet.c_str());
	}


	return _ret;
}

std::string SimulatorConfig::getValueFromSectionKey(const char* section, const char* key)
{
	//Convert to Unicode 
	CA2W uSection(section);
	CA2W uKey(key);

	std::string _strPath = getFileNamepath();
	_strPath.append(m_SimConfigName);

	CA2W uPath(_strPath.c_str());

	char* _value = new char[1024];
	char* _default = new char[1024];

	sprintf(_default,"EMPTY");

	CA2W uValue(_value);
	CA2W uDefault(_default);


	DWORD iTemRet = ::GetPrivateProfileString(uSection,uKey,uDefault,uValue,strlen(_value),uPath);

	CW2A _ret(uValue);
	std::string _return(_ret);

	delete[] _default;
	delete[] _value;

	return _return;
}

SimulatorConfig* SimulatorConfig::getInstance()
{
	{
		TA_Base_Core::ThreadGuard guard(m_ObjectLock);

		if(m_simulatorConfig==NULL)
		{
			m_simulatorConfig = new SimulatorConfig();
			m_simulatorConfig->ReadConfig();
		}
		return m_simulatorConfig;
	}
	
}

void SimulatorConfig::clearInstance()
{
	if(m_simulatorConfig!=NULL)
	{
		delete m_simulatorConfig;
		m_simulatorConfig = NULL;
	}
}

bool SimulatorConfig::checkRadioIDConfig()
{
	{
		TA_Base_Core::ThreadGuard guard(m_ObjectLock);
		bool _ret = false;

		FILE * fp=fopen("RadioIDConfig.cfg","rt");
		if(NULL != fp )
			_ret = true;
		fclose(fp);
		return _ret;
	}
}
void SimulatorConfig::ReadConfig()
{
	FILE * fp=fopen("Simulator.cfg","rt");
	if(fp==NULL)
	{
		AfxMessageBox(L"fail to open simulator.cfg");
		return;
	}
	char chLine[1024],chData[1024];
	int iData;
	int rtn;
	int iLogStatus;

	memset(chLine,0,sizeof(chLine));
	m_extInfo.clear();
	while(NULL!=fgets(chLine,sizeof(chLine),fp))
	{
		if(memcmp(chLine,"//",2 )==0 )
			continue;
		if(memcmp(chLine,"[EXT INFO]",strlen("[EXT INFO]"))==0)
		{
			m_bLoadingExtInfo = true;
			continue;
		}
		if(memcmp(chLine,"[END EXT INFO]",strlen("[END EXT INFO]"))==0)
		{
			m_bLoadingExtInfo = false;
			continue;
		}
		if(m_bLoadingExtInfo)
		{
			std::string str = chLine;
			m_extInfo.push_back(str);
			continue;
		}
		memset(chData,0,sizeof(chData));
		rtn = sscanf(chLine,"server ip = %s",chData);
		if(0!=rtn && EOF != rtn)
		{
			m_ip=chData;
			continue;
		}
		rtn = sscanf(chLine,"server port = %d",&iData);
		if(0!=rtn && EOF != rtn)
		{
			m_port=iData;
			continue;
		}

		rtn = sscanf(chLine,"log status = %d",&iLogStatus);
		if(0!=rtn && EOF != rtn)
		{
			m_logStatus=iLogStatus;
			continue;
		}

		rtn = sscanf(chLine,"Trains List Heartbeat = %s",chData);
		if(0!=rtn && EOF != rtn)
		{
			m_trainsList = chData;
			continue;
		}

		rtn = sscanf(chLine,"Heartbeat DelaySeconds = %s",chData);
		if(0!=rtn && EOF != rtn)
		{
			m_heartBeatDelay = chData;
			continue;
		}


		rtn = sscanf(chLine,"pds server ip = %s",chData);
		if(0!=rtn && EOF != rtn)
		{
			m_PDSServerIP = chData;
			continue;
		}

		rtn = sscanf(chLine,"pds server port = %s",chData);
		if(0!=rtn && EOF != rtn)
		{
			m_PDSServerPort = chData;
			continue;
		}


		rtn = sscanf(chLine,"connect ip = %s",chData);
		if(0!=rtn && EOF != rtn)
		{
			m_connectIp=chData;
			continue;
		}
		rtn = sscanf(chLine,"connect port = %d",&iData);
		if(0!=rtn && EOF != rtn)
		{
			m_connectPort=iData;
			continue;
		}

		rtn = sscanf(chLine,"upgrade version = %s",chData);
		if(0!=rtn && EOF != rtn)
		{
			m_libraryVersion=chData;
			continue;
		}

		rtn = sscanf(chLine,"current library version = %s",chData);
		if(0!=rtn && EOF != rtn)
		{
			m_currentLibVersion=chData;
			continue;
		}

		rtn = sscanf(chLine,"next library version = %s",chData);
		if(0!=rtn && EOF != rtn)
		{
			m_nextLibVersion=chData;
			continue;
		}

		rtn = sscanf(chLine,"pre-recorded library version = %s",chData);
		if(0!=rtn && EOF != rtn)
		{
			m_preRecordLibVersion=chData;
			continue;
		}

		rtn = sscanf(chLine,"pec source = %s",chData);
		if(0!=rtn && EOF != rtn)
		{
			m_pecSource=chData;
			continue;
		}

		rtn = sscanf(chLine,"client command file = %s",chData);
		if(0!=rtn && EOF != rtn)
		{
			m_clientCommandFile=chData;
			continue;
		}
		rtn = sscanf(chLine,"server command file = %s",chData);
		if(0!=rtn && EOF != rtn)
		{
			m_serverCommandFile=chData;
			continue;
		}

		memset(chLine,0,sizeof(chLine));
	}
	fclose(fp);
}

void SimulatorConfig::ReadExtConfig()
{
	FILE * fp=fopen("Simulator.cfg","rt");
	if(fp==NULL)
	{
		AfxMessageBox(L"fail to open simulator.cfg");
		return;
	}
	char chLine[1024],chData[1024];
	int iData = 0;
	int rtn = 0;
	memset(chLine,0,sizeof(chLine));
	m_extInfo.clear();
	while(NULL!=fgets(chLine,sizeof(chLine),fp))
	{
		if(memcmp(chLine,"//",2 )==0 )
			continue;
		if(memcmp(chLine,"[EXT INFO]",strlen("[EXT INFO]"))==0)
		{
			m_bLoadingExtInfo = true;
			continue;
		}
		if(memcmp(chLine,"[END EXT INFO]",strlen("[END EXT INFO]"))==0)
		{
			m_bLoadingExtInfo = false;
			continue;
		}
		if(m_bLoadingExtInfo)
		{
			std::string str = chLine;
			m_extInfo.push_back(str);
			continue;
		}
		memset(chLine,0,sizeof(chLine));
	}
	fclose(fp);
}

int SimulatorConfig::getHeartBeatDelay()
{
	int _ret = 0;
	_ret = atoi(m_heartBeatDelay.c_str());
	return _ret;
}

std::vector<int> SimulatorConfig::getTrainsListHeartBeat()
{
	std::vector<int> _ret = stringTokenizer(m_trainsList,",");
	return _ret;
}

std::vector<int> SimulatorConfig::stringTokenizer(const std::string &str, const std::string &delim)
{
	vector<int> tokens;
	size_t p0 = 0, p1 = string::npos;

	while(p0 != string::npos)
	{
		p1 = str.find_first_of(delim, p0);
		if(p1 != p0)
		{
			string token = str.substr(p0, p1 - p0);
			tokens.push_back(atoi(token.c_str()));
		}
		p0 = str.find_first_not_of(delim, p1);
	}
	return tokens;
}

std::vector<int> SimulatorConfig::getLibraryVersion()
{
	std::vector<int> _ret = stringTokenizer(m_libraryVersion,".");
	return _ret;
}

std::vector<int> SimulatorConfig::getCurrentLibVersion()
{
	std::vector<int> _ret = stringTokenizer(m_currentLibVersion,".");
	return _ret;
}

std::vector<int> SimulatorConfig::getNextLibVersion()
{
	std::vector<int> _ret = stringTokenizer(m_nextLibVersion,".");
	return _ret;
}

std::vector<int> SimulatorConfig::getPreRecordedLibVersion()
{
	std::vector<int> _ret = stringTokenizer(m_preRecordLibVersion,".");
	return _ret;
}
