#include "IDBSyncSimGUIAccess.h"
#include "core/threads/src/Thread.h"

#pragma once

class CDbSyncTestMonitor
	: public TA_Base_Core::Thread
{
public:
	CDbSyncTestMonitor(IDBSyncSimGUIAccess* pSimTestAccess);
	~CDbSyncTestMonitor(void);

	virtual void run();
	virtual void terminate();

private:
	bool m_bTerminate;
	IDBSyncSimGUIAccess* m_pSimTestAccess;
};
