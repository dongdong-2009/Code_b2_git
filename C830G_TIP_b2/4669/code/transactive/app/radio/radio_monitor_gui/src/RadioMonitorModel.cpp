/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_monitor_gui/src/RadioMonitorModel.cpp $
  * @author Jade Lee
  * @version $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/02/10 16:25:11 $
  * Last modified by: $Author: builder $
  * 
  * Contains business side of RadioMonitorDlg class
  *
  */


#include "app/radio/radio_monitor_gui/src/stdafx.h"
#include "app/radio/radio_monitor_gui/src/RadioMonitorModel.h"

#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164 
#include "bus/generic_gui/src/AppLauncher.h"
#include "bus/application_types/src/apptypes.h"
#include "core/exceptions/src/ApplicationException.h"

namespace TA_IRS_App
{
    RadioMonitorModel::RadioMonitorModel()
    :
    m_isCurrentlyMonitoring(false)
    {
    }

    
    std::set<std::string> RadioMonitorModel::requestOneShotMonitoring(const TSISequence& tsiList)
    {
        std::set<std::string> successfulRequests;

        try
        {
            bool continueRequested = false;
           
            for (TSISequence::const_iterator itr = tsiList.begin(); itr != tsiList.end(); itr ++)
            {
                try
                {
                    CORBA_CALL( getRadioSession(), getActiveCalls,
                                ( itr->c_str(), CachedConfig::getInstance().getSessionId().c_str() ) );//limin

                    successfulRequests.insert(std::set<std::string>::value_type(*itr));                                   
                }
                catch (const TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException&)
                {
                    if (continueRequested)
                    {
                        // User requested a continue, ignore subsequent errors
                        continue;
                    }

                    // Upon Radio API Failure 
                    TSISequence::const_iterator endItr = itr;
                    endItr ++;

                    if (tsiList.end() == endItr)
                    {
                        // No more items remaining, simple warning will do
                        warnUser(ONE_SHOT_FAIL);
                    }
                    else
                    {
                        // need to ask if want to continue for remainder items
                        if (!promptForContinue(ONE_SHOT_FAIL))
                        {
                            break;
                        }
                        continueRequested = true;
                    }                                            
                }
            }
        }
        DEFAULT_RM_CATCH_HANDLER("requestOneShotMonitoring failure");        

        return successfulRequests;
    }


    void RadioMonitorModel::requestStartMonitoring(const TSISequence& tsiList, bool enterMonitoringState)
    {
        try
        {
            bool continueRequested = false;

            for (TSISequence::const_iterator itr = tsiList.begin(); itr != tsiList.end(); itr ++)
            {
                try
                {
                    CORBA_CALL( getRadioSession(), startMonitoring, (
                            itr->c_str(), 
                            CachedConfig::getInstance().getSessionId().c_str() ) );//limin
                                               
                    // Store in actively monitored list
                    m_itemsBeingMonitored.insert(std::set<std::string>::value_type(*itr));
                }
                catch (const TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException&)
                {
                    if (continueRequested)
                    {
                        // User requested a continue, ignore subsequent errors
                        continue;
                    }

                    // Upon Radio API Failure 
                    TSISequence::const_iterator endItr = itr;
                    endItr ++;

                    if (tsiList.end() == endItr)
                    {
                        // No more items remaining, simple warning will do
                        warnUser(START_MONITOR_FAIL);
                    }
                    else
                    {
                        // need to ask if want to continue for remainder items
                        if (!promptForContinue(START_MONITOR_FAIL))
                        {
                            break;
                        }
                        
                        continueRequested = true;
                    }                           
                }
                           
                if (enterMonitoringState)
                {
                    // Enter monitoring state as soon as one request is successful
                    m_isCurrentlyMonitoring = true;
                }
            }
        }
        DEFAULT_RM_CATCH_HANDLER("requestStartMonitoring failure");        
    }


    void RadioMonitorModel::requestStopMonitoring(const TSISequence& tsiList, bool leaveMonitoringState)
    {        
        try
        {
            bool continueRequested = false;

            for (TSISequence::const_iterator itr = tsiList.begin(); itr != tsiList.end(); itr ++)
            {
                try
                {
                    CORBA_CALL( getRadioSession(), stopMonitoring, (
                        itr->c_str(), 
                        CachedConfig::getInstance().getSessionId().c_str() ) );//limin

                    // May remove from actively monitored list on success
                    m_itemsBeingMonitored.erase(*itr);
                }
                catch (const TA_Base_Bus::IRadioSessionCorbaDef::invalidRequestException&)
                {
                    // Assume this means call has terminated, or has already been stopped
                    // (if there is a failure to stop monitoring half way through a request
                    // we can get into a state where some of the monitors have already stopped,
                    // so need to gracefully handle this case..)
                    
                    // May remove from actively monitored list on success
                    m_itemsBeingMonitored.erase(*itr);                
                }
                catch (const TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException&)
                {
                    if (continueRequested)
                    {
                        // User requested a continue, ignore subsequent errors
                        continue;
                    }

                    // Radio API failure - indicate to user there has been a failure,
                    TSISequence::const_iterator endItr = itr;
                    endItr ++;

                    if (tsiList.end() == endItr)
                    {
                        // No more items remaining, simple warning
                        warnUser(STOP_MONITOR_FAIL);
                    }
                    else
                    {
                        // need to ask if want to continue for remainder items
                        if (!continueRequested && !promptForContinue(STOP_MONITOR_FAIL))
                        {
                            break;
                        }

                        continueRequested = true;
                    }                    
                }
            }

            // Leave monitoring state once all calls successfully cancelled
            if (leaveMonitoringState)
            {
                m_isCurrentlyMonitoring = false;
            }
        }
        DEFAULT_RM_CATCH_HANDLER("requestStopMonitoring failure");        
    }


    void RadioMonitorModel::requestStartListening(long callId)
    {
        try
        {
            CORBA_CALL( getRadioSession(), startListening, (
                        callId, 
                        CachedConfig::getInstance().getSessionId().c_str() ) );//limin
        }
        DEFAULT_RM_CATCH_HANDLER("requestStartListening failure");       
    }


    void RadioMonitorModel::requestStopListening(long callId)
    {
        try
        {
            CORBA_CALL( getRadioSession(), stopListening, (
                        callId, 
                        CachedConfig::getInstance().getSessionId().c_str() ) );//limin
        }
        DEFAULT_RM_CATCH_HANDLER("requestStopListening failure");        
    }



    void RadioMonitorModel::requestEndCall(long callId)
    {
        try
        {
            CORBA_CALL( getRadioSession(), terminateCall, (
                        callId, 
                        CachedConfig::getInstance().getSessionId().c_str(), true ) );//limin // TD14247
        }
        DEFAULT_RM_CATCH_HANDLER("requestEndCall failure");     
    }



    void RadioMonitorModel::requestJoinCall(long callId)
    {
        try
        {
            CORBA_CALL( getRadioSession(), joinCall, (
                        callId, 
                        CachedConfig::getInstance().getSessionId().c_str() ) );//limin

            // The joinCall request will cause the agent to have the radio
            // ring us - so we need to launch the radio manager (and have it
            // automatically pick up this call..)
            launchRadioManagerForCallPickup(callId);                        
        }
        DEFAULT_RM_CATCH_HANDLER("requestJoinCall failure");        
    }

    
    void RadioMonitorModel::forceMonitorSetRemoval(const TSISequence& tsiList)
    {
        for (TSISequence::const_iterator itr = tsiList.begin(); itr != tsiList.end(); itr ++)
        {
            m_itemsBeingMonitored.erase(*itr);
        }
    }


    void RadioMonitorModel::launchRadioManagerForCallPickup(long callId)
    {
        try
        {
            std::ostringstream ostrCommandLine;
            ostrCommandLine << "--entity-name=RadioManager";
            ostrCommandLine << " --call-id=" << callId;

            // These default parameters taken from MainNavigator.gdf
            unsigned long posType = TA_Base_Bus::TA_GenericGui::POS_BOUNDED | 
                                    TA_Base_Bus::TA_GenericGui::POS_RELATIVE | 
                                    TA_Base_Bus::TA_GenericGui::POS_RESIZABLE;
            unsigned long alignType = TA_Base_Bus::TA_GenericGui::ALIGN_FIT;
                
            RECT mainRect;
            // Determine left position of target window
            AfxGetMainWnd()->GetWindowRect(&mainRect);
            
            RECT* objectDim = 0;
            
            // Obtain dimensions we need to place PA Manager at
            RECT screenDim = TA_Base_Bus::AppLauncher::getInstance().getRect(TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT,
                                                                         TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC, 
                                                                         mainRect.left);
    
            TA_Base_Bus::AppLauncher::getInstance().launchApplication(RADIO_CONTROLLER_GUI_APPTYPE, 
                                                                 ostrCommandLine.str(),
                                                                 posType,
                                                                 alignType,
                                                                 objectDim,
                                                                 &screenDim);

        }
        catch (const TA_Base_Core::ApplicationException&)
	    {
		    LOG(SourceInfo,
                TA_Base_Core::DebugUtil::ExceptionCatch, 
                "ApplicationException", 
			    "Could not launch Radio Manager App.");    
                        
            RadioMonitorErrorHandler::getInstance().displayError(
                    RadioMonitorErrorHandler::GENERIC_REQUEST_FAILURE);  
	    }
        catch (...)
        {
            LOG(SourceInfo,
                 TA_Base_Core::DebugUtil::ExceptionCatch, 
                 "Unknown exception launching application", 
                 "Could not launch Radio Manager App.");   
                
            RadioMonitorErrorHandler::getInstance().displayError(
                    RadioMonitorErrorHandler::GENERIC_REQUEST_FAILURE);  
        }
    }


    RadioMonitor::IRadioSession& RadioMonitorModel::getRadioSession()
    {
        return CachedConfig::getInstance().getRadioSessionCorbaDef();
    }


    void RadioMonitorModel::warnUser(EErrorCode errorCode)
    {
        std::string errorStr;

		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString actionName;
		// ++ TD14164 

        switch (errorCode)
        {
        case START_MONITOR_FAIL:
			// TD14164 ++
            /*errorStr = "Errors encountered while executing start monitoring request.";*/
			actionName = "start";
			// ++ TD14164 
            break;
        case ONE_SHOT_FAIL:
			// TD14164 ++
            /*errorStr = "Errors encountered while executing one shot monitoring request.";*/
			actionName = "one shot";
			// ++ TD14164 
            break;
        case STOP_MONITOR_FAIL:
			// TD14164 ++
            /*errorStr = "Errors encountered while executing stop monitoring request.";*/
			actionName = "stop";
			// ++ TD14164 
            break;
        default:
            TA_ASSERT(FALSE, "Unhandled switch handler");
        };

		// TD14164 ++
        /*MessageBox(0, errorStr.c_str(), "Warning", 0);*/
		userMsg << actionName;
		userMsg.showMsgBox(IDS_UE_630013);
		// ++ TD14164 
    }


    bool RadioMonitorModel::promptForContinue(EErrorCode errorCode)
    {
        std::string errorStr;

		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString actionName;
		// ++ TD14164 

        switch (errorCode)
        {
        case START_MONITOR_FAIL:
			// TD14164 ++
            /*errorStr = "Errors encountered while executing start monitoring request.\nContinue trying for remaining items?";*/
			actionName = "start";
			// ++ TD14164 
            break;
        case ONE_SHOT_FAIL:
			// TD14164 ++
            /*errorStr = "Errors encountered while executing one shot monitoring request.\nContinue trying for remaining items?";*/
			actionName = "one shot";
			// ++ TD14164 
            break;
        case STOP_MONITOR_FAIL:
			// TD14164 ++
            /*errorStr = "Errors encountered while executing stop monitoring request.\nContinue trying for remaining items?";*/
			actionName = "stop";
			// ++ TD14164 
            break;
        default:
            TA_ASSERT(FALSE, "Unhandled switch handler");
        };

		// TD14164 ++
		userMsg << actionName;
		UINT selectedButton = userMsg.showMsgBox(IDS_UE_630014);
        /*return IDYES == MessageBox(0, errorStr.c_str(), "Warning", MB_YESNO);*/
		return IDYES == selectedButton;
		// ++ TD14164 
    }


} // end namespace TA_IRS_App
