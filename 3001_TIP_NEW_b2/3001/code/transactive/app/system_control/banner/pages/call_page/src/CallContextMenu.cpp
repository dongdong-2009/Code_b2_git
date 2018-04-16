#include "StdAfx.h"
#include "CallContextMenu.h"

#include "core\utilities\src\DebugUtil.h"

namespace TA_IRS_App
{
	CallContextMenu::CallContextMenu(const UINT & nResourceID, 
									 CWnd * hWndTargetCtrl, 
									 const std::vector <std::string> & vctrMenu) : 
								m_nContextResourceId (nResourceID), 
								m_wndCtrl (hWndTargetCtrl),
								m_vctrMenu (vctrMenu)
	{	
		FUNCTION_ENTRY ("ID_RESET_ALL_PEC");
		FUNCTION_EXIT;
	}

	CallContextMenu::~CallContextMenu(void)
	{
		FUNCTION_ENTRY ("~ID_RESET_ALL_PEC");
		FUNCTION_EXIT;
	}

	void CallContextMenu::show ()
	{
		
		FUNCTION_ENTRY ("show");
		
		CRect rectSubmitButton;
		m_wndCtrl->GetWindowRect (&rectSubmitButton);

		CMenu menu;
		menu.CreatePopupMenu();
		
		for (int nCounter = 0; nCounter < m_vctrMenu.size(); nCounter++)
		{			
			menu.AppendMenu(MF_ENABLED, m_nContextResourceId,m_vctrMenu.at (nCounter).c_str());
		}
		

		CPoint mousePoint;
		::GetCursorPos(&mousePoint);

		menu.TrackPopupMenu(TPM_LEFTALIGN, mousePoint.x, mousePoint.y, m_wndCtrl, rectSubmitButton);

		menu.DestroyMenu();
		
		FUNCTION_EXIT;
	}
}	
