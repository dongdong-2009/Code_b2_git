#include "StdAfx.h"
#include "SimulatorConfig.h"

SimulatorConfig* SimulatorConfig::m_simulatorConfig = NULL;
SimulatorConfig::SimulatorConfig(void)
:m_ip("127.0.0.1"),m_port(7600),m_typeMsgHandler(""),m_pointTableFileName(""),m_connectIp(""),m_clientCommandFile(""),m_serverCommandFile(""),
m_connectPort(0),m_atsStatus(255),m_bLoadingExtInfo(false),m_bIsUDP(false)
{
	m_extInfo.clear();
}

SimulatorConfig::~SimulatorConfig(void)
{
	m_extInfo.clear();
}

SimulatorConfig* SimulatorConfig::getInstance()
{
	if(m_simulatorConfig==NULL)
	{
		m_simulatorConfig = new SimulatorConfig();
		m_simulatorConfig->ReadConfig();
	}
	return m_simulatorConfig;
}

void SimulatorConfig::clearInstance()
{
	if(m_simulatorConfig!=NULL)
	{
		delete m_simulatorConfig;
		m_simulatorConfig = NULL;
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
		rtn = sscanf(chLine,"isUDP = %d",&iData);
		if(0!=rtn && EOF != rtn)
		{
			m_bIsUDP = iData;
			continue;
		}
		rtn = sscanf(chLine,"handler = %s",chData);
		if(0!=rtn && EOF != rtn)
		{
			m_typeMsgHandler=chData;
			continue;
		}
		rtn = sscanf(chLine,"pt file = %s",chData);
		if(0!=rtn && EOF != rtn)
		{
			m_pointTableFileName=chData;
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
		rtn = sscanf(chLine,"ats status = %d",&iData);
		if(0!=rtn && EOF != rtn)
		{
			m_atsStatus=iData;
			continue;
		}
		rtn = sscanf(chLine,"base addr = %d",&iData);
		if(0!=rtn && EOF != rtn)
		{
			m_baseAddr=iData;
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
	int iData;
	int rtn;
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