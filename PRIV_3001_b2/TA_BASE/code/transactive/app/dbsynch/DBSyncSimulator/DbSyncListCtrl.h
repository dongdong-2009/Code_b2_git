#include "DBSyncSimCommon.h"
#include <vector>

#pragma once

// CDbSyncListCtrl

class CDbSyncListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CDbSyncListCtrl)
private:
	enum VIEW_COL{
		COL_CLIENT_NAME = 0,
		COL_DEQUEUE_MSGID,
		COL_DEQUEUE_COUNT,
		COL_ENQUEUE_MSGID,
		COL_ENQUEUE_COUNT,
		COL_CONN_STATUS,
		COL_GEN_STATUS,
		COL_ERROR_COUNT
	};
public:
	CDbSyncListCtrl();
	virtual ~CDbSyncListCtrl();

	void Initialize();
	void SetClientList(const T_CLIENT_MAP& mapClients);
	afx_msg void OnNMCustomdrawListDbsync(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();

	void ProccessStatusUpdate(ClientUpdate*pUpdate);
	void ProccessMgsIDUpdate(ClientUpdate* pUpdate);
	void ProcessClientUpdate(T_ClientInfo_Map& pClientUpdate);
	void GetTotalDeqEnq(TA_AQ_Lib::ulong64& ulDeq, TA_AQ_Lib::ulong64& ulEnq);
	
protected:
	DECLARE_MESSAGE_MAP()

private:
	std::string ulong64ToString(TA_AQ_Lib::ulong64 ulData);
	std::string formatErrorInfo(ULONG ulDeqErr, ULONG ulEnqErr);
	std::string u64tostr(TA_AQ_Lib::ulong64 u64Val);
	std::string formatQueDisplay(TA_AQ_Lib::ulong64 totalQueue, TA_AQ_Lib::ulong64 lastMsgID);
private:
	T_ClientInfo_Map m_mapClients;
	TA_AQ_Lib::ulong64 m_ulDeqTotal;
	TA_AQ_Lib::ulong64 m_ulEnqTotal;
};


