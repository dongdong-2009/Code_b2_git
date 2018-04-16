/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/trains/train_agent/src/TrainAlarm.cpp $
  * @author:  Glen Kidd
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class manages the PublicAdress commands.  It handles the sequencing
  * of the commands and error checking
  *
  */

///////////////////////////////////////////////////////////
//  TrainAlarm.h
//  Implementation of the Class TrainPublicAdress
//  Created on:      21-Jan-2004
///////////////////////////////////////////////////////////

#include "TrainAlarm.h"
#include "ITrainManager.h"
#include "app/trains/train_agent/src/TARethrow.h"
#include "app/trains/train_agent/src/TrainException.h"
#include "app/trains/train_agent/src/TrainActivityMonitor.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/TrainEvent.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/misc0A1TableEventReader.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/miscATCAlarmEventReader.h"
#include "core/utilities/src/TAAssert.h"



namespace TA_IRS_App {


    
    
    TrainAlarm::TrainAlarm(ITrainManager& callback, ITrainNotificationFacade& notifier,
						   OperationModeManager& operationModeManager,
						   TrainActivityMonitor& trainActivityMonitor) 
        : m_trainManager(callback),
        m_notifier(notifier),
		m_operationModeManager(operationModeManager),
		m_trainActivityMonitor(trainActivityMonitor)
    {
        m_trainManager.addDelegate(this, true);
    }
    
    
    TrainAlarm::~TrainAlarm()
    {
        
    }


    bool TrainAlarm::delegate(TrainEvent& event)
    {
        TrainEventReader* r = event.getReader();
        bool consumed = true;
        if (dynamic_cast<misc0A1TableEventReader*>(r) != 0)
        {
            onOA1Table(event, dynamic_cast<misc0A1TableEventReader*>(r));
        }
        else if (dynamic_cast<miscATCAlarmEventReader*>(r) != 0)
        {
            onATCAlarm(event, dynamic_cast<miscATCAlarmEventReader*>(r));
        }
        else 
        {
            consumed = false;
        }
        return consumed;
    }

    
    void TrainAlarm::onOA1Table(TrainEvent & event, misc0A1TableEventReader * reader)
    {
		FUNCTION_ENTRY("getStatusAlarmTable");
        unsigned int trainID = event.getTrainID();

		if (m_operationModeManager.isInControlOfTrain(trainID))
		{
			m_trainActivityMonitor.oa1Received(trainID);
		}
		else
		{
			// Dodgy - should broadcast special message that says "someone take this train"
			TrainStateUpdateObserver tsuoTSI(m_trainManager.getTrainStatusManager(), trainID, syncTSI);
			tsuoTSI.synchronise(); // Just here as a precaution - don't want this optimised out
		}

		std::vector<unsigned char> table = reader->getCriticalAlarmTable();
		m_notifier.notifyATSofOa1(trainID, table);
        
		FUNCTION_EXIT;
    }
    
    void TrainAlarm::onATCAlarm(TrainEvent & event, miscATCAlarmEventReader * reader)
    {
		FUNCTION_ENTRY("getStatusAlarmTable");
        unsigned int trainID = event.getTrainID();

		if (m_operationModeManager.isInControlOfTrain(trainID))
		{
			m_trainActivityMonitor.atcReceived(trainID);
		}
		else
		{
			// Dodgy - should broadcast special message that says "someone take this train"
			TrainStateUpdateObserver tsuoTSI(m_trainManager.getTrainStatusManager(), trainID, syncTSI);
			tsuoTSI.synchronise(); // Just here as a precaution - don't want this optimised out
		}

		std::vector<unsigned char> table = reader->getStatusAlarmTable();
		m_notifier.notifyATSofAtc(trainID, table);
        
		FUNCTION_EXIT;
    }    

}  // namespace
