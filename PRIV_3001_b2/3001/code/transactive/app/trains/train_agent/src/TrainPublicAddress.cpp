/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/trains/train_agent/src/TrainPublicAddress.cpp $
  * @author:  Glen Kidd
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This class manages the PublicAdress commands.  It handles the sequencing
  * of the commands and error checking
  *
  * TODO: session management 
  */

///////////////////////////////////////////////////////////
//  TrainCCTV.h
//  Implementation of the Class TrainPublicAdress
//  Created on:      21-Jan-2004
///////////////////////////////////////////////////////////

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "TrainPublicAddress.h"
#include "ITrainManager.h"
#include "TrainManager.h"
#include "app/trains/train_agent/src/TARethrow.h"
#include "app/trains/train_agent/src/TrainTimer.h"
#include "app/trains/train_agent/src/TrainException.h"
#include "app/trains/train_agent/src/OperationModeManager.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/TrainEvent.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/paPreRecordedAnnouncement.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/paContinueCommand.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/paCyclicAnnouncementCommand.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/paLiveAnnouncement.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/paResetCommand.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/paStatus.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/paCommandReceivedEventReader.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/paReadyForLiveAnnouncementEventReader.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/paRequestResetEventReader.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/paResetEventReader.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/paTrainMessageCompletedEventReader.h"
#include "core/data_access_interface/entity_access/src/TrainSettingsEntityData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/message/types/TrainAudit_MessageTypes.h"


// helper function in anonymous namespace
namespace {
    using namespace TA_IRS_App;

    void checkBusy(paCommandReceivedEventReader* reader)    
    {
        switch (reader->getCommandReceivedStatus())
        {
        case paCommandReceivedEventReader::PA_PROCESSING:
            break;
        case paCommandReceivedEventReader::PA_IN_USE:
            TA_THROW(TrainBusyException("PA in use, cannot override", false));
            break;
        case paCommandReceivedEventReader::PA_IN_USE_OVERRIDE:
            TA_THROW(TrainBusyException("PA in use, may be overriden", true));
            break;
        case paCommandReceivedEventReader::PA_IN_USE_CYCLIC_CANCELLED:
            // cyclic cancel throws same exception as in-use
            TA_THROW(TrainBusyException("PA Cyclic cancelled, cannot override", false));
            break;
        case paCommandReceivedEventReader::PA_RESET_COMMAND:
            TA_THROW(TrainGeneralFailureException("", TrainGeneralFailureException::RESET));
            break;
        case paCommandReceivedEventReader::PA_FAILED:
            TA_THROW(TrainGeneralFailureException("", TrainGeneralFailureException::COMMAND_FAILED));
            break;
        case paCommandReceivedEventReader::PA_DIFF_ORIGIN:
            TA_THROW(TrainGeneralFailureException());
            break;
        case paCommandReceivedEventReader::PA_DIFF_ID:
            TA_THROW(TrainGeneralFailureException());
            break;
        case paCommandReceivedEventReader::DIFFERENT_VERSION_NUMBER:
            TA_THROW(TrainGeneralFailureException("",TrainGeneralFailureException::DIFF_VERSION));
            break;
        }
    }
}

namespace TA_IRS_App {
  
    
    TrainPublicAddress::TrainPublicAddress( ITrainManager& callback,
                                            ITrainNotificationFacade& notifier,
                                            TrainWorkQueueManager& trainWorkQueueManager) 
        : m_trainManager(callback),
          m_notifier(notifier),
          m_workQueueManager(trainWorkQueueManager),
          m_pa3TimerManager(*this)
    {
        m_trainManager.addDelegate( dynamic_cast<ITrainEventReaderDelegator*>(this) );
        m_trainManager.addDelegate( dynamic_cast<ITrainCallEndDelegator*>(this), TrainStatus::Resource_PA );
        

        /// @todo Get PAx Timer values (from TrainAgent entity data ?!?)
//        m_retryLimit = 3;
        m_timerPA1 = m_trainManager.getGlobalTrainAgentEntity()->getTimerPA1();
        m_timerPA2 = m_trainManager.getGlobalTrainAgentEntity()->getTimerPA2();
        m_timerPA3 = m_trainManager.getGlobalTrainAgentEntity()->getTimerPA3();
        m_timerPA4 = m_trainManager.getGlobalTrainAgentEntity()->getTimerPA4();
    }
    
    
    TrainPublicAddress::~TrainPublicAddress()
    {
        
    }
    
    bool TrainPublicAddress::delegate(TrainEvent& event)
    {
        TrainEventReader* r = event.getReader();
        bool consumed = true;
        if (dynamic_cast<paReadyForLiveAnnouncementEventReader*>(r) != 0)
        {
            onPAReadyForLiveAnnouncement(event, dynamic_cast<paReadyForLiveAnnouncementEventReader*>(r));
        }
        else if (dynamic_cast<paRequestResetEventReader*>(r) != 0)
        {
            onPARequestReset(event, dynamic_cast<paRequestResetEventReader*>(r));
        }
        else if (dynamic_cast<paResetEventReader*>(r) != 0)
        {
            onPAReset(event, dynamic_cast<paResetEventReader*>(r));
        }
        else if (dynamic_cast<paTrainMessageCompletedEventReader*>(r) != 0)
        {
            onPATrainMessageCompleted(event, dynamic_cast<paTrainMessageCompletedEventReader*>(r));
        }
        else 
        {
            consumed = false;
        }
        return consumed;
    }

    bool TrainPublicAddress::delegate(unsigned long callId, std::vector<unsigned char>& trains)
    {
        // A PA live call has been disconnected

        // all trains sharing a call share the same announcement ID
        int announcementId =
            m_trainManager.getTrainStatusManager().getPAAnnouncementID(trains[0]);

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
            "Audio lost for PA live announcement id %d, cleaning up",
            announcementId );

        // clean up - end the broadcast for all trains
        // send the commands to the trains
        for (std::vector<unsigned char>::const_iterator iter = trains.begin();
            iter != trains.end(); ++iter)
        {
            std::string sessionID =
                m_trainManager.getTrainStatusManager().getOwningSessionID(*iter, TrainStatus::Resource_PA);

            PaEndLiveAnnouncementCommand *x =
                new PaEndLiveAnnouncementCommand( *this,
                                                  m_notifier,
                                                  *iter,
                                                  announcementId,
                                                  sessionID);
            m_workQueueManager.perform(x);
        }
        
        // send a global failure message
        m_notifier.notifyPublicAddressAnnouncement(
            PA_CallLostAnnouncement, announcementId);

        return true;
    }
    
    void TrainPublicAddress::paPreRecordedAnnouncement(unsigned char trainID,
        bool override, 
        unsigned char tableOffset, 
        unsigned short libraryVersion, 
        unsigned char announcementID,
        std::string sessionID) 
    {

		TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncPublicAddress);
        
		// attempt to own the session
        m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_PA, sessionID);    

        // Send command to train
        TA_IRS_App::paPreRecordedAnnouncement command; // ?!? won't compile without explicit namespace
        
        command.setTrainID(trainID);
        if (override) 
        {
            command.setStatus( static_cast<paPreRecordedAnnouncement::EPAAnnouncementStatus>(OVERRIDE), 
                announcementID);
        } 
        else
        {
            command.setStatus( static_cast<paPreRecordedAnnouncement::EPAAnnouncementStatus>(REQUEST),
                announcementID);
        }
        command.setIdentifierOffset(tableOffset);
        command.setLibraryVersion(libraryVersion);
        
        try 
        {
            // this call can throw a TrainMessageTimeoutException, but the idl has a 
            // IPATrainCommandsCorbaDef::TrainTimeout defined - should we convert the exception???
            TrainEventPtr event = m_trainManager.sendTrainCommandWithWait(trainID, command, m_timerPA1);
            
            // Check event reply
            paCommandReceivedEventReader* reader = dynamic_cast<paCommandReceivedEventReader*>(event->getReader());
            if (0 == reader)
            {
                TA_THROW( TrainGeneralFailureException() );
            }
    
            // check validity of received message - command matches; 
            if (paCommandReceivedEventReader::PA_PRE_RECORDED_ANNOUNCEMENT != 
                reader->getOriginalCommand())
            {
                TA_THROW(TrainInvalidParameterException());
            }

            // busy?
            checkBusy(reader);

            // update train status 
            m_trainManager.getTrainStatusManager().setPALibraryVersion(trainID, reader->getLibraryVersion());

        }
        catch (TrainException & e) 
        {
	        m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_PA, "", true);
            TA_RETHROW( e ); 
        }
        m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_PA, "", true);
        
    }
    
    
    void TrainPublicAddress::paCyclicAnnouncement(
        unsigned char trainID,
        bool override,
        unsigned char tableOffset,
        unsigned short libraryVersion,
        const PATimeInterval& announcementPeriod,
        unsigned char announcementID,
        std::string sessionID)
    {
		TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncPublicAddress);

		// attempt to own the session
        m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_PA, sessionID);    

        // Send command to train
        paCyclicAnnouncementCommand command; 
        
        command.setTrainID(trainID);
        if (override) 
        {
            command.setStatus( static_cast<EPAAnnouncementStatus>(OVERRIDE), 
                announcementID);
        } 
        else
        {
            command.setStatus( static_cast<EPAAnnouncementStatus>(REQUEST),
                announcementID);
        }
        command.setIdentifierOffset(tableOffset);
        command.setLibraryVersion(libraryVersion);
        command.setStartTime(announcementPeriod.startHour, announcementPeriod.startMin);
        command.setStopTime(announcementPeriod.stopHour, announcementPeriod.stopMin);
        
        try
        {
            TrainEventPtr event = m_trainManager.sendTrainCommandWithWait(trainID, command, m_timerPA1);
            
            // Check event reply
            paCommandReceivedEventReader* reader = dynamic_cast<paCommandReceivedEventReader*>(event->getReader());
            if (0 == reader)
            {
                TA_THROW( TrainGeneralFailureException() );
            }
            
            // check validity of received message - command matches; 
            if (paCommandReceivedEventReader::ATAS_CYCLIC_ANNOUNCEMENT != 
                reader->getOriginalCommand())
            {
                TA_THROW(TrainInvalidParameterException());
            }

            // ss++
            // TD13952
            try
            {
                checkBusy(reader);
            }
            catch(TrainBusyException& e)
            {
                if(reader->getCommandReceivedStatus() == paCommandReceivedEventReader::PA_IN_USE_CYCLIC_CANCELLED)
                {
                    TA_THROW(TrainPaFirstCyclicException(e.what()));
                }
                else
                {
                    throw;
                }
            }
            // ++ss

        }
        catch (TrainException & e) 
        {
	        m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_PA, "", true);
            TA_RETHROW( e ); 
        }
        m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_PA, "", true);
        
    }
    
    
    void TrainPublicAddress::paSetupLiveAnnouncement(unsigned char trainID,
        bool override, unsigned char announcementID, std::string sessionID)
    {
		TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncPublicAddress);

		// attempt to own the session
        m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_PA, sessionID);

        // Send command to train
        TA_IRS_App::paLiveAnnouncement command; 
        
        command.setTrainID(trainID);
        if (override) 
        {
            command.setStatus( static_cast<paLiveAnnouncement::EPAAnnouncementStatus>(OVERRIDE), 
                announcementID);
        } 
        else
        {
            command.setStatus( static_cast<paLiveAnnouncement::EPAAnnouncementStatus>(REQUEST),
                announcementID);
        }

        try
        {    
            TrainEventPtr event = m_trainManager.sendTrainCommandWithWait(trainID, command, m_timerPA1);
            
            // Check event reply
            paCommandReceivedEventReader* reader = dynamic_cast<paCommandReceivedEventReader*>(event->getReader());
            if (0 == reader)
            {
                TA_THROW( TrainGeneralFailureException() );
            }
            
            // check validity of received message - command matches; 
            if (paCommandReceivedEventReader::PA_LIVE_ANNOUNEMENT != 
                reader->getOriginalCommand())
            {
                TA_THROW(TrainInvalidParameterException());
            }

            // busy?
            checkBusy(reader);

            m_trainManager.getTrainStatusManager().setPAAnnouncementID(trainID, announcementID);

            // wait for the ready for live announcement
            m_trainManager.getTrainTimer().startTimer(trainID, m_timerPA2);
            m_trainManager.getTrainTimer().waitTimer(trainID);
        }
        catch (TrainException & e) 
        {
            m_trainManager.getTrainTimer().endTimer(trainID);
            m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_PA, "", true);

            TA_RETHROW( e ); 
        }

        // clean up the timer once signalled
        m_trainManager.getTrainTimer().endTimer(trainID);
    }
    
    void TrainPublicAddress::paBeginLiveAnnouncement(unsigned char announcementId, std::string sessionID)
    {
        // get all trains involved in this announcement

        // Create audio line to train. Use RadioSessionCorbaDef interface... 

        int callId = 0;
		
		{
			std::vector<unsigned char> trainList = 
				m_trainManager.getTrainStatusManager().getTrainsForAnnouncement(announcementId);
			
			if (trainList.size() == 0)
			{
				TA_THROW( TrainInvalidParameterException() );
			}
			
			std::vector<std::string> itsiList;
			for (std::vector<unsigned char>::iterator iter = trainList.begin();
			iter != trainList.end(); iter++)
			{
				itsiList.push_back( m_trainManager.getTrainStatusManager().getTrainTSI(*iter) );
			}
			
			// Initiate the call
			callId = m_notifier.callRadioMakeCall(itsiList, ITrainNotificationFacade::RADIO_PA, sessionID);
			if (callId == 0)
			{
				TA_THROW( TrainGeneralFailureException() );
			}
		}

        // set the call Id in each trains state
        std::vector<unsigned char> trainList = 
            m_trainManager.getTrainStatusManager().getTrainsForAnnouncement(announcementId);
        
        for (std::vector<unsigned char>::iterator trainIter = trainList.begin();
        trainIter != trainList.end(); ++trainIter )
        {
            TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), *trainIter, syncPublicAddress);

            // set the callId in the train status
	        m_trainManager.getTrainStatusManager().setCallAllocation(*trainIter, TrainStatus::Resource_PA, callId);
        }
    }
    
    void TrainPublicAddress::paContinueLiveAnnouncement(unsigned char trainID, std::string sessionID)
    {
        // verify the session
        //m_trainManager.getTrainStatusManager().verifyOwningSessionID(trainID, TrainStatus::Resource_PA, sessionID);

		unsigned char iscsAnnouncementId = m_trainManager.getTrainStatusManager().getPAAnnouncementID(trainID);
		
		// we have no record of it
		if (iscsAnnouncementId == 0)
		{
            // just log an error, there is no point providing feedback
            // this happens due to timing, if we receive a PA reset at the same time
            // an operator hits continue, then we dont want to send the continue
            // if we throw an error, it will hide the actual condition in the PA manager
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"PA Continue button pressed for train %d the announcement on that train has ended, ignoring.", trainID );
			return;
		}

		TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncPublicAddress);
            
        // Send command to train
        paContinueCommand command; 
        
        command.setAnnouncementId(iscsAnnouncementId);
        
        m_trainManager.sendTrainCommandNoReply(trainID, command);
        
        // Update continueCount to zero
        m_trainManager.getTrainStatusManager().resetContinueCounter(trainID, TrainStatus::Resource_PA);

        // start the PA 3 timer
        startPA3Timer(trainID);
    }

    
    void TrainPublicAddress::paEndLiveAnnouncement(unsigned char trainID, std::string sessionID)
    {
        if (m_trainManager.getTrainStatusManager().getPAAnnouncementID(trainID) == 0)
        {
            return;
        }

        // verify the session
        //m_trainManager.getTrainStatusManager().verifyOwningSessionID(trainID, TrainStatus::Resource_PA, sessionID);    

		TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncPublicAddress);

		// reset the callId in the train status
		m_trainManager.getTrainStatusManager().setCallAllocation(trainID, TrainStatus::Resource_PA, 0);


        // Send command to train
        paResetCommand command; 
        command.setAnnouncementId(
            m_trainManager.getTrainStatusManager().getPAAnnouncementID(trainID));

        try 
        {
            TrainEventPtr event = m_trainManager.sendTrainCommandWithWait(trainID, command, m_timerPA4);
            
			// TD12450 - handle both reply events
			TrainEventReader* r = event->getReader();
			if (dynamic_cast<paCommandReceivedEventReader*>(r) != 0)
			{
				// Still process this event, as it indicates error conditions
				paCommandReceivedEventReader* reader = dynamic_cast<paCommandReceivedEventReader*>(r);
				try
				{
					checkBusy(reader);
				}
				catch( TrainGeneralFailureException& tge )
				{
					// rethrow every exception except if PA said reset - as this is expected for a reset/abort
					if (tge.reason() != TrainGeneralFailureException::RESET)
					{
						throw;
					}
				}
			}
			else if (dynamic_cast<paTrainMessageCompletedEventReader*>(r) != 0)
			{
				// error, unexpected event
				TA_THROW( TrainGeneralFailureException() );
			}
			else
			{
				// error, unexpected event
				TA_THROW( TrainGeneralFailureException() );
			}
        }
        catch (TrainException & e) 
        {
            m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_PA, "", true); 
			m_trainManager.getTrainStatusManager().resetContinueCounter(trainID, TrainStatus::Resource_PA);
            TA_RETHROW( e ); 
        }

		m_trainManager.getTrainStatusManager().resetContinueCounter(trainID, TrainStatus::Resource_PA);
        m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_PA, "", true);
        m_trainManager.getTrainStatusManager().setPAAnnouncementID(trainID, 0);
    }
    

    void TrainPublicAddress::paEndBroadcast(unsigned char trainID, unsigned char announcementID, std::string sessionID)
    {
        // Send command to train
        paResetCommand command; 
        command.setAnnouncementId(announcementID);

        try 
        {
            TrainEventPtr event = m_trainManager.sendTrainCommandWithWait(trainID, command, m_timerPA4);

			TrainEventReader* r = event->getReader();
			if (dynamic_cast<paCommandReceivedEventReader*>(r) != 0)
			{
				// Still process this event, as it indicates error conditions
				paCommandReceivedEventReader* reader = dynamic_cast<paCommandReceivedEventReader*>(r);
				try
				{
					checkBusy(reader);
				}
				catch( TrainGeneralFailureException& tge )
				{
					// rethrow every exception except if PA said reset - as this is expected for a reset/abort
					if (tge.reason() != TrainGeneralFailureException::RESET)
					{
						throw;
					}
				}
			}
			else if (dynamic_cast<paTrainMessageCompletedEventReader*>(r) != 0)
			{
				// Note: Do *not* need to call onPATrainMessageCompleted(), as PA already sets 
				// the state to aborted by calling this method 
			}
			else
			{
				// error, unexpected event
				TA_THROW( TrainGeneralFailureException() );
			}
        }
        catch (TrainException & e) 
        {
            TA_RETHROW( e ); 
        }
    }
    

    void TrainPublicAddress::onPAReadyForLiveAnnouncement(TrainEvent &event, paReadyForLiveAnnouncementEventReader * reader)
    {
        unsigned int trainID = event.getTrainID();

		TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncPublicAddress);

        std::string sessionID = m_trainManager.getTrainStatusManager().getOwningSessionID(trainID, TrainStatus::Resource_PA);

        // Notify (via semaphore) beginLiveAnnouncement() method, so it can return
        m_trainManager.getTrainTimer().signalTimer(trainID);

		// Notify PA the train is ready
        m_notifier.notifyPublicAddress(
            PA_ReadyForLiveAnnouncement,
            trainID,
            m_trainManager.getTrainStatusManager().getPAAnnouncementID(trainID),
            sessionID);

        // start the PA 3 timer
        startPA3Timer(trainID);
    }

    
    void TrainPublicAddress::onPARequestReset(TrainEvent &event, paRequestResetEventReader * reader)
    {
        unsigned int trainID = event.getTrainID();

		// first, try to get the announcement ID from the continue
		unsigned char messageAnnouncementId = reader->getAnnouncementId();
		unsigned char iscsAnnouncementId = m_trainManager.getTrainStatusManager().getPAAnnouncementID(trainID);
		
		// if the message announcement Id and ours is 0, we dont know anything about this
		if ( (messageAnnouncementId == 0) && (iscsAnnouncementId == 0) )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"PA Continue received when there is no ongoing announcement." );

			return;
		}

		// the message had an announement id, but we have no record of it, ignore
		if (iscsAnnouncementId == 0)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"PA Continue received for unknown announcement id %d, ignoring.", messageAnnouncementId );
			return;
		}

        // stop the PA 3 timer
        stopPA3Timer(trainID);

		TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncPublicAddress);

        // Update TrainStatus - increment retries
        m_trainManager.getTrainStatusManager().incrementContinueCounter(trainID, TrainStatus::Resource_PA);
        
        // If retries less than 'limit' autosend the continue
        if (!m_trainManager.getTrainStatusManager().exceededContinueLimit(trainID, TrainStatus::Resource_PA))
        {
            // Send an automatic reply
            paContinueCommand command; 
            //command.setAnnouncementId(reader->getAnnouncementId());
			command.setAnnouncementId(iscsAnnouncementId);
            try
            {
                m_trainManager.sendTrainCommandNoReply(trainID, command);
                // no reply expected

                // start the PA 3 timer
                startPA3Timer(trainID);
            }
            catch (TrainMessageTimeoutException e) 
            {
                // don't care about reply
            }
        }
        else 
        {
			// dont reset it because if the operator doesnt press it and we receive another continue then
			// we will auto send one
            //m_trainManager.getTrainStatusManager().resetContinueCounter(trainID, TrainStatus::Resource_PA);
            
            // Notify OCC (ContinueLive)
            std::string sessionID = m_trainManager.getTrainStatusManager().getOwningSessionID(trainID, TrainStatus::Resource_PA);
            m_notifier.notifyPublicAddress(PA_ContinueLiveAnnouncement, trainID, iscsAnnouncementId, sessionID);
        }
    }

    
    void TrainPublicAddress::onPAReset(TrainEvent &event, paResetEventReader * reader)
    {
        unsigned int trainID = event.getTrainID();

        // stop the PA 3 timer
        stopPA3Timer(trainID);

		TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncPublicAddress);

		std::string sessionID = m_trainManager.getTrainStatusManager().getOwningSessionID(trainID, TrainStatus::Resource_PA);

        m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_PA, "", true);
        m_trainManager.getTrainStatusManager().resetContinueCounter(trainID, TrainStatus::Resource_PA);

        // Notify OCC (ResetLive)
        m_notifier.notifyPublicAddress(PA_LiveAnnouncementReset, trainID, reader->getAnnouncementId(), sessionID);

        m_trainManager.getTrainStatusManager().setPAAnnouncementID(trainID, 0);

    }

    
    void TrainPublicAddress::onPATrainMessageCompleted(TrainEvent &event, paTrainMessageCompletedEventReader * reader)
    {
        unsigned int trainID = event.getTrainID();

        TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncPublicAddress);

		std::string sessionID = m_trainManager.getTrainStatusManager().getOwningSessionID(trainID, TrainStatus::Resource_PA);

        // This message is used for a couple of different things... make sure we check them all...
        switch(reader->getCompletionStatus())
        {
        case paTrainMessageCompletedEventReader::SUCCESSFUL:
            // Notify OCC (CompletedCyclic)
            m_notifier.notifyPublicAddress(PA_CompletedAnnouncement, trainID, reader->getAnnouncementId(), sessionID);

            break;
        case paTrainMessageCompletedEventReader::OCC_INTERRUPT:
        case paTrainMessageCompletedEventReader::PSC_INTERRUPT:
        case paTrainMessageCompletedEventReader::DRIVER_INTERRUPT:
            // Notify OCC (Interupted)
            m_notifier.notifyPublicAddress(PA_InteruptedAnnouncement, trainID, reader->getAnnouncementId(), sessionID);

            break;
        }
    }


    void TrainPublicAddress::handleTimeout(unsigned char trainId)
    {
        onPA3Timeout(trainId);
    }


    void TrainPublicAddress::startPA3Timer(unsigned char trainId)
    {
        m_pa3TimerManager.startTimer(trainId, m_timerPA3, true, "PA3Timer");
    }

    void TrainPublicAddress::stopPA3Timer(unsigned char trainId)
    {
        m_pa3TimerManager.endTimer(trainId);
    }

    void TrainPublicAddress::onPA3Timeout(unsigned char trainId)
    {
        // An expected TIMS reset request was not received

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
            "TIMS communication lost for PA announcment on train %d, notifying operator", trainId );

        // Just invalidate the train, and send a message out
        // do not send anything to the train

        std::string sessionId =
            m_trainManager.getTrainStatusManager().getOwningSessionID(trainId, TrainStatus::Resource_PA);
        unsigned char announcementId =
            m_trainManager.getTrainStatusManager().getPAAnnouncementID(trainId);

        m_notifier.notifyPublicAddress(PA_ContinueTimeoutAnnouncement, trainId, announcementId, sessionId);
    }



    //
    // parallel commands
    //

    void TrainPublicAddress::paPreRecordedAnnouncementParallel(
        const std::vector<unsigned char>& trainList,
        bool override, 
        unsigned char tableOffset, 
        unsigned short libraryVersion, 
        unsigned char announcementID,
        OperationModeManager& operationModeManager,
        std::string sessionID)
    {
        FUNCTION_ENTRY("paPreRecordedAnnouncementParallel()");

        // the number of threads that need to block
        ACE_Barrier barrier(trainList.size() + 1);

        for (std::vector<unsigned char>::const_iterator iter = trainList.begin();
            iter != trainList.end(); ++iter)
        {
            PaPreRecordedAnnouncementCommand *x =
                new PaPreRecordedAnnouncementCommand( *this,
                                                      m_notifier,
                                                      *iter,
                                                      override, 
                                                      tableOffset, 
                                                      libraryVersion, 
                                                      announcementID,
                                                      operationModeManager,
                                                      sessionID,
                                                      barrier );
            m_workQueueManager.perform(x);
        }

        // now wait for each command to finish
        barrier.wait();

        FUNCTION_EXIT;
    }

    void TrainPublicAddress::paCyclicAnnouncementParallel(
        const std::vector<unsigned char>& trainList,
        bool override,
        unsigned char tableOffset,
        unsigned short libraryVersion,
        const PATimeInterval& announcementPeriod,
        unsigned char announcementID,
        OperationModeManager& operationModeManager,
        std::string sessionID)
    {
        FUNCTION_ENTRY("paCyclicAnnouncementParallel()");

        // the number of threads that need to block
        ACE_Barrier barrier(trainList.size() + 1);

        for (std::vector<unsigned char>::const_iterator iter = trainList.begin();
            iter != trainList.end(); ++iter)
        {
            PaCyclicAnnouncementCommand *x =
                new PaCyclicAnnouncementCommand( *this,
                                                 m_notifier,
                                                 *iter,
                                                 override, 
                                                 tableOffset, 
                                                 libraryVersion,
                                                 announcementPeriod,
                                                 announcementID,
                                                 operationModeManager,
                                                 sessionID,
                                                 barrier );
            m_workQueueManager.perform(x);
        }

        // now wait for each command to finish
        barrier.wait();

        FUNCTION_EXIT;
    }

    void TrainPublicAddress::paSetupLiveAnnouncementParallel(
        const std::vector<unsigned char>& trainList,
        bool override, 
        unsigned char announcementID,
        OperationModeManager& operationModeManager,
        std::string sessionID)
    {
        FUNCTION_ENTRY("paSetupLiveAnnouncementParallel()");

        for (std::vector<unsigned char>::const_iterator iter = trainList.begin();
            iter != trainList.end(); ++iter)
        {
            PaSetupLiveAnnouncementCommand *x =
                new PaSetupLiveAnnouncementCommand( *this,
                                                    m_notifier,
                                                    *iter,
                                                    override, 
                                                    announcementID,
                                                    operationModeManager,
                                                    sessionID );
            m_workQueueManager.perform(x);
        }

        FUNCTION_EXIT;
    }

    void TrainPublicAddress::paEndLiveAnnouncementParallel(
        unsigned char announcementId,
        std::string sessionID)
    {
        FUNCTION_ENTRY("paEndLiveAnnouncementParallel()");

        // get the call ID for the announcement
        unsigned long callId =
            m_trainManager.getTrainStatusManager().getCallForAnnouncement(announcementId);

        // get the trains involved in the announcement
        std::vector<unsigned char> trainList =
            m_trainManager.getTrainStatusManager().getTrainsForAnnouncement( announcementId );

        // reset the audio
		m_notifier.callRadioReleaseCall(callId, sessionID);

        // the number of threads that need to block
        ACE_Barrier barrier(trainList.size() + 1);

        // send the commands to the trains
        for (std::vector<unsigned char>::const_iterator iter = trainList.begin();
            iter != trainList.end(); ++iter)
        {
            PaEndLiveAnnouncementCommand *x =
                new PaEndLiveAnnouncementCommand( *this,
                                                  m_notifier,
                                                  *iter,
                                                  announcementId,
                                                  sessionID,
                                                  &barrier);
            m_workQueueManager.perform(x);
        }

        // now wait for each command to finish
        barrier.wait();
        FUNCTION_EXIT;
    }

    void TrainPublicAddress::paEndBroadcastParallel(
        const std::vector<unsigned char>& trainList,
        unsigned char announcementId,
        std::string sessionID)
    {
        FUNCTION_ENTRY("paEndBroadcastParallel()");

        // the number of threads that need to block
        ACE_Barrier barrier(trainList.size() + 1);

        // send the commands to the trains
        for (std::vector<unsigned char>::const_iterator iter = trainList.begin();
            iter != trainList.end(); ++iter)
        {
            PaEndBroadcastCommand *x =
                new PaEndBroadcastCommand( *this,
                                           m_notifier,
                                           *iter,
                                           announcementId,
                                           sessionID,
                                           &barrier);
            m_workQueueManager.perform(x);
        }

        // now wait for each command to finish
        barrier.wait();
        FUNCTION_EXIT;
    }




    // parallel command ACE methods
	std::string PaPreRecordedAnnouncementCommand::info()
    {
		std::ostringstream s;
		s << "PaPreRecordedAnnouncementCommand [Train=" << static_cast<int>(m_trainId) << "]";

		return s.str();
	}

    int PaPreRecordedAnnouncementCommand::call()
    {
        FUNCTION_ENTRY("PaPreRecordedAnnouncementCommand::call()");

        // for the command result notification
        ITrainNotificationFacade::EPaErrorType error(ITrainNotificationFacade::NoError);
        std::string errorDetails("");
        bool canOverride(false);

        // check whether to execute
        if ( !m_operationModeManager.isInControlOfTrain(m_trainId) )
        {
            error = ITrainNotificationFacade::TrainAccessDenied;
            errorDetails = "Not in control of this train";
        }
        else
        {
            try
		    {
                // simple, execute the command
                m_trainPublicAddress.paPreRecordedAnnouncement(
                    m_trainId, m_override, m_tableOffset, m_libraryVersion,
                    m_announcementID, m_sessionId);
		    }
		    catch (TrainInvalidTrainException& e)
		    {
				LOG_EXCEPTION_CATCH(SourceInfo, "TrainInvalidTrainException", e.what());

                error = ITrainNotificationFacade::InvalidParameter;
                errorDetails = "Specified train is invalid";
		    }
		    catch (TrainInvalidParameterException& e)
		    {
				LOG_EXCEPTION_CATCH(SourceInfo, "TrainInvalidParameterException", e.what());

                error = ITrainNotificationFacade::InvalidParameter;
                errorDetails = e.what();
		    }
		    catch (TrainTimeoutException& e )
		    {
				LOG_EXCEPTION_CATCH(SourceInfo, "TrainTimeoutException", e.what());

			    TA_Base_Core::DescriptionParameters parameters;
			    TA_Base_Core::NameValuePair nvpair("Event", "preRecordedAnnouncement");
			    parameters.push_back(&nvpair);
			    m_notify.AuditOperation(TA_Base_Core::TrainAudit::TrainTimeoutAuditEvent, parameters, m_sessionId);
			    
                error = ITrainNotificationFacade::TrainTimeout;
		    }
		    catch (TrainBusyException& e)
		    {
				LOG_EXCEPTION_CATCH(SourceInfo, "TrainBusyException", e.what());

                error = ITrainNotificationFacade::TrainBusy;
                errorDetails = e.what();
                canOverride = e.override();
		    }
		    catch (TrainGeneralFailureException& e)
		    {
				LOG_EXCEPTION_CATCH(SourceInfo, "TrainGeneralFailureException", e.what());

                error = ITrainNotificationFacade::TrainGeneralFailure;
                errorDetails = e.what();
		    }
            catch (TrainException& e)
		    {
				LOG_EXCEPTION_CATCH(SourceInfo, "TrainException", e.what());

                error = ITrainNotificationFacade::TrainGeneralFailure;
                errorDetails = e.what();
		    }
        }

        // Send the command result
        m_notify.notifyPublicAddress(
            TrainPublicAddress::PA_CommandResultAnnouncement,
            m_trainId,
            m_announcementID,
            m_sessionId,
            error,
            errorDetails,
            canOverride);

        if (error == ITrainNotificationFacade::NoError)
        {
		    // Now audit the event
		    TA_Base_Core::DescriptionParameters parameters;

            std::stringstream trainIDstr;
		    trainIDstr << static_cast<int>(m_trainId) << std::ends;
		    TA_Base_Core::NameValuePair nvpair("Train", trainIDstr.str().c_str());
		    parameters.push_back(&nvpair);

            std::stringstream idStr;
		    idStr << m_announcementID << std::ends;
		    TA_Base_Core::NameValuePair nvid("ID", idStr.str().c_str());
            parameters.push_back(&nvid);

            std::stringstream libStr;
		    libStr << m_libraryVersion << std::ends;
		    TA_Base_Core::NameValuePair nvlib("Library", libStr.str().c_str());
		    parameters.push_back(&nvlib);
            //m_notify.AuditOperation(TA_Base_Core::TrainAudit::TrainPAPrerecorded,parameters,m_sessionId);
        }

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "PaPreRecordedAnnouncementCommand completed for train %d, waiting for others to complete.", m_trainId);

        // wait on the barrier
        m_barrier.wait();

        FUNCTION_EXIT;
        return 0;
    }


	std::string PaCyclicAnnouncementCommand::info()
    {
		std::ostringstream s;
		s << "PaCyclicAnnouncementCommand [Train=" << static_cast<int>(m_trainId) << "]";

		return s.str();
	}

    int PaCyclicAnnouncementCommand::call()
	{
        FUNCTION_ENTRY("PaCyclicAnnouncementCommand::call()");

        // for the command result notification
        ITrainNotificationFacade::EPaErrorType error(ITrainNotificationFacade::NoError);
        std::string errorDetails("");
        bool canOverride(false);

        // check whether to execute
        if ( !m_operationModeManager.isInControlOfTrain(m_trainId) )
        {
            error = ITrainNotificationFacade::TrainAccessDenied;
            errorDetails = "Not in control of this train";
        }
        else
        {
            try
		    {
                // simple, execute the command
                m_trainPublicAddress.paCyclicAnnouncement(
                    m_trainId, m_override, m_tableOffset, m_libraryVersion,
                    m_announcementPeriod, m_announcementID, m_sessionId);
		    }
		    catch (TrainInvalidTrainException& e)
		    {
				LOG_EXCEPTION_CATCH(SourceInfo, "TrainInvalidTrainException", e.what());

                error = ITrainNotificationFacade::InvalidParameter;
                errorDetails = "Specified train is invalid";
		    }
		    catch (TrainInvalidParameterException& e)
		    {
				LOG_EXCEPTION_CATCH(SourceInfo, "TrainInvalidParameterException", e.what());

                error = ITrainNotificationFacade::InvalidParameter;
                errorDetails = e.what();
		    }
		    catch (TrainTimeoutException& e)
		    {
				LOG_EXCEPTION_CATCH(SourceInfo, "TrainTimeoutException", e.what());

			    TA_Base_Core::DescriptionParameters parameters;
			    TA_Base_Core::NameValuePair nvpair("Event", "cyclicAnnouncment");
			    parameters.push_back(&nvpair);
			    m_notify.AuditOperation(TA_Base_Core::TrainAudit::TrainTimeoutAuditEvent, parameters, m_sessionId);
			    
                error = ITrainNotificationFacade::TrainTimeout;
		    }
		    catch (TrainBusyException& e)
		    {
				LOG_EXCEPTION_CATCH(SourceInfo, "TrainBusyException", e.what());

                error = ITrainNotificationFacade::TrainBusy;
                errorDetails = e.what();
                canOverride = e.override();
		    }
		    catch (TrainGeneralFailureException& e)
		    {
				LOG_EXCEPTION_CATCH(SourceInfo, "TrainGeneralFailureException", e.what());

                error = ITrainNotificationFacade::TrainGeneralFailure;
                errorDetails = e.what();
		    }
            // ss++
            // TD13952
            catch(TrainPaFirstCyclicException& e)
            {
                error = ITrainNotificationFacade::TrainPaFirstCyclicCancelled;
                errorDetails = e.what();
            }
            // ++ss
            catch (TrainException& e)
		    {
				LOG_EXCEPTION_CATCH(SourceInfo, "TrainException", e.what());

                error = ITrainNotificationFacade::TrainGeneralFailure;
                errorDetails = e.what();
		    }
        }

        // Send the command result
        m_notify.notifyPublicAddress(
            TrainPublicAddress::PA_CommandResultAnnouncement,
            m_trainId,
            m_announcementID,
            m_sessionId,
            error,
            errorDetails,
            canOverride);

        if (error == ITrainNotificationFacade::NoError)
        {
            TA_Base_Core::DescriptionParameters parameters;

            std::stringstream trainIDstr;
		    trainIDstr << static_cast<int>(m_trainId) << std::ends;
		    TA_Base_Core::NameValuePair nvpair("Train", trainIDstr.str().c_str());
		    parameters.push_back(&nvpair);

		    std::stringstream idStr;
		    idStr << m_announcementID << std::ends;
		    TA_Base_Core::NameValuePair nvid("ID", idStr.str().c_str());
            parameters.push_back(&nvid);

		    std::stringstream libStr;
		    libStr << m_libraryVersion << std::ends;
		    TA_Base_Core::NameValuePair nvlib("Library", libStr.str().c_str());
		    parameters.push_back(&nvlib);

		    //m_notify.AuditOperation(TA_Base_Core::TrainAudit::TrainPACyclic,parameters,sessionID);
        }

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "PaCyclicAnnouncementCommand completed for train %d, waiting for others to complete.", m_trainId);

        // wait on the barrier
        m_barrier.wait();

        FUNCTION_EXIT;
        return 0;
	}

	std::string PaSetupLiveAnnouncementCommand::info()
    {
		std::ostringstream s;
		s << "PaSetupLiveAnnouncementCommand [Train=" << static_cast<int>(m_trainId) << "]";

		return s.str();
	}

    int PaSetupLiveAnnouncementCommand::call()
	{
        FUNCTION_ENTRY("PaSetupLiveAnnouncementCommand::call()");
        
        // for the command result notification
        ITrainNotificationFacade::EPaErrorType error(ITrainNotificationFacade::NoError);
        std::string errorDetails("");
        bool canOverride(false);

        // check whether to execute
        if ( !m_operationModeManager.isInControlOfTrain(m_trainId) )
        {
            error = ITrainNotificationFacade::TrainAccessDenied;
            errorDetails = "Not in control of this train";
        }
        else
        {
            try
		    {
                // simple, execute the command
                m_trainPublicAddress.paSetupLiveAnnouncement(
                    m_trainId, m_override, m_announcementID, m_sessionId);
		    }
		    catch (TrainInvalidTrainException& e)
		    {
				LOG_EXCEPTION_CATCH(SourceInfo, "TrainInvalidTrainException", e.what());

                error = ITrainNotificationFacade::InvalidParameter;
                errorDetails = "Specified train is invalid";
		    }
		    catch (TrainInvalidParameterException& e)
		    {
				LOG_EXCEPTION_CATCH(SourceInfo, "TrainInvalidParameterException", e.what());

                error = ITrainNotificationFacade::InvalidParameter;
                errorDetails = e.what();
		    }
		    catch (TrainTimeoutException& e)
		    {
				LOG_EXCEPTION_CATCH(SourceInfo, "TrainTimeoutException", e.what());

			    TA_Base_Core::DescriptionParameters parameters;
			    TA_Base_Core::NameValuePair nvpair("Event", "setupLiveAnnouncement");
			    parameters.push_back(&nvpair);
			    m_notify.AuditOperation(TA_Base_Core::TrainAudit::TrainTimeoutAuditEvent, parameters, m_sessionId);
			    
                error = ITrainNotificationFacade::TrainTimeout;
		    }
		    catch (TrainBusyException& e)
		    {
				LOG_EXCEPTION_CATCH(SourceInfo, "TrainBusyException", e.what());

                error = ITrainNotificationFacade::TrainBusy;
                errorDetails = e.what();
                canOverride = e.override();
		    }
		    catch (TrainGeneralFailureException& e)
		    {
				LOG_EXCEPTION_CATCH(SourceInfo, "TrainGeneralFailureException", e.what());

                error = ITrainNotificationFacade::TrainGeneralFailure;
                errorDetails = e.what();
		    }
            catch (TrainException& e)
		    {
				LOG_EXCEPTION_CATCH(SourceInfo, "TrainException", e.what());

                error = ITrainNotificationFacade::TrainGeneralFailure;
                errorDetails = e.what();
		    }
        }

        // Send the command result - only if there was an error.
        // if there was no error, a ready for live announcement message will be sent instead
        if (error != ITrainNotificationFacade::NoError)
        {
            m_notify.notifyPublicAddress(
                TrainPublicAddress::PA_CommandResultAnnouncement,
                m_trainId,
                m_announcementID,
                m_sessionId,
                error,
                errorDetails,
                canOverride);
        }

        // Now audit the event
        if (error == ITrainNotificationFacade::NoError)
        {
		    TA_Base_Core::DescriptionParameters parameters;

		    std::stringstream trainIDstr;
		    trainIDstr << static_cast<int>(m_trainId) << std::ends;
		    TA_Base_Core::NameValuePair nvpair("Train", trainIDstr.str().c_str());
		    parameters.push_back(&nvpair);

		    std::stringstream idStr;
		    idStr << m_announcementID << std::ends;
		    TA_Base_Core::NameValuePair nvid("ID", idStr.str().c_str());
		    parameters.push_back(&nvid);

		    //m_notify.AuditOperation(TA_Base_Core::TrainAudit::TrainPASetupLiveAnnouncement,parameters,sessionID);
        }

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "PaSetupLiveAnnouncementCommand completed for train %d.", m_trainId);

        // do not wait on a barrier
        // this call must return fast to the client

        FUNCTION_EXIT;
        return 0;
	}

	std::string PaEndLiveAnnouncementCommand::info()
    {
		std::ostringstream s;
		s << "PaEndLiveAnnouncementCommand [Train=" << static_cast<int>(m_trainId) << "]";

		return s.str();
	}

    int PaEndLiveAnnouncementCommand::call()
	{
        FUNCTION_ENTRY("PaEndLiveAnnouncementCommand::call()");

        // for the command result notification
        ITrainNotificationFacade::EPaErrorType error(ITrainNotificationFacade::NoError);
        std::string errorDetails("");
        bool canOverride(false);

        try
		{
            // simple, execute the command
            m_trainPublicAddress.paEndLiveAnnouncement(
                m_trainId, m_sessionId);
		}
		catch (TrainInvalidTrainException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TrainInvalidTrainException", e.what());

            error = ITrainNotificationFacade::InvalidParameter;
            errorDetails = "Specified train is invalid";
		}
		catch (TrainInvalidParameterException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TrainInvalidParameterException", e.what());

            error = ITrainNotificationFacade::InvalidParameter;
            errorDetails = e.what();
		}
		catch (TrainTimeoutException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TrainTimeoutException", e.what());

			TA_Base_Core::DescriptionParameters parameters;
			TA_Base_Core::NameValuePair nvpair("Event", "endLiveAnnouncement");
			parameters.push_back(&nvpair);
			m_notify.AuditOperation(TA_Base_Core::TrainAudit::TrainTimeoutAuditEvent, parameters, m_sessionId);
			
            error = ITrainNotificationFacade::TrainTimeout;
		}
		catch (TrainBusyException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TrainBusyException", e.what());

            error = ITrainNotificationFacade::TrainBusy;
            errorDetails = e.what();
            canOverride = e.override();
		}
		catch (TrainGeneralFailureException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TrainGeneralFailureException", e.what());

            error = ITrainNotificationFacade::TrainGeneralFailure;
            errorDetails = e.what();
		}
        catch (TrainException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TrainException", e.what());

            error = ITrainNotificationFacade::TrainGeneralFailure;
            errorDetails = e.what();
		}

        // Send the command result
        if (m_barrier)
        {
            // only if the caller cares about it
            m_notify.notifyPublicAddress(
                TrainPublicAddress::PA_CommandResultAnnouncement,
                m_trainId,
                m_announcementID,
                m_sessionId,
                error,
                errorDetails,
                canOverride);
        }

        // Now audit the event
		TA_Base_Core::DescriptionParameters parameters;

		std::stringstream trainIDstr;
		trainIDstr << static_cast<int>(m_trainId) << std::ends;
		TA_Base_Core::NameValuePair nvpair("Train", trainIDstr.str().c_str());
		parameters.push_back(&nvpair);

	    std::stringstream idStr;
	    idStr << m_announcementID << std::ends;
	    TA_Base_Core::NameValuePair nvid("ID", idStr.str());
	    parameters.push_back(&nvid);

		//m_notify.AuditOperation(TA_Base_Core::TrainAudit::TrainPAEndLiveAnnouncement,parameters,sessionID);


        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "PaEndLiveAnnouncementCommand completed for train %d, waiting for others to complete.", m_trainId);

        // wait on the barrier
        if (m_barrier)
        {
            m_barrier->wait();
        }

        FUNCTION_EXIT;
        return 0;
	}

	std::string PaEndBroadcastCommand::info()
    {
		std::ostringstream s;
		s << "PaEndLiveAnnouncementCommand [Train=" << static_cast<int>(m_trainId) << "]";

		return s.str();
	}


    int PaEndBroadcastCommand::call()
	{
        FUNCTION_ENTRY("PaEndBroadcastCommand::call()");

        // for the command result notification
        ITrainNotificationFacade::EPaErrorType error(ITrainNotificationFacade::NoError);
        std::string errorDetails("");
        bool canOverride(false);

        try
		{
            // simple, execute the command
            m_trainPublicAddress.paEndBroadcast(
                m_trainId, m_announcementID, m_sessionId);
		}
		catch (TrainInvalidTrainException&)
		{
            error = ITrainNotificationFacade::InvalidParameter;
            errorDetails = "Specified train is invalid";
		}
		catch (TrainInvalidParameterException& e)
		{
            error = ITrainNotificationFacade::InvalidParameter;
            errorDetails = e.what();
		}
		catch (TrainTimeoutException& )
		{
			TA_Base_Core::DescriptionParameters parameters;
			TA_Base_Core::NameValuePair nvpair("Event", "endBroadcast");
			parameters.push_back(&nvpair);
			m_notify.AuditOperation(TA_Base_Core::TrainAudit::TrainTimeoutAuditEvent, parameters, m_sessionId);
			
            error = ITrainNotificationFacade::TrainTimeout;
		}
		catch (TrainBusyException& e)
		{
            error = ITrainNotificationFacade::TrainBusy;
            errorDetails = e.what();
            canOverride = e.override();
		}
		catch (TrainGeneralFailureException& e)
		{
            error = ITrainNotificationFacade::TrainGeneralFailure;
            errorDetails = e.what();
		}
        catch (TrainException& e)
		{
            error = ITrainNotificationFacade::TrainGeneralFailure;
            errorDetails = e.what();
		}

        // Send the command result
        if (m_barrier)
        {
            // only if the caller cares about it
            m_notify.notifyPublicAddress(
                TrainPublicAddress::PA_CommandResultAnnouncement,
                m_trainId,
                m_announcementID,
                m_sessionId,
                error,
                errorDetails,
                canOverride);
        }

        // Now audit the event
		TA_Base_Core::DescriptionParameters parameters;

		std::stringstream trainIDstr;
		trainIDstr << static_cast<int>(m_trainId) << std::ends;
		TA_Base_Core::NameValuePair nvpair("Train", trainIDstr.str().c_str());
		parameters.push_back(&nvpair);

	    std::stringstream idStr;
	    idStr << m_announcementID << std::ends;
	    TA_Base_Core::NameValuePair nvid("ID", idStr.str());
	    parameters.push_back(&nvid);

		//m_notify.AuditOperation(TA_Base_Core::TrainAudit::TrainPAEndLiveAnnouncement,parameters,sessionID);


        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "PaEndBroadcastCommand completed for train %d, waiting for others to complete.", m_trainId);

        // wait on the barrier
        if (m_barrier)
        {
            m_barrier->wait();
        }

        FUNCTION_EXIT;
        return 0;
	}


}  // namespace
