/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/history/history_viewer/src/HistoryCommsSubscriber.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#include "stdafx.h"
#include "HistoryCommsSubscriber.h"
#include "DebugUtil.h"
#include "app/history/scada_historian/scada_historian_dmif/src/ScadaHistorianProcessTimeUpdateCorbaDef.h"


using namespace TA_Core;


namespace TA_App
{

    void HistoryCommsSubscriber::receiveSpecialisedMessage(const CommsMessageCorbaDef& message)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "receiveSpecialisedMessage" );
        try
        {
            // Extract the timestamp from the Comms Message
	        // Extract to updateTime
            TA_App::ScadaHistorianProcessTimeUpdateCorbaDef* updateTimeData = NULL;

            if( ( message.messageState >>= updateTimeData ) != 0 )
            {
                long* entitykey = new long(updateTimeData->entitykey);
                time_t* timestamp = new time_t(updateTimeData->processtime);

                LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                    "Received HistoryUpdate message with entity key: %lu and timestamp: %lu", 
                    *entitykey, *timestamp );

	            // Tell the app that a History Update message has been received
                ::PostMessage( AfxGetApp()->GetMainWnd()->m_hWnd, WM_HISTORY_UPDATE, reinterpret_cast<WPARAM>(entitykey), reinterpret_cast<LPARAM>(timestamp) );
            }
	        else
            {
                LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugInfo, "Event does not contain updateTimeData");
			    return;
            }
        }
        catch (...)
        {
	        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugFatal, "Exception thrown while receiving event");

            // ANITA: Redo
            //GenericGUI::CGenericGUIManagedProcess::GetInstance()->UpdateStatus(nexus::ProcessManagerDmIf::DependencyFail);
        }

        LOG ( SourceInfo, DebugUtil::FunctionExit, "receiveSpecialisedMessage" );
    }
}   // TA_History