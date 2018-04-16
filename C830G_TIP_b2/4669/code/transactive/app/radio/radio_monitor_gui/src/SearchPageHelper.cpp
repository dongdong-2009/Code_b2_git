/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_monitor_gui/src/SearchPageHelper.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Provides necessary interface functions to allow use of CSearchPage
  *
  */


#include "app/radio/radio_monitor_gui/src/stdafx.h"

#include <vector>

#include "app/radio/radio_monitor_gui/src/SearchPageHelper.h"
#include "app/radio/radio_monitor_gui/src/RadioMonitorDlg.h"

#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"
#include "bus/trains/TrainSelectionComponent/src/MFCTrainSelectorGUI.h"

namespace TA_IRS_App
{
    SearchPageHelper::SearchPageHelper()
    :
    m_trainSelectorGUI(TRAIN_INBOUND_MSG, TRAIN_OUTBOUND_MSG, TRAIN_UPDATE_SELECTION_MSG),
    m_trainSelector(0)
    {
    }


    SearchPageHelper& SearchPageHelper::getInstance()
    {
        static SearchPageHelper instance;
        return instance;
    }
    
	TA_IRS_Bus::ISearchPageHelper::IRadio& SearchPageHelper::getRadioCorbaDef()
	{
		return CachedConfig::getInstance().getRadioCorbaDef();
	}

    TA_IRS_Bus::ISearchPageHelper::IRadioDirectory& SearchPageHelper::getRadioDirectoryCorbaDef()
    {
        return CachedConfig::getInstance().getRadioDirectoryCorbaDef();
    }


    TA_IRS_Bus::MFCTrainSelectorGUI& SearchPageHelper::getTrainSelectorGUI()
    {
        initialiseTrainSelector();

        return m_trainSelectorGUI;
    }


    TA_IRS_Bus::TrainSelector* SearchPageHelper::getTrainSelector()
    {
        initialiseTrainSelector();

        return m_trainSelector;
    }


    void SearchPageHelper::initialiseTrainSelector()
    {
        if (0 == m_trainSelector)
        {
            try
            {                
                unsigned long depotLocationKey = TA_IRS_Bus::LocationCache::getInstance().getDepotLocationKey();
                unsigned long occLocationKey = TA_IRS_Bus::LocationCache::getInstance().getOccLocationKey();                        
                TA_IRS_Bus::LocationCache::EStationType stationType;
                
                if (CachedConfig::getInstance().getPhysicalLocationKey() == depotLocationKey)
                {
                    stationType = TA_IRS_Bus::LocationCache::Depot;
                }
                else if (CachedConfig::getInstance().getPhysicalLocationKey() == occLocationKey)
                {
                    stationType = TA_IRS_Bus::LocationCache::InService;
                }
                else
                {
                    stationType = TA_IRS_Bus::LocationCache::Station;
                }
        	
    	        m_trainSelector = new TA_IRS_Bus::TrainSelector(m_trainSelectorGUI,
                                                CachedConfig::getInstance().getPhysicalLocationKey(), 
                                                stationType);
            }
            catch (...)
            {
    
            }    
        }
    }


    bool SearchPageHelper::isFunctionPermitted(DWORD globalRightIdentifier)
    {
        return CachedConfig::getInstance().isFunctionPermitted(globalRightIdentifier);            
    }


    void SearchPageHelper::SetCall(CString& tsiNumber, CString& label, bool hotline, const TA_IRS_Bus::ISearchPageHelper::ESelectionType& type)
    {
        // This does nothing - we don't care which call is currently selected
    }


    void SearchPageHelper::addDynamicGroupMembers(std::vector<std::vector<CString> >* selectedSubscribers)
    {
		// This does nothing - we don't care this info
    }
    
	void SearchPageHelper::addMonitors(std::vector<std::vector<CString> >* selectedSubscribers)
	{
        TA_ASSERT(0 != selectedSubscribers, "Input was NULL");
		
        RadioMonitorDlg* radioDlg =  dynamic_cast<RadioMonitorDlg*>(AfxGetMainWnd());
		
        if (0 != radioDlg)
        {
            // Define the fields, in order
            enum { ID, LOCATION, TSI };
			
            std::vector< RadioMonitorDlg::SelectedEntity > entities;
			
            // Convert data into appropriate format
            for (unsigned int i = 0; i < selectedSubscribers->size(); i ++)
            {
                RadioMonitorDlg::SelectedEntity entity;
                try
                {
                    std::vector<CString>& dataArray = selectedSubscribers->at(i);
					
                    entity.id = (LPCSTR)dataArray.at(ID);
                    entity.location = (LPCSTR)dataArray.at(LOCATION);
                    entity.tsi = (LPCSTR)dataArray.at(TSI);
					
                    entities.push_back(entity);
                }
                catch (const std::out_of_range&)
                {
                    RadioMonitorErrorHandler::getInstance().displayError(
						RadioMonitorErrorHandler::INTERNAL_FAILURE);
                }                
            }
			
            radioDlg->addToSelectionList(entities);
        }
        
        // Our responsibility to clean this up
        delete selectedSubscribers;
	}


    DWORD SearchPageHelper::getAddButtonRightIdentifier()
    {
        return TA_Base_Bus::aca_RADIO_MONITOR_CALLS;
    }


    DWORD SearchPageHelper::getEntityLocationKey()
    {
        return CachedConfig::getInstance().getEntityLocationKey();
    }


    bool SearchPageHelper::shouldDisplayDynamicGroupNode() 
    { 
        return false; 
    }

    void SearchPageHelper::selectedTrainDetailsUpdated( CString& oldTsiNumber,
                                                        CString& oldServiceNumber,
                                                        CString& oldLocation,
                                                        CString& newTsiNumber,
                                                        CString& newServiceNumber,
                                                        CString& newLocation )
    {
        // called when train details are updated in the search page.
        // refresh any other GUI elements here.
    }

}
