#pragma once

#include <string>
#include <list>
using namespace std;

class SimulatorConfig
{
public:
	SimulatorConfig(void);
	~SimulatorConfig(void);
	void ReadConfig();
	void ReadExtConfig();
	static SimulatorConfig*  getInstance();
	static void clearInstance();
public:
	string m_ip,m_connectIp;
	int m_port,m_connectPort;
	string m_typeMsgHandler;
	string m_pointTableFileName;
	string m_clientCommandFile,m_serverCommandFile;
	int m_atsStatus;
	int m_baseAddr;
	bool m_bIsUDP;
	std::list<std::string> m_extInfo;
private:
	static SimulatorConfig*  m_simulatorConfig;
	bool	m_bLoadingExtInfo;
};
