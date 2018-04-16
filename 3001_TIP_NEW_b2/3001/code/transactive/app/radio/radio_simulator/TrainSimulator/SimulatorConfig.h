#pragma once

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include <string>
#include <list>
#include <vector>
#include <map>

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
	int getHeartBeatDelay();
	vector<int> getTrainsListHeartBeat();
	vector<int> getLibraryVersion();
	vector<int> getCurrentLibVersion();
	vector<int> getNextLibVersion();
	vector<int> getPreRecordedLibVersion();
	vector<int> stringTokenizer(const string & str, const string & delim);

	std::map<int,int> getRadioIDMap();
	
	//Hook PEC Source
	static std::string getPECSourceConfig();

public:
	string m_ip,m_connectIp;
	int m_port,m_connectPort;
	int m_logStatus;

	string m_PDSServerIP;
	string m_PDSServerPort;

	string m_trainsList;
	string m_heartBeatDelay;

	string m_libraryVersion;

	string m_currentLibVersion;
	string m_nextLibVersion;
	string m_preRecordLibVersion;
	
	

	string m_clientCommandFile,m_serverCommandFile;

	std::list<std::string> m_extInfo;

	int getRadioID(int trainID, bool bPrimaryTsi = true);
	bool checkRadioIDConfig();
	std::string m_pecSource;

private:
	static SimulatorConfig*  m_simulatorConfig;
	bool	m_bLoadingExtInfo;

	
	

	static TA_Base_Core::ReEntrantThreadLockable m_ObjectLock;

	//Use for RadioID
	static std::string getFileNamepath();
	static std::string getValueFromSectionKey(const char* section, const char* key);
	static bool stringToBool(const std::string& str);
	static std::string m_SimConfigName;

};
