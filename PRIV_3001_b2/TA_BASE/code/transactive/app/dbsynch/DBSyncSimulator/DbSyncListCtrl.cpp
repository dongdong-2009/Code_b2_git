// DbSyncListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "DBSyncSimulator.h"
#include "DbSyncListCtrl.h"

#include "core/utilities/src/TAAssert.h"
// CDbSyncListCtrl
using namespace TA_Base_Core;

IMPLEMENT_DYNAMIC(CDbSyncListCtrl, CListCtrl)

CDbSyncListCtrl::CDbSyncListCtrl()
	: m_ulDeqTotal(0)
	, m_ulEnqTotal(0)
{  

}

CDbSyncListCtrl::~CDbSyncListCtrl()
{
}


BEGIN_MESSAGE_MAP(CDbSyncListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT( NM_CUSTOMDRAW, OnNMCustomdrawListDbsync )
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CDbSyncListCtrl message handlers
void CDbSyncListCtrl::Initialize()
{
	// SetUp Headers
	InsertColumn(COL_CLIENT_NAME, "Clients", LVCFMT_CENTER, 100);
	InsertColumn(COL_DEQUEUE_MSGID, "DeqMsgID", LVCFMT_CENTER, 80);
	InsertColumn(COL_DEQUEUE_COUNT, "DeqCount", LVCFMT_CENTER, 80);
	InsertColumn(COL_DEQUEUE_MSGID, "EnqMsgID", LVCFMT_CENTER, 80);
	InsertColumn(COL_DEQUEUE_COUNT, "EnqCount", LVCFMT_CENTER, 80);
	InsertColumn(COL_CONN_STATUS, "ConStatus", LVCFMT_CENTER, 80);
	InsertColumn(COL_GEN_STATUS, "GeneralStatus", LVCFMT_CENTER, 80);
	InsertColumn(COL_ERROR_COUNT, "DEQ/ENQ Error Count", LVCFMT_LEFT, 200);

	SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

void CDbSyncListCtrl::SetClientList(const T_CLIENT_MAP& mapClients)
{
	m_mapClients.clear();
	
	T_CLIENT_MAP::const_iterator itr = mapClients.begin();
	for (itr; itr != mapClients.end(); itr++)
	{
		ClientInfo* pClientInfo = new ClientInfo;
		pClientInfo->strClientName = itr->second;

		int nIndex = this->GetItemCount(); 
		InsertItem(nIndex, pClientInfo->strClientName.c_str());
		SetItemText(nIndex, COL_DEQUEUE_MSGID, g_strStatus[pClientInfo->nDequeueStatus].c_str());
		SetItemText(nIndex, COL_DEQUEUE_COUNT, g_strStatus[pClientInfo->nDequeueStatus].c_str());
		SetItemText(nIndex, COL_ENQUEUE_MSGID, g_strStatus[pClientInfo->nEnqueueStatus].c_str());
		SetItemText(nIndex, COL_ENQUEUE_COUNT, g_strStatus[pClientInfo->nEnqueueStatus].c_str());
		SetItemText(nIndex, COL_CONN_STATUS, g_strStatus[pClientInfo->nConnStatus].c_str());
		SetItemText(nIndex, COL_GEN_STATUS, g_strStatus[pClientInfo->nSimStatus].c_str());
		SetItemText(nIndex, COL_ERROR_COUNT, formatErrorInfo(pClientInfo->ulDequeueErrCount, pClientInfo->ulEnqueueErrCount).c_str());

		m_mapClients[pClientInfo->strClientName] = pClientInfo;
		SetItemData(nIndex, (DWORD)pClientInfo);
	}

}

BOOL CDbSyncListCtrl::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CDbSyncListCtrl::OnPaint() 
{
	//Double buffer the window to avoid flickering.
	CRect rcClient;
	GetClientRect(rcClient);

	CPaintDC dc(this);
	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);

	CBitmap bmMem;
	bmMem.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
	CBitmap* pbmOld = dcMem.SelectObject(&bmMem);

	dcMem.FillSolidRect(rcClient, ::GetSysColor(COLOR_WINDOW));

	this->DefWindowProc(WM_PAINT, (WPARAM)dcMem.m_hDC, (LPARAM)0);

	dc.BitBlt(0,0,rcClient.Width(), rcClient.Height(), &dcMem, 0, 0, SRCCOPY);
	dcMem.SelectObject(pbmOld);

	CHeaderCtrl*    pCtrl = this->GetHeaderCtrl();
	if (::IsWindow(pCtrl->GetSafeHwnd()))
	{
		CRect   aHeaderRect;
		pCtrl->GetClientRect(&aHeaderRect);
		pCtrl->RedrawWindow(&aHeaderRect);
	}
}
void CDbSyncListCtrl::OnNMCustomdrawListDbsync(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
	//int iRow = lplvcd->nmcd.dwItemSpec;

	switch(lplvcd->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT :
		{
			*pResult = CDRF_NOTIFYITEMDRAW;
			return;
		}
	case CDDS_ITEMPREPAINT:
		{
			*pResult = CDRF_NOTIFYSUBITEMDRAW | CDRF_NOTIFYPOSTPAINT;;				
			return;
		}
	case CDDS_ITEMPREERASE:
		{
			*pResult = CDRF_NOTIFYSUBITEMDRAW;
			return;
		}
		// Modify sub item text and/or background
	case CDDS_SUBITEM | CDDS_PREPAINT | CDDS_ITEM:
		{
			DWORD row=lplvcd->nmcd.dwItemSpec;
			DWORD col=lplvcd->iSubItem;

			ClientInfo* pClient = (ClientInfo*) GetItemData(row);
			
			if (NULL == pClient)
			{
				*pResult = CDRF_DODEFAULT;
				return;
			}
			CString str;
			lplvcd->clrText = RGB(0,0,0);
			
			switch(col)
			{
			case COL_CLIENT_NAME:
				lplvcd->clrTextBk = RGB(244,164,96);
				str = pClient->strClientName.c_str();
				break;
			case COL_DEQUEUE_MSGID:
				lplvcd->clrTextBk = g_clrRef[pClient->nDequeueStatus];
				str = ulong64ToString(pClient->ulDequeueMsgID).c_str();
				break;
			case COL_DEQUEUE_COUNT:
				lplvcd->clrTextBk = g_clrRef[pClient->nDequeueStatus];
				str = ulong64ToString(pClient->ulDeqTotalMgs).c_str();
				break;
			case COL_ENQUEUE_MSGID:
				lplvcd->clrTextBk = g_clrRef[pClient->nEnqueueStatus];
				str = ulong64ToString(pClient->ulEnqueueMsgID).c_str();
				break;
			case COL_ENQUEUE_COUNT:
				lplvcd->clrTextBk = g_clrRef[pClient->nEnqueueStatus];
				str = ulong64ToString(pClient->ulEnqTotalMgs).c_str();
				break;
			case COL_CONN_STATUS:
				lplvcd->clrTextBk = g_clrRef[pClient->nConnStatus];
				str = g_strStatus[pClient->nConnStatus].c_str();
				break;
			case COL_GEN_STATUS:
				lplvcd->clrTextBk = g_clrRef[pClient->nSimStatus];
				str = g_strStatus[pClient->nSimStatus].c_str();
				break;
			case COL_ERROR_COUNT:
				str = formatErrorInfo(pClient->ulDequeueErrCount, pClient->ulEnqueueErrCount).c_str();
				//break;
			default:
				lplvcd->clrTextBk = RGB(255,255,255);
				break;
			}

			//CString str = GetItemText(row,col);

			CDC* pDC;
			CRect rect;
			pDC = CDC::FromHandle(lplvcd->nmcd.hdc);
			CBrush brush(lplvcd->clrTextBk);
			GetSubItemRect(row,col,LVIR_BOUNDS,rect);

			if (col != 1)
				rect.left=rect.left+1;

			rect.bottom=rect.bottom-1;
			pDC->FillRect(&rect,&brush);

			rect.OffsetRect(6,0);
			rect.right -= 6;
			pDC->DrawText(str,&rect,DT_END_ELLIPSIS|DT_NOPREFIX);
			
			*pResult = CDRF_SKIPDEFAULT;
			return;
		}
	case CDDS_ITEMPOSTPAINT :
		{
			DWORD row=lplvcd->nmcd.dwItemSpec;
			if(lplvcd->nmcd.uItemState & CDIS_SELECTED)
			{
				// Test
				CDC* pDC;
				CRect rect;
				pDC = CDC::FromHandle(lplvcd->nmcd.hdc);

				COLORREF polor = RGB(0,0,255);
				GetItemRect(row,rect,LVIR_BOUNDS);
				rect.bottom -= 1;
				CBrush brush1(polor);
				pDC->FrameRect(&rect,&brush1);
			}

			*pResult = CDRF_SKIPDEFAULT;
			return;
		}
	}


	*pResult = 0;
}


void CDbSyncListCtrl::ProccessStatusUpdate(ClientUpdate* pUpdate)
{
	if (NULL != pUpdate)
	{
		ClientInfo*pClientInfo = m_mapClients[pUpdate->strClientName];
		if (NULL != pUpdate)
		{
			switch (pUpdate->nUpdateType)
			{
			case DEQUEUE_UPDATE:
				pClientInfo->nDequeueStatus = pUpdate->nStatus;
				break;
			case ENQUEUE_UPDATE:
				pClientInfo->nEnqueueStatus = pUpdate->nStatus;
				break;
			case CONNECTION_UPDATE:
				pClientInfo->nConnStatus = pUpdate->nStatus;
				break;
			}

			this->Invalidate();
		}
	}
}

void CDbSyncListCtrl::ProccessMgsIDUpdate(ClientUpdate* pUpdate)
{
	if (NULL != pUpdate)
	{
		ClientInfo*pClientInfo = m_mapClients[pUpdate->strClientName];
		if (NULL != pUpdate)
		{
			switch (pUpdate->nUpdateType)
			{
			case DEQUEUE_UPDATE:
				pClientInfo->ulDequeueMsgID = pUpdate->ulMgsID;
				break;
			case ENQUEUE_UPDATE:
				pClientInfo->ulEnqueueMsgID = pUpdate->ulMgsID;
				break;
			case CONNECTION_UPDATE:
				TA_ASSERT(false, "Connection don't have message update");
				break;
			}

			this->Invalidate();
		}
	}
}

std::string CDbSyncListCtrl::ulong64ToString(TA_AQ_Lib::ulong64 ulData)
{
	std::string strRetVal;
	std::stringstream mystream;

	mystream << ulData;
	strRetVal = mystream.str();

	return strRetVal;
}

std::string CDbSyncListCtrl::formatErrorInfo(ULONG ulDeqErr, ULONG ulEnqErr)
{
	std::string strRetVal;
	std::stringstream mystream;

	mystream << "Deq Err: ";
	mystream << ulDeqErr;
	mystream << ", ";
	mystream << "Enq Err: ";
	mystream << ulEnqErr;

	strRetVal = mystream.str();

	return strRetVal;
}

std::string CDbSyncListCtrl::formatQueDisplay(TA_AQ_Lib::ulong64 totalQueue, TA_AQ_Lib::ulong64 lastMsgID)
{
	std::string strRetVal;
	std::stringstream mystream;

	mystream << "Total: ";
	mystream << totalQueue;
	mystream << ", ";
	mystream << "Last MsgID: ";
	mystream << lastMsgID;

	strRetVal = mystream.str();

	return strRetVal;
}

std::string CDbSyncListCtrl::u64tostr(TA_AQ_Lib::ulong64 u64Val)
{
	std::string strRet;
	unsigned nDigit = 0;
	while (u64Val >= 10)
	{
		nDigit = u64Val - (u64Val / 10) * 10;
		nDigit |= 0x30;

		strRet.insert(0, 1, nDigit);
		u64Val = u64Val / 10;
	}
	nDigit = u64Val | 0x30;
	strRet.insert(0, 1, nDigit);

	return strRet;
}

void CDbSyncListCtrl::ProcessClientUpdate(T_ClientInfo_Map& pClientUpdate)
{
	T_ClientInfo_Map::iterator itr = pClientUpdate.begin();
	m_ulDeqTotal = 0;
	m_ulEnqTotal = 0;

	for (int nIdx=0; nIdx<GetItemCount(); nIdx++)
	{
		ClientInfo* pClientInfo = (ClientInfo*)GetItemData(nIdx);
		pClientInfo->nConnStatus = pClientUpdate[pClientInfo->strClientName]->nConnStatus;
		pClientInfo->nDequeueStatus =pClientUpdate[pClientInfo->strClientName]->nDequeueStatus;
		pClientInfo->nEnqueueStatus =pClientUpdate[pClientInfo->strClientName]->nEnqueueStatus;
		pClientInfo->nSimStatus		=pClientUpdate[pClientInfo->strClientName]->nSimStatus;
		pClientInfo->ulDequeueMsgID =pClientUpdate[pClientInfo->strClientName]->ulDequeueMsgID;
		pClientInfo->ulEnqueueMsgID =pClientUpdate[pClientInfo->strClientName]->ulEnqueueMsgID;
		pClientInfo->ulEnqueueErrCount = pClientUpdate[pClientInfo->strClientName]->ulEnqueueErrCount;
		pClientInfo->ulDequeueErrCount = pClientUpdate[pClientInfo->strClientName]->ulDequeueErrCount;
		pClientInfo->ulEnqTotalMgs = pClientUpdate[pClientInfo->strClientName]->ulEnqTotalMgs;
		pClientInfo->ulDeqTotalMgs = pClientUpdate[pClientInfo->strClientName]->ulDeqTotalMgs;

		m_ulDeqTotal += pClientInfo->ulDeqTotalMgs;
		m_ulEnqTotal += pClientInfo->ulEnqTotalMgs;

	}

	
	this->Invalidate();
}

void CDbSyncListCtrl::GetTotalDeqEnq(TA_AQ_Lib::ulong64& ulDeq, TA_AQ_Lib::ulong64& ulEnq)
{
	ulDeq = m_ulDeqTotal;
	ulEnq = m_ulEnqTotal;
}
