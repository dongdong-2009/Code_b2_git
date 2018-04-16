#include "StdAfx.h"
#include "GUIUpdateHandler.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/TAAssert.h"

using namespace TA_Base_Core;

CGUIUpdateHandler* CGUIUpdateHandler::m_pInstance = NULL;
TA_Base_Core::NonReEntrantThreadLockable CGUIUpdateHandler::m_singletonLock;

CGUIUpdateHandler& CGUIUpdateHandler::getInstance()
{
	if (NULL != m_pInstance)
	{
		return *m_pInstance;
	}
	
	ThreadGuard guard(m_singletonLock);
	if (NULL == m_pInstance)
	{
		m_pInstance = new CGUIUpdateHandler();
	}

	return *m_pInstance;
}

void CGUIUpdateHandler::releaseInstance()
{
	ThreadGuard guard(m_singletonLock);
	if (NULL != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = 0;
	}
}

CGUIUpdateHandler::CGUIUpdateHandler(void)
{
}

CGUIUpdateHandler::~CGUIUpdateHandler(void)
{
	std::map<std::string, ClientInfo*>::const_iterator itr = m_mapClients.begin();
	for (itr; itr != m_mapClients.end(); itr++)
	{
		ClientInfo* pInfo = itr->second;
		if (NULL != pInfo)
		{
			delete pInfo;
			pInfo = NULL;
		}
	}

	m_mapClients.clear();
}


void CGUIUpdateHandler::notifySetupClients(const T_CLIENT_MAP& mapClients)
{
	TA_ASSERT(NULL != m_hwndTarget, "Target window not set.");
	ThreadGuard guard(m_threadLock);

	// copy clients
	m_clients = mapClients;

	WPARAM wParam = (WPARAM)&m_clients;
	LPARAM lParam = 0;

	PostMessage(m_hwndTarget, WMU_SETUP_CLIENT, wParam, lParam);	
}


void CGUIUpdateHandler::notifyEnqueueStatusUpdate(int nClientIdx, CLIENT_STATUS nStatus)
{
	TA_ASSERT(NULL != m_hwndTarget, "Target window not set.");
	ThreadGuard guard(m_threadLock);

	_notifyStatusUpdate(nClientIdx, nStatus, ENQUEUE_UPDATE);
}

void CGUIUpdateHandler::notifyDequeueStatusUpdate(int nClientIdx, CLIENT_STATUS nStatus)
{
	TA_ASSERT(NULL != m_hwndTarget, "Target window not set.");
	ThreadGuard guard(m_threadLock);

	_notifyStatusUpdate(nClientIdx, nStatus, DEQUEUE_UPDATE);
}

void CGUIUpdateHandler::notifyConnStatusUpdate(int nClientIdx, CLIENT_STATUS nStatus)
{
	TA_ASSERT(NULL != m_hwndTarget, "Target window not set.");
	ThreadGuard guard(m_threadLock);

	_notifyStatusUpdate(nClientIdx, nStatus, CONNECTION_UPDATE);
}

void CGUIUpdateHandler::notifEnqueueMsgIDUpdate(int nClientIdx, TA_AQ_Lib::ulong64 ulMsgID)
{
	TA_ASSERT(NULL != m_hwndTarget, "Target window not set.");
	ThreadGuard guard(m_threadLock);

	_notifyMsgIDUpdate(nClientIdx, ulMsgID, ENQUEUE_UPDATE);
}

void CGUIUpdateHandler::notifDequeueMsgIDUpdate(int nClientIdx, TA_AQ_Lib::ulong64 ulMsgID)
{
	TA_ASSERT(NULL != m_hwndTarget, "Target window not set.");
	ThreadGuard guard(m_threadLock);

	_notifyMsgIDUpdate(nClientIdx, ulMsgID, DEQUEUE_UPDATE);

}

void CGUIUpdateHandler::_notifyStatusUpdate(int nClientIdx, CLIENT_STATUS nStatus, CLIENT_UPDATE_TYPE nType)
{
	ClientUpdate* pUpdate = new ClientUpdate;

	//pUpdate->nClientIdx = nClientIdx;
	pUpdate->nUpdateType = nType;
	pUpdate->nStatus = nStatus;

	WPARAM wParam = (WPARAM)pUpdate;
	LPARAM lParam = 0;

	PostMessage(m_hwndTarget, WMU_CLIENT_STATUS_UPDATE, wParam, lParam);	
}

void CGUIUpdateHandler::_notifyMsgIDUpdate(int nClientIdx, TA_AQ_Lib::ulong64 ulMsgID, CLIENT_UPDATE_TYPE nType)
{
	ClientUpdate* pUpdate = new ClientUpdate;

	//pUpdate->nClientIdx = nClientIdx;
	pUpdate->nUpdateType = nType;
	pUpdate->ulMgsID = ulMsgID;
	
	WPARAM wParam = (WPARAM)pUpdate;
	LPARAM lParam = 0;

	PostMessage(m_hwndTarget, WMU_CLIENT_MSGID_UPDATE, wParam, lParam);	
}

void CGUIUpdateHandler::notifyClientUpdate(const std::map<std::string, TA_DB_Sync::SimuClientInfo*>& clients)
{
	std::map<std::string, TA_DB_Sync::SimuClientInfo*>::const_iterator itr = clients.begin();
	for (itr; itr != clients.end(); itr++)
	{
		TA_DB_Sync::SimuClientInfo* pSimInfo = (TA_DB_Sync::SimuClientInfo*)itr->second;
		
		//fix runing crash
		if (true == pSimInfo->AgentName.empty())
		{
			return;
		}
		
		T_ClientInfo_Map::iterator itFind = m_mapClients.find(itr->first);
		
		if (itFind != m_mapClients.end())
		{	
			ClientInfo* pInfo = (ClientInfo*)itFind->second;
			
			if( STATUS_GOOD == pInfo->nEnqueueStatus && false == pSimInfo->IsEnqHealth)
				pInfo->ulEnqueueErrCount += 1;

			if( STATUS_GOOD == pInfo->nDequeueStatus && false == pSimInfo->IsDeqHealth)
				pInfo->ulDequeueErrCount += 1;
			
			pInfo->nConnStatus = pSimInfo->IsTcpHealth?STATUS_GOOD:STATUS_ERROR;
			pInfo->nDequeueStatus = pSimInfo->IsDeqHealth?STATUS_GOOD:STATUS_ERROR;
			pInfo->nEnqueueStatus = pSimInfo->IsEnqHealth?STATUS_GOOD:STATUS_ERROR;
			pInfo->nSimStatus = (CLIENT_STATUS)pSimInfo->Status;
			pInfo->ulDequeueMsgID = pSimInfo->LastDeqSeqID;
			pInfo->ulEnqueueMsgID = pSimInfo->LastEnqSeqID;
			pInfo->ulEnqTotalMgs = pSimInfo->totalEnq;
			pInfo->ulDeqTotalMgs = pSimInfo->totalDeq;

		}
		else
		{
			ClientInfo* pInfo = new ClientInfo();
			
			if( STATUS_GOOD == pInfo->nEnqueueStatus && false == pSimInfo->IsEnqHealth)
				pInfo->ulEnqueueErrCount += 1;

			if( STATUS_GOOD == pInfo->nDequeueStatus && false == pSimInfo->IsDeqHealth)
				pInfo->ulDequeueErrCount += 1;
			
			pInfo->strClientName = pSimInfo->AgentName;
			pInfo->nConnStatus = pSimInfo->IsTcpHealth?STATUS_GOOD:STATUS_ERROR;
			pInfo->nDequeueStatus = pSimInfo->IsDeqHealth?STATUS_GOOD:STATUS_ERROR;
			pInfo->nEnqueueStatus = pSimInfo->IsEnqHealth?STATUS_GOOD:STATUS_ERROR;
			pInfo->nSimStatus = (CLIENT_STATUS)pSimInfo->Status;
			pInfo->ulDequeueMsgID = pSimInfo->LastDeqSeqID;
			pInfo->ulEnqueueMsgID = pSimInfo->LastEnqSeqID;
			pInfo->ulEnqTotalMgs = pSimInfo->totalEnq;
			pInfo->ulDeqTotalMgs = pSimInfo->totalDeq;


			m_mapClients[pInfo->strClientName] = pInfo;
		}
	}

	WPARAM wParam = (WPARAM)&m_mapClients;
	LPARAM lParam = 0;

	PostMessage(m_hwndTarget, WMU_CLIENT_UPDATE, wParam, lParam);	

}