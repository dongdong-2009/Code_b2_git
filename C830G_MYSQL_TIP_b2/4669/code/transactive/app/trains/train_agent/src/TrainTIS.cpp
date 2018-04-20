/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/trains/train_agent/src/TrainTIS.cpp $
  * @author:  Glen Kidd
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class manages the PublicAdress commands.  It handles the sequencing
  * of the commands and error checking
  *
  */

///////////////////////////////////////////////////////////
//  TrainTIS.h
//  Implementation of the Class TrainPublicAdress
//  Created on:      21-Jan-2004
///////////////////////////////////////////////////////////

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <utility>
#include <functional>
#include <algorithm>

#include "TrainTIS.h"
#include "ITrainManager.h"
#include "app/trains/train_agent/src/TARethrow.h"
#include "app/trains/train_agent/src/TrainException.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/TrainEvent.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/ttisCommandReceivedEventReader.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/ttisEndOfDownloadEventReader.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/ttisFreeTextMessage.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/ttisLibraryDownloadCommand.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/ttisLibraryUpgradeCommand.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/ttisPreStoredMessageCommand.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/ttisResetMessageCommand.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/ttisScheduleDownloadCommand.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/ttisScheduleUpgradeCommand.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/ttisCommandReceivedEventReader.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/ttisEndOfDownloadEventReader.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/TTDFrame.h"
#include "core/data_access_interface/entity_access/src/TrainSettingsEntityData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/message/types/TrainAudit_MessageTypes.h"

using TA_IRS_App::TTDFrame;
using TA_Base_Core::DebugUtil;

// helper function in anonymous namespace
namespace {
    using namespace TA_IRS_App;

    void checkStatus(ttisCommandReceivedEventReader* reader)    
    {
        switch (reader->getCommandStatus())
        {
        case ttisCommandReceivedEventReader::TTIS_PROCEEDING:
            break; // normal
        case ttisCommandReceivedEventReader::TTIS_IN_USE:
            TA_THROW(TrainBusyException("Train in use, cannot override", false));
            break;
        case ttisCommandReceivedEventReader::TTIS_LIBRARY_UPGRADED:
            break;
        case ttisCommandReceivedEventReader::TTIS_RESET:
            //TA_THROW(TrainGeneralFailureException("", TrainGeneralFailureException::RESET));
            break;
        case ttisCommandReceivedEventReader::TTIS_FAILED:
            TA_THROW(TrainGeneralFailureException("", TrainGeneralFailureException::COMMAND_FAILED));
            break;
        case ttisCommandReceivedEventReader::DIFFERENT_VERSION_NUMBER:
            TA_THROW(TrainGeneralFailureException("",TrainGeneralFailureException::DIFF_VERSION));
            break;
        }
    }

	static const std::string MODEEXCEPTIONMESSAGE = "The train agent mode does not allow the operation";;
}

namespace TA_IRS_App {

    
    
    
    TrainTIS::TrainTIS(ITrainManager& callback, ITrainNotificationFacade& notifier,
		TrainWorkQueueManager& trainWorkQueueManager,OperationModeManager& operationModeManager) 
        : m_trainManager(callback),
          m_notifier(notifier),
          m_workQueueManager(trainWorkQueueManager),
		  m_operationModeManager(operationModeManager),
          m_ttis2TimerManager(*this)
    {
        m_trainManager.addDelegate( dynamic_cast<ITrainEventReaderDelegator*>(this) );
        m_trainManager.addDelegate( dynamic_cast<ITrainDownloadDelegator*>(this) );

        m_timerTIS1 = m_trainManager.getGlobalTrainAgentEntity()->getTimerTIS1();
        m_timerTIS2 = m_trainManager.getGlobalTrainAgentEntity()->getTimerTIS2();
    }
    
    
    TrainTIS::~TrainTIS()
    {
        
    }


    bool TrainTIS::delegate(TrainEvent& event)
    {
        TrainEventReader* r = event.getReader();
        bool consumed = true;
        /*
        if (dynamic_cast<ttisCommandReceivedEventReader*>(r) != 0)
        {
            onTISCommandReceived(event, dynamic_cast<ttisCommandReceivedEventReader*>(r));
        }
        else 
        */
        if (dynamic_cast<ttisEndOfDownloadEventReader*>(r) != 0)
        {
            onTISEndOfDownload(event, dynamic_cast<ttisEndOfDownloadEventReader*>(r));
        }
        else 
        {
            consumed = false;
        }
        return consumed;
    }

    bool TrainTIS::delegate(unsigned char trainId, bool success, const std::string& failReason)
    {
        if (trainId > 0)
        {
            std::string sessionID = m_trainManager.getTrainStatusManager().getOwningSessionID(trainId, TrainStatus::Resource_TTIS);

            if ( false == success )
            {
                stopDownloadTimer( trainId );

                // unsuccessful, send a failure message immediately
                switch (m_trainManager.getTrainStatusManager().getTTISStatus(trainId))
                {
                    case TrainTypes::TTIS_Library:
                        m_notifier.notifyTIS(TrainTIS::TIS_LibraryDownload, trainId, false, sessionID);
                        break;
                    case TrainTypes::TTIS_Schedule:
                        m_notifier.notifyTIS(TrainTIS::TIS_ScheduleDownload, trainId, false, sessionID);
                        break;
                    case TrainTypes::TTIS_Unknown:
                        // ignore
                        break;        
                }
            
                // cleanup
                m_trainManager.getTrainStatusManager().setTTISStatus(trainId, TrainTypes::TTIS_Unknown);
                // release the session
                m_trainManager.getTrainStatusManager().setOwningSessionID(trainId, TrainStatus::Resource_TTIS, "", true);
            }
            else
            {
                // start a timer for the TIMS message
                // to make sure an end of download is received for the train
                startDownloadTimer( trainId );
            }
        }
        else
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                "Received RadioComms CircuitModeNotification, for a TSI that is not a train");
        }

        return true;
    }

	short TrainTIS::getLibraryVersion(unsigned char trainID)
	{
        return static_cast<short>(m_trainManager.getTrainStatusManager().getTTISLibraryVersion(trainID));
	}

	short TrainTIS::getScheduleVersion(unsigned char trainID)
	{
        return static_cast<short>(m_trainManager.getTrainStatusManager().getTTISScheduleVersion(trainID));
	}


    void TrainTIS::displayFreeTextMessage(
        unsigned char trainID,
        unsigned char PIDAddress,
        unsigned char displayAttribute,
        unsigned char priority,
        const TimeInterval & displayPeriod,
        unsigned char repeatInterval,
        const char * message,
        const char * sessionID)
    {
		TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncNOP);
    
		// Send command to train
        ttisFreeTextMessage command(message); 
        
        command.setTrainID(trainID);

		// split pid into car/pid 
		unsigned char car = (0xf0 & PIDAddress) >>4;
		unsigned char pid = 0x0f & PIDAddress ;
		command.setPIDAddress(car,pid);
        command.setDisplayAttributes(displayAttribute);
		command.setMessagePriority(priority);
        command.setStartTime(displayPeriod.startHour, displayPeriod.startMin);
        command.setStopTime(displayPeriod.stopHour, displayPeriod.stopMin);
        command.setRepeatInterval(repeatInterval);

        try 
        {
            // this call can throw a TrainMessageTimeoutException, but the idl has a 
            // IPATrainCommandsCorbaDef::TrainTimeout defined - should we convert the exception???
            TrainEventPtr event = m_trainManager.sendTrainCommandWithWait(trainID, command, m_timerTIS1);
            
            // Check event reply
            ttisCommandReceivedEventReader* reader = dynamic_cast<ttisCommandReceivedEventReader*>(event->getReader());
            if (0 == reader)
            {
                TA_THROW( TrainGeneralFailureException() );
            }
            
            checkStatus(reader);
        }
        catch (TrainException & e) 
        {
            TA_RETHROW( e );
        }
    }
    
    void TrainTIS::displayStoredMessage(
        unsigned char trainID,
        unsigned char PIDAddress,
        short libraryVersion,
        unsigned char identifierType,
        unsigned char identifierOffset,
        unsigned char priority,
        const TimeInterval & displayPeriod,
        const char * sessionID)
    {
		TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncNOP);

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "TrainTIS::displayStoredMessage received a command for train:%d,PIDAddress:%d.",
			trainID,PIDAddress);
		

        // Send command to train
        ttisPreStoredMessageCommand command; 
        
        command.setTrainID(trainID);

		// split pid into car/pid 
		unsigned char car = (0xf0 & PIDAddress) >>4;
		unsigned char pid = 0x0f & PIDAddress ;
		command.setPIDAddress(car,pid);
        command.setLibraryVersion(libraryVersion);
        command.setIdentifierType(identifierType);
        command.setIdentifierOffset(identifierOffset);
        command.setMessagePriority(priority);
        command.setStartTime(displayPeriod.startHour, displayPeriod.startMin);
        command.setStopTime(displayPeriod.stopHour, displayPeriod.stopMin);

        try 
        {
            // this call can throw a TrainMessageTimeoutException, but the idl has a 
            // IPATrainCommandsCorbaDef::TrainTimeout defined - should we convert the exception???
            TrainEventPtr event = m_trainManager.sendTrainCommandWithWait(trainID, command, m_timerTIS1);
            
            // Check event reply
            ttisCommandReceivedEventReader* reader = dynamic_cast<ttisCommandReceivedEventReader*>(event->getReader());
            if (0 == reader)
            {
                TA_THROW( TrainGeneralFailureException() );
            }
            
            checkStatus(reader);
        }
        catch (TrainException & e) 
        {
            TA_RETHROW( e );
        }
    }
    
    void TrainTIS::resetMessage(
			unsigned char trainID, 
			unsigned char PIDAddress,
			unsigned char messageType)
    {
		TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncNOP);

        // Send command to train
        ttisResetMessageCommand command; 
        
        command.setTrainID(trainID);

		// split pid into car/pid 
		unsigned char car = (0xf0 & PIDAddress) >>4;
		unsigned char pid = 0x0f & PIDAddress ;
		command.setPIDAddress(car,pid);
		command.setMessageType(messageType);

        try 
        {
            // this call can throw a TrainMessageTimeoutException, but the idl has a 
            // IPATrainCommandsCorbaDef::TrainTimeout defined - should we convert the exception???
            TrainEventPtr event = m_trainManager.sendTrainCommandWithWait(trainID, command, m_timerTIS1);
            
            // Check event reply
            ttisCommandReceivedEventReader* reader = dynamic_cast<ttisCommandReceivedEventReader*>(event->getReader());
            if (0 == reader)
            {
                TA_THROW( TrainGeneralFailureException() );
            }
            
            checkStatus(reader);
        }
        catch (TrainException & e) 
        {
            TA_RETHROW( e );
        }
    }
    
    void TrainTIS::libraryDownload(unsigned char trainID, 
        const LibraryDataType& libraryData, 
        const char * sessionID)
    {
		TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncNOP);
        
		// attempt to own the session
        m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_TTIS, sessionID);    
        
        // Send command to train
        ttisLibraryDownloadCommand command; 
        
        command.setTrainID(trainID);
        command.setCommand(ttisLibraryDownloadCommand::START); 
        
        try 
        {
            TrainEventPtr event = m_trainManager.sendTrainCommandWithWait(trainID, command, m_timerTIS1);
            
            // Check event reply
            ttisCommandReceivedEventReader* reader = dynamic_cast<ttisCommandReceivedEventReader*>(event->getReader());
            if (0 == reader)
            {
                TA_THROW( TrainGeneralFailureException() );
            }
            
            checkStatus(reader);
        }
        catch (TrainException & e) 
        {
            m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_TTIS, "", true); 
            TA_RETHROW( e );
        }

        m_trainManager.getTrainStatusManager().setTTISStatus(trainID, TrainTypes::TTIS_Library);
        
        stopDownloadTimer( trainID );

		TrainTIS::LibraryDataType rawData;    
		for (unsigned int j = 0;j < libraryData.size();++j)
		{
			rawData.push_back(libraryData[j]);
		}

        TTDFrame::TtisFrameList frameList = TTDFrame::frameData(
            rawData, trainID, TTDFrame::LibraryData, TTDFrame::MESSAGE_DESTINATION_ISCS);
        // Save it in train status manager
        m_trainManager.getTrainStatusManager().setDownloadLibrary(trainID, frameList);

        // Prepare the buffer for Radio
        std::vector<unsigned char> framedData;
        for (TTDFrame::TtisFrameList::iterator i = frameList.begin(); i != frameList.end(); ++i)
        {
            std::vector<unsigned char> frame = (*i).frameData;
            std::copy(frame.begin(), frame.end(), std::back_inserter(framedData));
        }
        // Do the Download - who does queueing?
        std::string ITSI = m_trainManager.getTrainStatusManager().getTrainTSI(trainID);
        m_notifier.callRadioSendData(ITSI, framedData, sessionID); //exc

    }
    
    void TrainTIS::libraryUpgrade(unsigned char trainID, const char * sessionID)
    {
		TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncNOP);

		// verify the session
// not reqd ?        m_trainManager.getTrainStatusManager().verifyOwningSessionID(trainID, TrainStatus::Resource_TTIS, sessionID);  

        // Send command to train
        ttisLibraryUpgradeCommand command; 
        
        command.setTrainID(trainID);
        
        try 
        {
            TrainEventPtr event = m_trainManager.sendTrainCommandWithWait(trainID, command, m_timerTIS1);
            
            // Check event reply
            ttisCommandReceivedEventReader* reader = dynamic_cast<ttisCommandReceivedEventReader*>(event->getReader());
            if (0 == reader)
            {
                TA_THROW( TrainGeneralFailureException() );
            }
            
            checkStatus(reader);

            unsigned int version = reader->getLibraryVersion();

            // Update the TrainStatusInfo with new library version
            m_trainManager.getTrainStatusManager().setTTISLibraryVersion(trainID, version);

        }
        catch (TrainException & e) 
        {
            // release the session
// not reqd ?            m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_TTIS, "", true); 
            TA_RETHROW( e );
        }
        // release the session
// not reqd ?        m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_TTIS, "", true); 
    }
    
    void TrainTIS::scheduleDownload(unsigned char trainID,
        const LibraryDataType & libraryData,
        const char * sessionID)
    {
        // attempt to own the session
        m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_TTIS, sessionID); 
        
        // Send command to train
        ttisScheduleDownloadCommand command; 
        
        command.setTrainID(trainID);
        
        try 
        {
            TrainEventPtr event = m_trainManager.sendTrainCommandWithWait(trainID, command, m_timerTIS1);
            
            // Check event reply
            ttisCommandReceivedEventReader* reader = dynamic_cast<ttisCommandReceivedEventReader*>(event->getReader());
            if (0 == reader)
            {
                TA_THROW( TrainGeneralFailureException() );
            }
            
            checkStatus(reader);
        }
        catch (TrainException & e) 
        {
            m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_TTIS, "", true); 
            TA_RETHROW( e );
        }

        m_trainManager.getTrainStatusManager().setTTISStatus(trainID, TrainTypes::TTIS_Schedule);
        
        stopDownloadTimer( trainID );

		TrainTIS::LibraryDataType rawData;    
		for (unsigned int j = 0;j < libraryData.size();++j)
		{
			rawData.push_back(libraryData[j]);
		}
        
        TTDFrame::TtisFrameList frameList = TTDFrame::frameData(
            rawData, trainID, TTDFrame::ScheduleData, TTDFrame::MESSAGE_DESTINATION_ISCS);
        // Save it in train status manager
        m_trainManager.getTrainStatusManager().setDownloadLibrary(trainID, frameList);

        // Prepare the buffer for Radio
        std::vector<unsigned char> framedData;
        for (TTDFrame::TtisFrameList::iterator i = frameList.begin(); i != frameList.end(); ++i)
        {
            std::vector<unsigned char> frame = (*i).frameData;
            std::copy(frame.begin(), frame.end(), std::back_inserter(framedData));
        }
        // Do the Download - who does queueing?
        std::string ITSI = m_trainManager.getTrainStatusManager().getTrainTSI(trainID);
        m_notifier.callRadioSendData(ITSI, framedData, sessionID); //exc

    }
    
    void TrainTIS::scheduleUpgrade(unsigned char trainID,const char * sessionID)
    {
		TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncNOP);

        // verify the session
// not reqd        m_trainManager.getTrainStatusManager().verifyOwningSessionID(trainID, TrainStatus::Resource_TTIS, sessionID);

        // Send command to train
        ttisScheduleUpgradeCommand command; 
        
        command.setTrainID(trainID);
        
        try 
        {
            TrainEventPtr event = m_trainManager.sendTrainCommandWithWait(trainID, command, m_timerTIS1);
            
            // Check event reply
            ttisCommandReceivedEventReader* reader = dynamic_cast<ttisCommandReceivedEventReader*>(event->getReader());
            if (0 == reader)
            {
                TA_THROW( TrainGeneralFailureException() );
            }
            
            checkStatus(reader);

            unsigned int version = reader->getLibraryVersion();

            // Update the TrainStatusInfo with new schedule version
            m_trainManager.getTrainStatusManager().setTTISScheduleVersion(trainID, version);

        }
        catch (TrainException & e) 
        {
            // release the session
//            m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_TTIS, "", true); 
            TA_RETHROW( e );
        }
        // release the session
//        m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_TTIS, "", true); 

    }
    
    
    
    void TrainTIS::onTISEndOfDownload(TrainEvent & event, ttisEndOfDownloadEventReader * reader)
    {
        unsigned int trainID = event.getTrainID();

        // stop any running timers
        stopDownloadTimer( trainID );
		std::string ITSI = m_trainManager.getTrainStatusManager().getTrainTSI(trainID);

		TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncNOP);
        
		std::string sessionID = m_trainManager.getTrainStatusManager().getOwningSessionID(trainID, TrainStatus::Resource_TTIS);

        /// @todo rewrite switch to be nicer...
        bool success = false;
        switch(reader->getDownloadStatus())
        {
        case ttisEndOfDownloadEventReader::OK:
            success = true;
        case ttisEndOfDownloadEventReader::FAILED:
            // look at orig command to see what to do
            switch (m_trainManager.getTrainStatusManager().getTTISStatus(trainID))
            {
            case TrainTypes::TTIS_Library:
                m_notifier.notifyTIS(TIS_LibraryDownload, trainID, success, sessionID);
                break;
            case TrainTypes::TTIS_Schedule:
                m_notifier.notifyTIS(TIS_ScheduleDownload, trainID, success, sessionID);
                break;
            case TrainTypes::TTIS_Unknown:
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "TIMS responded too late with end of download for train %d", trainID);
                break;        
            }
            
            // cleanup
            m_trainManager.getTrainStatusManager().setTTISStatus(trainID, TrainTypes::TTIS_Unknown);
			// release the session
			m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_TTIS, "", true); 
			
			//hongzhi, CL20191, ECP531, cancel TTIS Library download when receive SDS message 39
			try
			{
	            m_notifier.callRadioCancelSendData(ITSI); //exc
			}
			catch (TrainGeneralFailureException& e)
			{
                LOG_EXCEPTION_CATCH(SourceInfo, "TrainGeneralFailureException", e.what());
			}
			catch (...)
			{
                LOG_EXCEPTION_CATCH(SourceInfo, "...", "While cancelling data call");
			}
			return;
            break;
        case ttisEndOfDownloadEventReader::REPETITION:

			//hongzhi, CL20191, ECP531, cancel TTIS Library download when receive SDS message 39
			try
			{
				m_notifier.callRadioCancelSendData(ITSI); //exc				
			}
			catch (TrainGeneralFailureException& e)
			{
                LOG_EXCEPTION_CATCH(SourceInfo, "TrainGeneralFailureException", e.what());
			}
			catch (...)
			{
                LOG_EXCEPTION_CATCH(SourceInfo, "...", "While cancelling data call");
			}

            if (m_trainManager.getTrainStatusManager().getTTISStatus(trainID) == TrainTypes::TTIS_Unknown)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "TIMS responded too late with end of download for train %d", trainID);

                // do not enter repetition
                return;
            }
            break;
        }

        // if we get here, we are in repetition mode...
        std::vector<unsigned int> repeat = reader->getRepeatedFrames(); 

        TTDFrame::TtisFrameList frameList = m_trainManager.getTrainStatusManager().getDownloadLibrary(trainID);
        
        // just log the frames to repeat
        std::stringstream repeatList;
        std::vector<unsigned int>::iterator i;
        for (i = repeat.begin(); i != repeat.end(); ++i)
        {
            repeatList << (*i) << " ";
        }
        repeatList << std::ends;

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
            "TIMS wants repetition of frames: %s", repeatList.str().c_str() );

        // Prepare the buffer for Radio
        std::vector<unsigned char> framedData;
        for (i = repeat.begin(); i != repeat.end(); ++i)
        {
            unsigned int frameNumber = (*i);
            if ( (frameNumber > frameList.size()) || (frameNumber == 0) )
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "TIMS wants repetition of non existent frame %d, ignoring frame.", frameNumber);
            }
            else
            {
                // frames are in order in the vector
                std::vector<unsigned char> frame = frameList[frameNumber - 1].frameData;
                std::copy(frame.begin(), frame.end(), std::back_inserter(framedData));
            }
        }

        if (framedData.size() > 0)
        {
            // Do the Download 
			//std::string ITSI = m_trainManager.getTrainStatusManager().getTrainTSI(trainID);
            m_notifier.callRadioSendData(ITSI, framedData, sessionID); //exc
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                "Cannot open circuit mode - there is no data to transfer. Perhaps TIMS asked for invalid frames.");

            // there was an error
            switch (m_trainManager.getTrainStatusManager().getTTISStatus(trainID))
            {
            case TrainTypes::TTIS_Library:
                m_notifier.notifyTIS(TIS_LibraryDownload, trainID, false, sessionID);
                break;
            case TrainTypes::TTIS_Schedule:
                m_notifier.notifyTIS(TIS_ScheduleDownload, trainID, false, sessionID);
                break;
            case TrainTypes::TTIS_Unknown:
                // ignore
                break;        
            }
            
            // cleanup
            m_trainManager.getTrainStatusManager().setTTISStatus(trainID, TrainTypes::TTIS_Unknown);
			// release the session
			m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_TTIS, "", true);
        }
    }


    void TrainTIS::handleTimeout(unsigned char trainId)
    {
        onDownloadTimeout(trainId);
    }

    void TrainTIS::startDownloadTimer(unsigned char trainId)
    {
        m_ttis2TimerManager.startTimer(trainId, m_timerTIS2, true, "TTISTimer");
    }

    void TrainTIS::stopDownloadTimer(unsigned char trainId)
    {
        m_ttis2TimerManager.endTimer(trainId);
    }

    void TrainTIS::onDownloadTimeout(unsigned char trainId)
    {
        // An expected TIMS reset request was not received

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
            "TIMS end of download not received for train %d, failing download", trainId );

        std::string sessionID = m_trainManager.getTrainStatusManager().getOwningSessionID(trainId, TrainStatus::Resource_TTIS);
        switch (m_trainManager.getTrainStatusManager().getTTISStatus(trainId))
        {
            case TrainTypes::TTIS_Library:
                m_notifier.notifyTIS(TrainTIS::TIS_LibraryDownload, trainId, false, sessionID);
                break;
            case TrainTypes::TTIS_Schedule:
                m_notifier.notifyTIS(TrainTIS::TIS_ScheduleDownload, trainId, false, sessionID);
                break;
            case TrainTypes::TTIS_Unknown:
                // ignore
                break;        
        }
    
        // cleanup
        m_trainManager.getTrainStatusManager().setTTISStatus(trainId, TrainTypes::TTIS_Unknown);
        // release the session
        m_trainManager.getTrainStatusManager().setOwningSessionID(trainId, TrainStatus::Resource_TTIS, "", true);

		//hongzhi, CL20191, ECP531, cancel TTIS Library download when receive SDS message 39
		std::string ITSI = m_trainManager.getTrainStatusManager().getTrainTSI(trainId);
		try
		{
			m_notifier.callRadioCancelSendData(ITSI); //exc
		}
		catch (TrainGeneralFailureException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TrainGeneralFailureException", e.what());
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "...", "While cancelling data call");
		}
		return;
    }

	//TD13214++
	// parallel command ACE methods
	std::string  TisDisplayFreeTextMessageCommand::info()
    {
		std::ostringstream s;
		s << "TisDisplayFreeTextMessageCommand [Train=" << static_cast<int>(m_trainID) << "]";
		
		return s.str();
	}
	
	std::string  TisDisplayStoredMessageCommand::info()
    {
		std::ostringstream s;
		s << "TisDisplayStoredMessageCommand [Train=" << static_cast<int>(m_trainID) << "]";
		
		return s.str();
	}
	
	
	std::string  TisResetMessageCommand::info()
    {
		std::ostringstream s;
		s << "TisResetMessageCommand [Train=" << static_cast<int>(m_trainID) << "]";
		
		return s.str();
	}


    int TisDisplayFreeTextMessageCommand::call()
    {
        FUNCTION_ENTRY("TisDisplayFreeTextMessageCommand::call()");

       	m_returnStatus->trainID = m_trainID;
        m_returnStatus->sessionID = m_sessionID;		
		m_returnStatus->success = true;
		m_returnStatus->errorDetails = "unknown error" ; 	

        // check whether to execute
        if ( m_operationModeManager.isInControlOfTrain(m_trainID) )
        {	
			std::string eventDesc("displayFreeTextMessage");

			try
			{
				// execute the command
				// for every pid, do it sequentially
				for (unsigned int i = 0 ; i <  m_PIDs.size(); ++i)
				{
					m_trainTis.displayFreeTextMessage(m_trainID,
						m_PIDs[i],
						m_displayAttribute,
						m_priority,
						m_displayPeriod,
						m_repeatInterval,
						m_message,
						m_sessionID);
				}

				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "TisDisplayFreeTextMessageCommand::call send the displayFreeTextMessage command for train:%d.",static_cast<int>(m_trainID));

			}
			catch (TrainInvalidTrainException& e)
			{
				
				std::stringstream s;
				s << "Specified train is invalid... " << e.what() << std::ends;
				// ss++
				// TD14374
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "displayFreeTextMessage throwing ITISTrainCommandsCorbaDef::InvalidParameter [%s]", s.str().c_str());
				// ++ss
				m_returnStatus->success = false;
                m_returnStatus->errorDetails = e.what();
				
				//	throw TA_Base_Bus::ITISTrainCommandsCorbaDef::InvalidParameter("Specified train is invalid");  
			}
			catch (TrainInvalidParameterException& e)
			{
                m_returnStatus->success = false;
                m_returnStatus->errorDetails = e.what();

				// ss++
				// TD14374
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "displayFreeTextMessage throwing ITISTrainCommandsCorbaDef::InvalidParameter [%s]", e.what());
				// ++ss
				// throw TA_Base_Bus::ITISTrainCommandsCorbaDef::InvalidParameter(e.what());
			}
			catch (TrainTimeoutException& e )
			{
                m_returnStatus->success = false;
                m_returnStatus->errorDetails = e.what();		

				TA_Base_Core::DescriptionParameters parameters;
				TA_Base_Core::NameValuePair nvpair("Event", eventDesc);

				parameters.push_back(&nvpair);
				m_notifier.AuditOperation(TA_Base_Core::TrainAudit::TrainTimeoutAuditEvent, parameters, m_sessionID);
				
				// ss++
				// TD14374
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "displayFreeTextMessage throwing ITISTrainCommandsCorbaDef::TrainTimeout");
				// ++ss
				//throw TA_Base_Bus::ITISTrainCommandsCorbaDef::TrainTimeout();
				
			}
			catch (TrainBusyException& e)
			{
			
                m_returnStatus->success = false;
                m_returnStatus->errorDetails = e.what();
				
				// ss++
				// TD14374
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "displayFreeTextMessage throwing ITISTrainCommandsCorbaDef::TrainBusy [%s]", e.what());
				// ++ss
				
				//throw TA_Base_Bus::ITISTrainCommandsCorbaDef::TrainBusy(e.override(), e.what());

			}
			catch (TrainGeneralFailureException& e)
			{
                m_returnStatus->success = false;
                m_returnStatus->errorDetails = e.what();
				
				// ss++
				// TD14374
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "displayFreeTextMessage throwing ITISTrainCommandsCorbaDef::TrainGeneralFailure [%s]", e.what());
				// ++ss

				//throw TA_Base_Bus::ITISTrainCommandsCorbaDef::TrainGeneralFailure(e.what());
			}
			catch (TrainException& e)
		    {
	    		m_returnStatus->success = false;
                m_returnStatus->errorDetails = e.what();

                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "displayFreeTextMessage met train exception [%s]", e.what());
		    }
			catch ( ... )
			{
                m_returnStatus->success = false;
                m_returnStatus->errorDetails = " displayAdhocMessage throwing unknown errors";

				// ss++
				// TD14374
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "displayAdhocMessage throwing unknown errors");
				// ++ss
				//throw TA_Base_Bus::ITISTrainCommandsCorbaDef::TrainGeneralFailure(e.what());
			}		
		
			try // TD13214
			{
				if (true == m_returnStatus->success)
				{
					// Now audit the event
					TA_Base_Core::DescriptionParameters parameters;
					
					std::stringstream trainIDStream;
					trainIDStream << static_cast<int>(m_trainID) << std::ends;
					
					TA_Base_Core::NameValuePair nvpair("Train", trainIDStream.str().c_str());
					parameters.push_back(&nvpair);
					
					m_notifier.AuditOperation(TA_Base_Core::TrainAudit::TrainTISFreeTextMessage,parameters,m_sessionID);			
				}
			}
			catch( ... )
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "displayFreeTextMessageCommand::call got a exception when auditing");
			}
        }
        else
        {
            m_returnStatus->success = false;
            m_returnStatus->errorDetails = "Not in control of this train";
			
			// ss++
			// TD14374
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "displayFreeTextMessage throws ITISTrainCommandsCorbaDef::TrainAccessDeniedException [%s]", MODEEXCEPTIONMESSAGE.c_str());
			// ++ss
			//throw TA_Base_Bus::ITISTrainCommandsCorbaDef::TrainAccessDeniedException(MODEEXCEPTIONMESSAGE.c_str());
        }
		// wait on the barrier
        m_barrier.post();

        FUNCTION_EXIT;
        return 0;
		
	}

	int TisDisplayStoredMessageCommand::call()
    {
        FUNCTION_ENTRY("TisDisplayStoredMessageCommand::call()");	
            
		m_returnStatus->trainID = m_trainID;
        m_returnStatus->sessionID = m_sessionID;		
		m_returnStatus->success = true;
		m_returnStatus->errorDetails = "unknown error" ; // this field is invalid,if success==true			
		
		 // check whether to execute
    	if ( m_operationModeManager.isInControlOfTrain(m_trainID) )
        {	
			//for audit
			std::string eventDesc("displayStoredMessage");
			std::stringstream PIDsStr;

			try
			{			
				// execute the command
				// for every pid, do it sequentially
				for (unsigned int i = 0; i <  m_PIDs.size();  ++i)
				{
					m_trainTis.displayStoredMessage(m_trainID, 
						m_PIDs[i] ,				
						m_libraryVersion,
						m_identifierType,
						m_identifierOffset,
						m_priority,
						m_displayPeriod,
						m_sessionID.c_str()	);

					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "TisDisplayStoredMessageCommand::call send the displayStoredTextMessage command for PID:%d,train:%d.",static_cast<int>(m_PIDs[i]), static_cast<int>(m_trainID));
				}				
			}			
			catch (TrainInvalidTrainException& e)
			{
				
				m_returnStatus->success = false;
				m_returnStatus->errorDetails = e.what();
				
				std::stringstream s;
				s << "Specified train is invalid... " << e.what() << std::ends;
				
				// ss++
				// TD14374
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "displayStoredMessage throwing ITISTrainCommandsCorbaDef::InvalidParameter [%s]", s.str().c_str());
				// ++ss
				
				//throw TA_Base_Bus::ITISTrainCommandsCorbaDef::InvalidParameter("Specified train is invalid");
			}
			catch (TrainInvalidParameterException& e)
			{
				
                m_returnStatus->success = false;
                m_returnStatus->errorDetails = e.what();
				
				// ss++
				// TD14374
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "displayStoredMessage throwing ITISTrainCommandsCorbaDef::InvalidParameter [%s]", e.what());
				// ++ss
				//throw TA_Base_Bus::ITISTrainCommandsCorbaDef::InvalidParameter(e.what());
			}
			catch (TrainTimeoutException& e )
			{			
                m_returnStatus->success = false;
               m_returnStatus->errorDetails = e.what();				
               	
				TA_Base_Core::DescriptionParameters parameters;
				TA_Base_Core::NameValuePair nvpair("Event", eventDesc);

				parameters.push_back(&nvpair);
				m_notifier.AuditOperation(TA_Base_Core::TrainAudit::TrainTimeoutAuditEvent, parameters, m_sessionID);
				
				// ss++
				// TD14374
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "displayStoredMessage throwing ITISTrainCommandsCorbaDef::TrainTimeout");
				// ++ss
				//throw TA_Base_Bus::ITISTrainCommandsCorbaDef::TrainTimeout();
			}
			catch (TrainBusyException& e)
			{
				m_returnStatus->success = false;
                m_returnStatus->errorDetails = e.what();
				
				// ss++
				// TD14374
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "displayStoredMessage throwing ITISTrainCommandsCorbaDef::TrainBusy [%s]", e.what());
				// ++ss
				//throw TA_Base_Bus::ITISTrainCommandsCorbaDef::TrainBusy(e.override(), e.what());
			}
			catch (TrainGeneralFailureException& e)
			{
                m_returnStatus->success = false;
                m_returnStatus->errorDetails = e.what();

				// ss++
				// TD14374
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "displayStoredMessage throwing ITISTrainCommandsCorbaDef::TrainGeneralFailure [%s]", e.what());
				// ++ss
				//throw TA_Base_Bus::ITISTrainCommandsCorbaDef::TrainGeneralFailure(e.what());
			}
			catch (TrainException& e)
		    {
	    		m_returnStatus->success = false;
                m_returnStatus->errorDetails = e.what();

                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "displayStoredMessage met train exception [%s]", e.what());
		    }
			catch ( ... )
			{
                m_returnStatus->success = false;
                m_returnStatus->errorDetails = " displayStoredMessage throwing unknown errors";

				// ss++
				// TD14374
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "displayStoredMessage throwing unknown errors");
				// ++ss
				//throw TA_Base_Bus::ITISTrainCommandsCorbaDef::TrainGeneralFailure(e.what());
			}		
		
            if (true == m_returnStatus->success)
            {         
				try 
				{
					// Now audit the event
					TA_Base_Core::DescriptionParameters parameters;
					std::stringstream trainIDstr;
					
					trainIDstr << static_cast<int>(m_trainID) << std::ends;
					TA_Base_Core::NameValuePair nvpair("Train", trainIDstr.str().c_str());
					
					parameters.push_back(&nvpair);
					
					TA_Base_Core::NameValuePair nvid("ID", PIDsStr.str().c_str());
					std::stringstream libStr;
					
					libStr << m_libraryVersion << std::ends;
					TA_Base_Core::NameValuePair nvlib("Library", libStr.str().c_str());
					
					parameters.push_back(&nvid);
					parameters.push_back(&nvlib);
					
					m_notifier.AuditOperation(TA_Base_Core::TrainAudit::TrainTISStoredMessage,parameters,m_sessionID);
				}
				catch( ... )
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "displayStoredMessageCommand::call got a exception when auditing");
				}
				
			}			
        }
        else
        {
            m_returnStatus->success = false;
            m_returnStatus->errorDetails = "Not in control of this train";

			// ss++
			// TD14374
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "displayStoredMessage throws ITISTrainCommandsCorbaDef::TrainAccessDeniedException [%s]", MODEEXCEPTIONMESSAGE.c_str());
			// ++ss
			//throw TA_Base_Bus::ITISTrainCommandsCorbaDef::TrainAccessDeniedException(MODEEXCEPTIONMESSAGE.c_str());
        }

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "displayStoredMessage::call finished for train:%d",m_trainID);

		m_barrier.post();		

        FUNCTION_EXIT;
        return 0;
		
	}

	int TisResetMessageCommand::call()
    {
     FUNCTION_ENTRY("TisResetMessageCommand::call()");

     m_returnStatus->trainID = m_trainID;
     m_returnStatus->sessionID = m_sessionID;		
	 m_returnStatus->success = true;
	 m_returnStatus->errorDetails = "unknown error" ; 	
	  
	  // check whether to execute
	  if ( m_operationModeManager.isInControlOfTrain(m_trainID) )
	  {			
		  std::string eventDesc("resetMessage");
		  
		  try
		  {
			  //execute the command
			  // for every pid, do it sequentially
			  for (unsigned int i = 0; i <  m_PIDs.size();  ++i)
			  {
				  
				  m_trainTis.resetMessage(m_trainID,m_PIDs[i],m_messageType);
			  }		

			  LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "TisResetMessageCommand::call send the reset command for train:%d.",m_trainID);

		  }
		  catch (TrainInvalidTrainException& e)
		  {
			  m_returnStatus->success = false;
			  m_returnStatus->errorDetails = e.what();

			  std::stringstream s;
			  s << "Specified train is invalid... " << e.what() << std::ends;

			  // ss++
			  // TD14374
			  LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "resetMessage throwing ITISTrainCommandsCorbaDef::InvalidParameter [%s]", s.str().c_str());
			  // ++ss
			  //throw TA_Base_Bus::ITISTrainCommandsCorbaDef::InvalidParameter("Specified train is invalid");   
		  }
		  catch (TrainInvalidParameterException& e)
		  {
			  m_returnStatus->success = false;
			  m_returnStatus->errorDetails = e.what();

			  // ss++
			  // TD14374
			  LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "resetMessage throwing ITISTrainCommandsCorbaDef::InvalidParameter [%s]", e.what());
			  // ++ss
			  //throw TA_Base_Bus::ITISTrainCommandsCorbaDef::InvalidParameter(e.what());
		  }
		  catch (TrainTimeoutException& e )
		  {
			  m_returnStatus->success = false;
			  m_returnStatus->errorDetails = e.what();
			  
			  TA_Base_Core::DescriptionParameters parameters;
			  TA_Base_Core::NameValuePair nvpair("Event", eventDesc);
			  parameters.push_back(&nvpair);

			  m_notifier.AuditOperation(TA_Base_Core::TrainAudit::TrainTimeoutAuditEvent, parameters, m_sessionID);

			  // ss++
			  // TD14374
			  LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "resetMessage throwing ITISTrainCommandsCorbaDef::TrainTimeout");
			  // ++ss
			  //throw TA_Base_Bus::ITISTrainCommandsCorbaDef::TrainTimeout();
		  }
		  catch (TrainBusyException& e)
		  {
			  m_returnStatus->success = false;
			  m_returnStatus->errorDetails = e.what();
			  
			  // ss++
			  // TD14374
			  LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "resetMessage throwing ITISTrainCommandsCorbaDef::TrainBusy [%s]", e.what());
			  // ++ss
			  //throw TA_Base_Bus::ITISTrainCommandsCorbaDef::TrainBusy(e.override(), e.what());
		  }
		  catch (TrainGeneralFailureException& e)
		  {
			  m_returnStatus->success = false;
			  m_returnStatus->errorDetails = e.what();
			  
			  // ss++
			  // TD14374
			  LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "resetMessage throwing ITISTrainCommandsCorbaDef::TrainGeneralFailure [%s]", e.what());
			  // ++ss
			  //throw TA_Base_Bus::ITISTrainCommandsCorbaDef::TrainGeneralFailure(e.what());
		  }
		  catch (TrainException& e)
		  {
			  m_returnStatus->success = false;
			  m_returnStatus->errorDetails = e.what();
			  
			  LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "resetMessage met train exception [%s]", e.what());
		  }
		  catch ( ... )
		  {
                m_returnStatus->success = false;
                m_returnStatus->errorDetails = " ResetMessage throwing unknown errors";

				// ss++
				// TD14374
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "ResetMessage throwing unknown errors");
				// ++ss
				//throw TA_Base_Bus::ITISTrainCommandsCorbaDef::TrainGeneralFailure(e.what());
		  }		
		  try
		  {
			  
			  if (true == m_returnStatus->success)
			  {
				  // Now audit the event
				  TA_Base_Core::DescriptionParameters parameters;
				  std::stringstream trainIDstr;
				  
				  trainIDstr << m_trainID << std::ends;
				  TA_Base_Core::NameValuePair nvpair("Train", trainIDstr.str().c_str());
				  
				  parameters.push_back(&nvpair);
				  m_notifier.AuditOperation(TA_Base_Core::TrainAudit::TrainTISResetMessage,parameters,m_sessionID);
			  }
		  }
		  catch( ... )
		  {
			  LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "ResetMessageCommand::call got a exception when auditing");
		  }
		  
	  }
	  else
	  {
		  m_returnStatus->success = false;
		  m_returnStatus->errorDetails = "Not in control of this train";
		  // ss++
		  // TD14374
		  LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "resetMessage throws ITISTrainCommandsCorbaDef::TrainAccessDeniedException [%s]", MODEEXCEPTIONMESSAGE.c_str());
		  // ++ss
		  //throw TA_Base_Bus::ITISTrainCommandsCorbaDef::TrainAccessDeniedException(MODEEXCEPTIONMESSAGE.c_str());
	  }
	  
	  // wait on the barrier
	  m_barrier.post();
	  
	  FUNCTION_EXIT;
	  return 0;
		
	}

	std::vector<TrainTIS::TisReturnStatus*> TrainTIS::displayFreeTextMessageParallel(
		const std::vector<unsigned char> trains,
		const std::vector<unsigned char> PIDs,
		unsigned char displayAttribute,
        unsigned char priority,
        const TimeInterval & displayPeriod,
        unsigned char repeatInterval,
        const char * message,
        const char * sessionID)
	{
	
		FUNCTION_ENTRY("TISdisplayFreeTextMessageParallel()");

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "received %d freetext message parallel display request.", static_cast<int>(trains.size()) );

        // the number of threads that need to block
        TrainBarrier barrier( trains.size() );

        std::vector<TisReturnStatus*> tisReturnValues;

        for (unsigned int i = 0; i < trains.size(); ++i)
        {
            // the caller, TrainAgent.cpp, is responsible to delete the heap memory.
			//initialise result in call method
			TisReturnStatus* result = new TisReturnStatus();

            tisReturnValues.push_back(result);

            TisDisplayFreeTextMessageCommand *x = 
                new TisDisplayFreeTextMessageCommand( *this,
				m_notifier,
				trains[i],
				PIDs,
				displayAttribute,
				priority,
				displayPeriod,
				repeatInterval,
				message,
				sessionID,
				barrier,
                tisReturnValues[i],
				m_operationModeManager);

            m_workQueueManager.perform(x);
        }
        // now wait for each command to finish

        barrier.wait();

        FUNCTION_EXIT;
        return tisReturnValues;
	}	
		
	

	std::vector<TrainTIS::TisReturnStatus*> TrainTIS::displayStoredMessageParallel(
		const std::vector<unsigned char> trains,
		const std::vector<unsigned char> PIDs,
		short libraryVersion,
        unsigned char identifierType,
        unsigned char identifierOffset,
        unsigned char priority,
        const TimeInterval  displayPeriod,
        const char * sessionID)
	{
		FUNCTION_ENTRY("TISdisplayFreeTextMessageParallel()");

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "received %d stored message parallel display request.",trains.size());
		unsigned int i ;	
	
		std::string strSessionID(sessionID);	
        // the number of threads that need to block

        TrainBarrier barrier( trains.size() );
		std::vector<TisReturnStatus*> tisReturnValues;

        for ( i = 0; i < trains.size(); ++i)
        {
			// the caller, TrainAgent.cpp, is responsible to delete the heap memory.
			//initial result in call method.
			TisReturnStatus* result = new TisReturnStatus();		
			
            tisReturnValues.push_back(result);

            TisDisplayStoredMessageCommand *x = 
                new TisDisplayStoredMessageCommand( *this,
				m_notifier,
				trains[i],
				PIDs,
				libraryVersion,
				identifierType,
				identifierOffset,
				priority,
				displayPeriod,	
				strSessionID,
				barrier,
				tisReturnValues[i],
				m_operationModeManager);			

            m_workQueueManager.perform(x);
        }
        // now wait for each command to finish

        barrier.wait();

        FUNCTION_EXIT;

        return tisReturnValues;
	}
	
	
	std::vector<TrainTIS::TisReturnStatus*> TrainTIS::resetMessageParallel(
		const std::vector<unsigned char> trains,
		const std::vector<unsigned char> PIDs,
		unsigned char messageType,
		const char * sessionID)
	{
		FUNCTION_ENTRY("TISdisplayFreeTextMessageParallel()");

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "received %d reset parallel request.",trains.size());

        // the number of threads that need to block
        TrainBarrier barrier( trains.size() );

		std::vector<TisReturnStatus*> tisReturnValues;

        for (unsigned int i = 0;  i < trains.size(); ++i)
        {			
			// the caller, TrainAgent.cpp, is responsible to delete the heap memory.
			//initial result in call method
			TisReturnStatus* result = new TisReturnStatus();

            tisReturnValues.push_back(result);

            TisResetMessageCommand *x = 
                new TisResetMessageCommand( *this,
				m_notifier,
				trains[i],
				PIDs,
				messageType,
				sessionID,
				barrier,
				tisReturnValues[i],
				m_operationModeManager);

            m_workQueueManager.perform(x);
        }
        // now wait for each command to finish

        barrier.wait();

        FUNCTION_EXIT;
		
		return tisReturnValues;
	}
//++TD13214


}  // namespace
