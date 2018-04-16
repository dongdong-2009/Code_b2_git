/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/system_control/banner/pages/call_page/src/CallBannerListCtrl.cpp $
  * @author Jade Lee
  * @version $Revision: #6 $
  *
  * Last modification: $DateTime: 2012/11/19 18:39:42 $
  * Last modified by: $Author: raghu.babu $
  * 
  * List control for displaying call banner items
  * 
  */


// Disable "identifier truncated" warning
#if defined(_MSC_VER)
#pragma warning(disable:4786 4284 4250)
#endif // _MSC_VER

#include "app\system_control\banner\pages\call_page\src\stdafx.h"
#include "app\system_control\banner\pages\call_page\src\CallBannerListCtrl.h"
#include "app\system_control\banner\pages\call_page\src\CallPage.h"
#include "bus\banner_pages\call_page_library\src\CallEntryManager.h"

// concrete class for CallContextMenu Inteface
// for context menu class
#include "app\system_control\banner\pages\call_page\src\ICallContextMenu.h"
#include "app\system_control\banner\pages\call_page\src\CallContextMenu.h"

#define INVALID_CALL_ID		-1

namespace TA_IRS_App
{
	const int CallBannerListCtrl::COL_STATUS = 5;

    CallBannerListCtrl::CallBannerListCtrl(int maxColumnCount)
    : m_calllistSortingInfo(maxColumnCount)
	, m_lastRightClickItem(-1)
	, m_callEntryManager(NULL)
    {
	
	}

	BEGIN_MESSAGE_MAP(CallBannerListCtrl, CListCtrl)
		ON_NOTIFY_REFLECT(NM_RCLICK, OnRButtonUp)
		ON_COMMAND(ID_REMOVE_CALL, onRemoveRadioCall)
		ON_COMMAND(ID_MAKE_CALL, onMakeRadioCall)
		ON_COMMAND(ID_RESET_ALL_PEC, OnResetAll)
		ON_COMMAND(ID_CLEAR_INVALID, OnClearInvalid)
		ON_NOTIFY_REFLECT ( NM_CUSTOMDRAW, OnCustomdrawMyList )
	END_MESSAGE_MAP()

    void CallBannerListCtrl::initialise(int columnIndex, TA_Base_Bus::ISortingCallback* callback)
    {
        m_calllistSortingInfo.activateSort(columnIndex);
        m_calllistSortingInfo.defineSortingSemantic(columnIndex, TA_Base_Bus::AutoSortListCtrl::BY_CALLBACK);
        m_calllistSortingInfo.setCallback(callback);
    }


    LRESULT CallBannerListCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
    {
        // Give call list control chance to sort itself
        TA_Base_Bus::AutoSortListCtrl::WindowProc(message, wParam, lParam, *this, m_calllistSortingInfo);    
    
        return ListCtrlSelNoFocus::WindowProc(message, wParam, lParam);
    }


	void CallBannerListCtrl::OnRButtonUp(NMHDR* pNMHDR, LRESULT* pResult)
	{
		LPNMITEMACTIVATE act = (LPNMITEMACTIVATE) pNMHDR;

		std::string status = "";
		
		if ( -1 != act->iItem) 
		{
			status = GetItemText(act->iItem, COL_STATUS ).GetString();
		} 
		else 
		{
			m_lastRightClickItem = -1;
		}

		if ( "Missed Call" == status ) 
		{
			m_lastRightClickItem = act->iItem;

			CRect rect;
			GetWindowRect(&rect);

			CMenu menu;
			menu.CreatePopupMenu();

			menu.AppendMenu(MF_ENABLED, ID_MAKE_CALL, "Make Call");
			menu.AppendMenu(MF_ENABLED,	ID_REMOVE_CALL, "Remove Call");

			CPoint mousePoint;
			::GetCursorPos(&mousePoint);

			menu.TrackPopupMenu(TPM_LEFTALIGN, mousePoint.x, mousePoint.y, this, rect);

			menu.DestroyMenu();
		}
		else
		{
			m_lastRightClickItem = act->iItem;
			if (0 != this->isValidToShowContextMenu())
			{
				showContextMenu ();
			}
		}

	}
	
	int CallBannerListCtrl::getLastRightClickItem()
	{
		return m_lastRightClickItem;
	}
	
	void CallBannerListCtrl::setCallEntryManager(TA_IRS_Bus::CallEntryManager* manager)
	{
		m_callEntryManager = manager;
	}

	void CallBannerListCtrl::onRemoveRadioCall()
	{
		int item = getLastRightClickItem();
		if ( -1 != item ) 
		{
			CString source;
			source = GetItemText(item, 2); // source string
			//unsigned long id = GetItemData(item);
			//std::stringstream stream;
			//stream << id;
			std::string callId((LPCTSTR) source);//stream.str();
			callId = callId.substr( callId.find("(")+1, callId.find(")")-callId.find("(")-1 );

			m_callEntryManager->removeRadioCallEntry(callId);
		}
	}

	void CallBannerListCtrl::onMakeRadioCall()
	{    
		int item = getLastRightClickItem();
		if ( -1 != item ) 
		{
			unsigned long id = GetItemData(item);

			TA_IRS_Bus::ICallEntry& callEntry =  m_callEntryManager->getCallEntry(id);

			callEntry.onActivate();
		}

	}
	
	void CallBannerListCtrl::OnResetAll()
	{
		// TODO: Add your command handler code here
		FUNCTION_ENTRY ("OnResetAll");

		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Test Context Menu : OnResetAll Event");
		
		int nId = getIdFromSelectionList();
		
		if (INVALID_CALL_ID != nId ) 
		{
			LOG_GENERIC (SourceInfo,
				TA_Base_Core::DebugUtil::DebugInfo,
				"Test Context Menu : Reseting ALL PEC with TRAIN ID %d", 
				nId);

			TA_IRS_Bus::ICallEntry& callEntry =  m_callEntryManager->getCallEntry(nId);

			LOG_GENERIC (SourceInfo,
				TA_Base_Core::DebugUtil::DebugInfo,
				"Test Context Menu : Calling CallEntry onForceReset ()");

			callEntry.onForceReset ();
			

			LOG_GENERIC (SourceInfo,
				TA_Base_Core::DebugUtil::DebugInfo,
				"Test Context Menu : Done Calling CallEntry onForceReset ()");
		}
		
		FUNCTION_EXIT;
	}
	
	void CallBannerListCtrl::showContextMenu ()
	{
		
		FUNCTION_ENTRY ("showContextMenu");
		// Instantiate CallContextMenu
		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Test Context Menu : Creating Object for Context Menu");
		
		// decide which item will be shown on the context menu
		std::vector <std::string> vctrMenu;
		TA_IRS_App::ICallContextMenu * contextMenu;

		if (m_callEntryManager->getCallEntry (getIdFromSelectionList ()).isCallInvalid())
		{
			vctrMenu.push_back("Clear");
			contextMenu = new TA_IRS_App::CallContextMenu (ID_CLEAR_INVALID,this,vctrMenu);
		}
		else
		{
			vctrMenu.push_back("Reset all");
			contextMenu = new TA_IRS_App::CallContextMenu (ID_RESET_ALL_PEC,this,vctrMenu);
		}

		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Test Context Menu : Created Object for Context Menu");
		// Instantiate CallContextMenu

		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Test Context Menu : RCLCK_EVENT, Showing ResetAll ContextMenu");

		contextMenu->show();

		if (NULL != contextMenu)
		{
			LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Test Context Menu: Deleting ContextMenu Object");

			delete contextMenu;
			contextMenu = NULL;

			LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Test Context Menu: Deleted ContextMenu Object");
		}

		FUNCTION_EXIT;
	}

	int CallBannerListCtrl::getIdFromSelectionList ()
	{
		FUNCTION_ENTRY ("getIdFromSelectionList");

		int nId = INVALID_CALL_ID;

		int item = getLastRightClickItem();

		if ( INVALID_CALL_ID != item ) 
		{
			nId = GetItemData(item);
		}
		
		LOG_GENERIC (SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					 "Selected Call Index ID : %d",nId);

		FUNCTION_EXIT;
		return nId;
	}

	bool CallBannerListCtrl::isValidToShowContextMenu ()
	{
		FUNCTION_ENTRY ("isValidToShowContextMenu");

		bool bIsValid = true;
		int nID = -1;

		if (0 == this->GetSelectedCount() 
			||
			INVALID_CALL_ID == (nID = getIdFromSelectionList ())
			)
		{
			bIsValid = false;
		}
		

 		if (bIsValid)
 		{
 			TA_IRS_Bus::ICallEntry& callEntry =  m_callEntryManager->getCallEntry(nID);
 			
			if (0 != callEntry.getCategory().compare("PEC"))
			{
				bIsValid = false;
			}
 		}
		
		FUNCTION_EXIT;
		
		LOG_GENERIC (SourceInfo,
					 TA_Base_Core::DebugUtil::DebugInfo,
					 "Can show contextmenu : %d",bIsValid);
		
		return bIsValid;	
	}

	// handles the reseting of invalid train, this call is one by one
	void CallBannerListCtrl::OnClearInvalid()
	{
		// TODO: Add your command handler code here
		FUNCTION_ENTRY ("OnClearInvalid");

		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Test Context Menu : OnClearInvalid Event");

		int nId = getIdFromSelectionList();

		if (INVALID_CALL_ID != nId ) 
		{
			
			TA_IRS_Bus::ICallEntry& callEntry =  m_callEntryManager->getCallEntry(nId);
			
			LOG_GENERIC (SourceInfo,
						TA_Base_Core::DebugUtil::DebugInfo,
						"Test Context Menu : Clearing invalid pec call @ Train : %d , ", 
						nId);

			callEntry.onClearCall ();

			LOG_GENERIC (SourceInfo,
						 TA_Base_Core::DebugUtil::DebugInfo,
						 "Test Context Menu : Cleared invalid pec call");
		}

		FUNCTION_EXIT;
	}

	void CallBannerListCtrl::OnCustomdrawMyList ( NMHDR* pNMHDR, LRESULT* pResult )
	{
		OnCustomDraw(pNMHDR,pResult);
	}

}      