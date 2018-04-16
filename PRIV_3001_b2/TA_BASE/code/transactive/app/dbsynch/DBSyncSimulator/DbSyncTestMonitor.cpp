#include "StdAfx.h"
#include "DbSyncTestMonitor.h"
#include "GUIUpdateHandler.h"

#define REFRESH_INTERVAL 1000 // 1 second

CDbSyncTestMonitor::CDbSyncTestMonitor(IDBSyncSimGUIAccess* pSimTestAccess)
	: m_pSimTestAccess(pSimTestAccess)
	, m_bTerminate(false)
{
}

CDbSyncTestMonitor::~CDbSyncTestMonitor(void)
{
}

void CDbSyncTestMonitor::run()
{
	while(!m_bTerminate)
	{
		std::map<std::string, TA_DB_Sync::SimuClientInfo*> clients = m_pSimTestAccess->getClientInfos();
		if (clients.size() != 0)
		{
			CGUIUpdateHandler::getInstance().notifyClientUpdate(clients);
		}

		TA_Base_Core::Thread::sleep(REFRESH_INTERVAL);
	}
}

void CDbSyncTestMonitor::terminate()
{
	m_bTerminate = true;
}