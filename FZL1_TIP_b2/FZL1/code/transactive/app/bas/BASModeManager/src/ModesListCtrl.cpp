// ModesListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "app/bas/BASModeManager/src/BASManager.h"
#include "app/bas/BASModeManager/src/ModesListCtrl.h"
#include "app/bas/BASModeManager/src/ModeRunningStatus.h"
#include "app/bas/BASModeManager/src/BasManagerDataHelper.h"
#include "app/bas/BASModeManager/src/SubSystemStatus.h"
#include "core/data_access_interface/bas/src/ModeTypeAccessFactory.h"

Namespace_TA_IRS_APP_Begin
// ModesListCtrl

#define COLOR_RED RGB(255,0,0)
#define COLOR_GREEN RGB(0,255,0)
#define COLOR_YELLOW RGB(255,255,0)
#define COLOR_WHITE RGB(255,255,255)
#define COLOR_BLACK RGB(0,0,0)
IMPLEMENT_DYNAMIC(ModesListCtrl, BASBaseListCtrl)
ModesListCtrl::ModesListCtrl()
{
	m_isFreshed = false;
}

ModesListCtrl::~ModesListCtrl()
{
}


BEGIN_MESSAGE_MAP(ModesListCtrl, BASBaseListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_TIMER()
END_MESSAGE_MAP()


void ModesListCtrl::RefreshGUI(UINT nIDEvent)
{
	if (nIDEvent == ModeListCtrlColorTimer)
	{
		ModeStatusManager* modeStatusManager = ModeStatusManagerInstance::instance();
		m_isFreshed = !m_isFreshed;
		bool isInFireMode = BasManagerDataHelperInstance::instance()->getIsManagerInFireMode();

		for (int i = 0; i < GetItemCount(); i++)
		{
			unsigned long modeKey = static_cast<unsigned long>(GetItemData(i));
			std::string strStatus = GetItemText(i, COLUMN_INDEX_MODE_STATUS).GetBuffer();
			int modeStatus = ModeRunningStatusConfigManager::getModeStatusByName(strStatus);
			if (eModeStateUnknown != modeStatus) // reducing the refresh process in GUI
			{
				CRect itemRect;
				GetItemRect(i, &itemRect, LVIR_BOUNDS);
				InvalidateRect(&itemRect, FALSE);
				unsigned long modeKey = GetItemData(i);
				if (!modeStatusManager->isModeExecuteSuccessInTime(modeKey))
				{
					modeStatusManager->updateModeStatus(modeKey, eModeExecutionFail);
                    Mode_SmartPtr mode = BasManagerDataHelperInstance::instance()->getModeByKey(modeKey);
                    if (NULL != mode.get())
                    {
                        SubSystemStatusInstance::instance()->updateSubsystemModeTimeoutFlag(mode->getLocationKey(), mode->getSubsystemKey(), true);
                    }
				}
			}
			else if (isInFireMode)
			{
				CRect itemRect;
				GetItemRect(i, &itemRect, LVIR_BOUNDS);
				InvalidateRect(&itemRect, FALSE);
			}
		}
	}
}


void ModesListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLVCUSTOMDRAW pLVCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
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
			if (GetItemCount() > nItem)
            {
                *pResult = CDRF_NOTIFYSUBITEMDRAW;

                // Consider optimize the performance of string compare and getModeStatusByName
                static const std::string strFireMode = BasManagerDataHelperInstance::instance()->getTypeName(TA_IRS_Core::ModeTypeAccessFctory::modeTypeKey_Fire);
				unsigned long modeKey = static_cast<unsigned long>(GetItemData(nItem));
                std::string strStatus = GetItemText(nItem, COLUMN_INDEX_MODE_STATUS).GetBuffer();
                std::string strType = GetItemText(nItem, COLUMN_INDEX_MODE_TYPE).GetBuffer();
				int modeStatus = ModeRunningStatusConfigManager::getModeStatusByName(strStatus);

				if (!BasManagerDataHelperInstance::instance()->getIsManagerInFireMode())
				{
					if (m_isFreshed)
					{
						if (eModeExecutionSuccess == modeStatus)
                        {
                            if ( strFireMode == strType )
                            {
                                pLVCD->clrTextBk = COLOR_RED;
                            }
                            else
                            {
                                pLVCD->clrTextBk = COLOR_GREEN;
                            }
						}
						else if(eModeExecutionFail == modeStatus)
						{
							pLVCD->clrTextBk = COLOR_YELLOW;
						}
						else
						{
							pLVCD->clrTextBk = COLOR_WHITE;
						}
					}
					else
					{
						if (eModeExecutionFail == modeStatus)
						{
							pLVCD->clrTextBk = COLOR_YELLOW;
						}
						else if (eModeExecuting == modeStatus || eModeExecutionSuccess == modeStatus)
                        {
                            if ( strFireMode == strType )
                            {
                                pLVCD->clrTextBk = COLOR_RED;
                            }
                            else
                            {
                                pLVCD->clrTextBk = COLOR_GREEN;
                            }
						}
					}
				}
				else
				{
					if (m_isFreshed)
					{
						if (modeStatus == eModeExecutionSuccess)
						{
							pLVCD->clrTextBk = COLOR_GREEN;
						}
						else
						{
							pLVCD->clrTextBk = COLOR_RED;
						}						
					} 
					else
					{
						pLVCD->clrTextBk = COLOR_WHITE;
					}
				}
			}
		}
	}
}


void ModesListCtrl::UpdateModeSatus(TA_IRS_Core::Mode_SmartPtr& mode_ptr, int status)
{
	FUNCTION_ENTRY("UpdateModeSatus");
	unsigned long modeKey = mode_ptr->getKey();
	UpdateModeSatus(modeKey, status);
	FUNCTION_EXIT;
}

void ModesListCtrl::UpdateModeSatus(unsigned long modeKey, int status)
{
	FUNCTION_ENTRY("UpdateModeSatus");
	unsigned long listCtrlCount = static_cast<unsigned long>(GetItemCount());
	for(unsigned long index = 0; index < listCtrlCount; index++ )
	{
		if(GetItemData(index) == modeKey)
		{
			SetItemText(index, COLUMN_INDEX_MODE_STATUS, ModeRunningStatusConfigManager::getModeRunningStatusByType(status).c_str());

			// refresh this item
			CRect itemRect;
			GetItemRect(index, &itemRect, LVIR_BOUNDS);
			InvalidateRect(&itemRect, TRUE);
			break;
		}
	}
	FUNCTION_EXIT;
}
Namespace_TA_IRS_APP_End
