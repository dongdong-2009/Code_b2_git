#pragma once

#include <string>

using namespace std;

class SimulatorConfig
{
public:
	SimulatorConfig(void);
	~SimulatorConfig(void);
	void ReadConfig();
	static SimulatorConfig*  getInstance();
	static void clearInstance();
public:
	string m_ip,m_connectIp;
	int m_port,m_connectPort;
	string m_typeMsgHandler;
	string m_pointTableFileName;
	int m_atsStatus;
	int m_baseAddr;
private:
	static SimulatorConfig*  m_simulatorConfig;
};
