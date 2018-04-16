#pragma once
#include "AQCommonDef.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "SynSimuCliientInfo.h"

#include "DBSyncSimCommon.h"
#include <map>

class CGUIUpdateHandler
{
public:
	static CGUIUpdateHandler& getInstance();
	static void releaseInstance();

	~CGUIUpdateHandler(void);
	void notifySetupClients(const T_CLIENT_MAP& mapClients);
	void setTargetWnd(HWND targetWnd) {m_hwndTarget=targetWnd; };

	void notifyEnqueueStatusUpdate(int nClientIdx, CLIENT_STATUS nStatus);
	void notifyDequeueStatusUpdate(int nClientIdx, CLIENT_STATUS nStatus);
	void notifyConnStatusUpdate(int nClientIdx, CLIENT_STATUS nStatus);

	void notifEnqueueMsgIDUpdate(int nClientIdx, TA_AQ_Lib::ulong64 ulMsgID);
	void notifDequeueMsgIDUpdate(int nClientIdx, TA_AQ_Lib::ulong64 ulMsgID);

	void notifyClientUpdate(const std::map<std::string, TA_DB_Sync::SimuClientInfo*>& clients);

private:
	CGUIUpdateHandler(void);
	void _notifyStatusUpdate(int nClientIdx, CLIENT_STATUS nStatus, CLIENT_UPDATE_TYPE nType);
	void _notifyMsgIDUpdate(int nClientIdx, TA_AQ_Lib::ulong64 ulMsgID, CLIENT_UPDATE_TYPE nType);

private:
	static CGUIUpdateHandler* m_pInstance;
	static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
	TA_Base_Core::NonReEntrantThreadLockable m_threadLock;

	T_CLIENT_MAP m_clients;
	T_ClientInfo_Map m_mapClients;

	HWND m_hwndTarget;

};
