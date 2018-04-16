#ifndef __IDB_SYNC_SIM_GUI_ACCESS_H_INCLUDED__
#define __IDB_SYNC_SIM_GUI_ACCESS_H_INCLUDED__

#include <map>
#include "SynSimuCliientInfo.h"

class IDBSyncSimGUIAccess
{
public: 
	virtual int	continueWork() = 0;

	virtual int	pauseWork() = 0;
	
	virtual int	printStatus() = 0;	//write to log
	
	virtual std::map<std::string, TA_DB_Sync::SimuClientInfo*>&   getClientInfos() = 0;

};



#endif //__IDB_SYNC_SIM_GUI_ACCESS_H_INCLUDED__