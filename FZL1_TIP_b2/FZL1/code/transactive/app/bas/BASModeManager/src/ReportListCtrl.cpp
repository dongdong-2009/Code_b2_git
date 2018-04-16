// ReportListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "app/bas/BASModeManager/src/BASManager.h"
#include "app/bas/BASModeManager/src/ReportListCtrl.h"
#include "app/bas/BASModeManager/src/ModeRunningStatus.h"
#include "app/bas/BASModeManager/src/BasManagerDataHelper.h"
Namespace_TA_IRS_APP_Begin
// ReportListCtrl
IMPLEMENT_DYNAMIC(ReportListCtrl, BASBaseListCtrl)
ReportListCtrl::ReportListCtrl()
{

}

ReportListCtrl::~ReportListCtrl()
{
}


BEGIN_MESSAGE_MAP(ReportListCtrl, BASBaseListCtrl)
	//ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_TIMER()
END_MESSAGE_MAP()



// ReportListCtrl message handlers
void ReportListCtrl::RefreshGUI(UINT nIDEvent)
{
	if (nIDEvent == ReportlistCtrlColorTimer)
	{
	}
	BASBaseListCtrl::OnTimer(nIDEvent);
}


void ReportListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	*pResult = 0;

	if (pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
	{
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
	else if (pLVCD->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT | CDDS_SUBITEM))
	{
		int nItem = pLVCD->nmcd.dwItemSpec;
		if (IsWindow(this->m_hWnd))
		{
			/*if (GetItemCount() > nItem)			
			{
				unsigned long modeKey = static_cast<unsigned long>(GetItemData(nItem));
				enumModeRunningStatus modeStatus = ModeStatusManagerInstance::instance().getModeStatus(modeKey);
				if (MRS_Fail == modeStatus)
				{
					pLVCD->clrTextBk = COLOR_YELLOW;
					*pResult = CDRF_NOTIFYSUBITEMDRAW;
				}
				else if (MRS_Running == modeStatus)
				{
					pLVCD->clrTextBk = COLOR_RED;
					*pResult = CDRF_NOTIFYSUBITEMDRAW;
				}
				else if (MRS_Success == modeStatus)
				{
					pLVCD->clrTextBk = COLOR_GREEN;
					*pResult = CDRF_NOTIFYSUBITEMDRAW;
				}
			}*/
		}
	}
}
void ReportListCtrl::UpdateCurrentMode(TA_IRS_Core::Mode_SmartPtr& mode_ptr)
{
	FUNCTION_ENTRY("UpdateCurrentMode");
	unsigned int subsystemKey = mode_ptr->getSubsystemKey();
	int subsystemCount = GetItemCount();
	for (int index = 0; index < subsystemCount; index++ )
	{
		if (GetItemData(index) == subsystemKey)
		{
			SetItemText(index, COLUMN_INDEX_REPORT_CURRENTMODE, mode_ptr->getName().c_str());
			break;
		}
	}
	FUNCTION_EXIT;
}

void ReportListCtrl::UpdateExpectMode(TA_IRS_Core::Mode_SmartPtr& mode_ptr)
{
	FUNCTION_ENTRY("UpdateExpectMode");
	unsigned int subsystemKey = mode_ptr->getSubsystemKey();
	int subsystemCount = GetItemCount();
	for (int index = 0; index < subsystemCount; index++ )
	{
		if (GetItemData(index) == subsystemKey)
		{
			SetItemText(index, COLUMN_INDEX_REPORT_TARGETMODE, mode_ptr->getName().c_str());
			break;
		}
	}
	FUNCTION_EXIT;
}

void ReportListCtrl::UpdateExpectModeFail(unsigned long subsystemKey)
{
	FUNCTION_ENTRY("UpdateExpectModeFail");
	int subsystemCount = GetItemCount();
	for (int index = 0; index < subsystemCount; index++ )
	{
		if (GetItemData(index) == subsystemKey)
		{
			SetItemText(index, COLUMN_INDEX_REPORT_TARGETMODE, "");
			break;
		}
	}
	FUNCTION_EXIT;
}

void ReportListCtrl::UpdateCurrentModeFail(unsigned long subsystemKey)
{
    FUNCTION_ENTRY("UpdateExpectModeFail");
    int subsystemCount = GetItemCount();
    for (int index = 0; index < subsystemCount; index++ )
    {
        if (GetItemData(index) == subsystemKey)
        {
            SetItemText(index, COLUMN_INDEX_REPORT_CURRENTMODE, "");
            break;
        }
    }
    FUNCTION_EXIT;
}

void ReportListCtrl::UpdateAutoManual(unsigned long autoManal, unsigned long subsystemKey)
{
	FUNCTION_ENTRY("UpdateAutoManual");
	int subsystemCount = GetItemCount();
	for (int index = 0; index < subsystemCount; index++ )
	{
		if (GetItemData(index) == subsystemKey)
		{
			CString strText = getAutoManualStringByValue(autoManal);
			SetItemText(index, COLUMN_INDEX_REPORT_AUTOMANUAL, strText);
			break;
		}
	}
	FUNCTION_EXIT;
}

int ReportListCtrl::InsertRow(int nPos,int nNoOfCols, LPCTSTR pText, ...)
{
	va_list argList;
	va_start(argList, pText);
	int nIndex = InsertItem(LVIF_TEXT|LVIF_STATE, nPos, pText, 0, LVIS_SELECTED, 0, 0);
	if (nIndex < 0) 
	{
		return(nIndex);
	}

	for (int i = 1; i < nNoOfCols; i++) 
	{
		LPCSTR p = va_arg(argList,LPCSTR);
		if (p != NULL) 
		{
			SetItemText(nIndex, i, p);    
		}
	}

	va_end(argList);
	return(nIndex);
}

CString ReportListCtrl::getAutoManualStringByValue(unsigned long value)
{
	CString strStatus = "";
	strStatus = BASManagerUtility::autoCFGValueToString(value).c_str();
	return strStatus;
}

void ReportListCtrl::prepareChangeToLocation(unsigned long locationKey)
{
	DeleteAllItems();

	MapOrderToSubsystem subsystems = BasManagerDataHelperInstance::instance()->getAllSubsystems(locationKey, false);

	for (MapOrderToSubsystem::iterator it = subsystems.begin(); it != subsystems.end(); ++it)
	{
		// fill with LEFT subsystem list ctrl
		if (!it->second->getIsMerge())
		{
			int index = InsertItem(LVIF_TEXT|LVIF_STATE, GetItemCount(), it->second->getName().c_str(), 0, LVIS_SELECTED, 0, 0);
			SetItemData(index, it->second->getKey());
		}
	}

	for (int index = 0; index < GetItemCount(); index++)
	{
		SetItemText(index , COLUMN_INDEX_REPORT_TARGETMODE, "");
		SetItemText(index , COLUMN_INDEX_REPORT_CURRENTMODE, "");
		SetItemText(index , COLUMN_INDEX_REPORT_AUTOMANUAL, "");
	}
}
Namespace_TA_IRS_APP_End
