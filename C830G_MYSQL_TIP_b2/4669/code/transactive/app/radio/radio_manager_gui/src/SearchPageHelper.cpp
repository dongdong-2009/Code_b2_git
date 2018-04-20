/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/SearchPageHelper.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Provides necessary interface functions to allow use of CSearchPage
  *
  */

#pragma warning ( disable : 4284 )

#include "app/radio/radio_manager_gui/src/stdafx.h"

#include <vector>

#include "resource.h"
#include "app/radio/radio_manager_gui/src/SearchPageHelper.h"
#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioDirectoryCorbaDef.h"
#include "RadioManagerDlg.h"
#include "SelectedCall.h"

// Cyclical dependency of callback on callee (may want to phase this out later)
#include "bus\radio\radiosearchpage\src\searchpage.h"

namespace TA_IRS_App
{
    SearchPageHelper::SearchPageHelper()
    :
    m_searchPageWindow(0)
    {
    }


    SearchPageHelper& SearchPageHelper::getInstance()
    {
        static SearchPageHelper instance;
        return instance;
    }


    CRadioManagerDialog::IRadioDirectory& SearchPageHelper::getRadioDirectoryCorbaDef()
    {
        CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());
        return manager->getRadioDirectoryCorbaDef();
    }

	CRadioManagerDialog::IRadio& SearchPageHelper::getRadioCorbaDef()
    {
        CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());
        return manager->getRadioCorbaDef();
    }



    TA_IRS_Bus::MFCTrainSelectorGUI& SearchPageHelper::getTrainSelectorGUI()
    {
        CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());
        return manager->getTrainSelectorGUI();
    }


    TA_IRS_Bus::TrainSelector* SearchPageHelper::getTrainSelector()
    {
        CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());
        return &manager->getTrainSelector();
    }


    bool SearchPageHelper::isFunctionPermitted(DWORD globalRightIdentifier)
    {
        CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());
        return manager->isFunctionPermitted(globalRightIdentifier);
    }


    void SearchPageHelper::SetCall(CString& tsiNumber, CString& label, bool hotline, const ESelectionType& type)
    {
        TA_ASSERT(0 != CSelectedCall::GetInstance(), "Selected call instance is null");
        
        if (type == DYNAMIC_GROUP_ROOT)
        {
            // Dynamic group treated as a special case
            CSelectedCall::GetInstance()->SetDynamicGroupCall(label);
        }
        else
        {
            // All other subscribers may be treated the same
            CSelectedCall::GetInstance()->SetCall(tsiNumber, label, hotline);        
        }
    }


    void SearchPageHelper::addDynamicGroupMembers(std::vector<std::vector<CString> >* selectedSubscribers)
    {
        CWnd* tabCtrl =  AfxGetApp()->GetMainWnd()->GetDlgItem(IDC_CALL_STACK_TAB);
        CRadioCallStack* callStack = dynamic_cast<CRadioCallStack*>(tabCtrl);
        
        if (callStack != NULL)
        {
            callStack->addDynamicGroupMembers(selectedSubscribers);
        }
    }


    DWORD SearchPageHelper::getAddButtonRightIdentifier()
    {
        return TA_Base_Bus::aca_RADIO_DYNAMIC_GROUP;
    }


    DWORD SearchPageHelper::getEntityLocationKey()
    {
        CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());
        return manager->getLocationKey();
    }

	void SearchPageHelper::addMonitors(std::vector<std::vector<CString> >* selectedSubscribers)
	{
	}    
    
    bool SearchPageHelper::shouldDisplayDynamicGroupNode()
    {
        return true;
    }

    void SearchPageHelper::selectedTrainDetailsUpdated( CString& oldTsiNumber,
                                                        CString& oldServiceNumber,
                                                        CString& oldLocation,
                                                        CString& newTsiNumber,
                                                        CString& newServiceNumber,
                                                        CString& newLocation )
    {
        // update the selected tsi area if the trains old tsi was keyed in
        if (CSelectedCall::GetInstance()->GetTetraID() == oldTsiNumber)
        {
            CSelectedCall::GetInstance()->SetCall(newTsiNumber, newServiceNumber, false);
        }

        CWnd* tabCtrl =  AfxGetApp()->GetMainWnd()->GetDlgItem(IDC_CALL_STACK_TAB);
        CRadioCallStack* callStack = dynamic_cast<CRadioCallStack*>(tabCtrl);
        
        if (callStack != NULL)
        {
            callStack->updateExistingDynamicGroupMembers( oldTsiNumber, oldServiceNumber, oldLocation,
                                                          newTsiNumber, newServiceNumber, newLocation);
        }
    }


    void SearchPageHelper::removeDynamicGroupSubscriber(CString tsi)
    {
        if (0 != m_searchPageWindow && 0 != m_searchPageWindow->m_hWnd)
        {
            m_searchPageWindow->removeDynamicGroupSubscriber(tsi);
        }
    }


    void SearchPageHelper::setSearchPage(TA_IRS_Bus::CSearchPage& searchPageWindow)
    {
        m_searchPageWindow = &searchPageWindow;
    }


    void SearchPageHelper::addDynamicGroupSubscriber(CString subscriberName, CString tsi)
    {
        if (0 != m_searchPageWindow && 0 != m_searchPageWindow->m_hWnd)
        {
            m_searchPageWindow->processDynamicGroupSubscriberUpdate(subscriberName, tsi);
        }
    }
}
