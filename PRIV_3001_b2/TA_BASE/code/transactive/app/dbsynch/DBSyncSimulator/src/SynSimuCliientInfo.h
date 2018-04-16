#ifndef  __CLASS_SYNSIMUCLINETINFO_HH___
#define  __CLASS_SYNSIMUCLINETINFO_HH___

#include "CommonDef.h"
#include "CommonData.h"


NS_BEGIN(TA_DB_Sync)

enum SimulatorClientStatus
{
    Normal,
    Pause,
    Error,
    Finished
};

class SimuClientInfo 
{
public:
	SimuClientInfo();
	~SimuClientInfo();
public:
	std::string AgentName;
	ulong64     LastEnqSeqID;
	ulong64     LastDeqSeqID;
	ulong64     totalEnq;
	ulong64     totalDeq;
    SimulatorClientStatus Status;		
	bool        IsTcpHealth;  
	bool        IsEnqHealth;  
    bool        IsDeqHealth;  
  
public:
	SimuClientInfo& operator=(const SimuClientInfo& infoSrc);
	std::string getStatus();
	int logSimuClientInfo();
};

NS_END(TA_DB_Sync)



#endif /// __CLASS_SYNSIMUCLINETINFO_HH___
