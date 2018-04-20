/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/trains/train_agent/src/TrainCallDriver.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */

///////////////////////////////////////////////////////////
//  TrainCallDriver.cpp
//  Implementation of the Class TrainCallDriver
//  Created on:      26-Nov-2003 02:15:47 PM
///////////////////////////////////////////////////////////

#include "TrainCallDriver.h"
#include "ITrainManager.h"
#include "app/trains/train_agent/src/TARethrow.h"
#include "app/trains/train_agent/src/TrainStatusManager.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/TrainEvent.h"
#include "app/trains/train_agent/src/TrainException.h"
#include "app/trains/train_agent/src/OperationModeManager.h"
#include "app/trains/train_agent/src/TrainWorkQueue.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/callCommandReceivedCommand.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/callEndOfOCCCallEventReader.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/callRequestCallTrainCommand.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/callRequestForOCCCallEventReader.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/callResetCommand.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/callTrainCommandReceivedEventReader.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/callEndVoiceCallCommand.h"
#include "core/data_access_interface/entity_access/src/TrainSettingsEntityData.h"
#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/message/types/TrainAudit_MessageTypes.h"

using namespace TA_IRS_App;


TrainCallDriver::TrainCallDriver( ITrainManager& callback,
                                  ITrainNotificationFacade& notifier,
                                  TrainWorkQueueManager& trainWorkQueueManager ) 
: m_trainManager(callback),
  m_notifier(notifier),
  m_workQueueManager(trainWorkQueueManager)
{
    m_trainManager.addDelegate( dynamic_cast<ITrainEventReaderDelegator*>(this) );
    m_trainManager.addDelegate( dynamic_cast<ITrainCallEndDelegator*>(this), TrainStatus::Resource_CallDriver );

    /// @todo Get VCx Timer values (from TrainAgent entity data ?!?)
//    m_retryLimit = 3;
    m_timerVC1 = m_trainManager.getGlobalTrainAgentEntity()->getTimerVC1();
    m_timerVC2 = m_trainManager.getGlobalTrainAgentEntity()->getTimerVC2();

}



TrainCallDriver::~TrainCallDriver()
{

}

bool TrainCallDriver::delegate(TrainEvent& event)
{
    TrainEventReader* r = event.getReader();
    bool consumed = true;
    if (dynamic_cast<callEndOfOCCCallEventReader*>(r) != 0)
    {
        onCallEndOfOCCCall(event, dynamic_cast<callEndOfOCCCallEventReader*>(r));
    }
    else if (dynamic_cast<callRequestForOCCCallEventReader*>(r) != 0)
    {
        onCallRequestForOCCCall(event, dynamic_cast<callRequestForOCCCallEventReader*>(r));
    }
/*
    else if (dynamic_cast<callTrainCommandReceivedEventReader*>(r) != 0)
    {
        onCallTrainCommandReceived(event, dynamic_cast<callTrainCommandReceivedEventReader*>(r));
    }
*/
    else 
    {
        consumed = false;
    }
    return consumed;
}


bool TrainCallDriver::delegate(unsigned long callId, std::vector<unsigned char>& trains)
{
    // A driver call has been disconnected

    // single train
    if (trains.size() == 1)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
            "Audio lost for OCC/DRIVER call on train %d, cleaning up.", trains[0] );
	    
	    TrainStatus::ECallDriverType callType = m_trainManager.getTrainStatusManager().getCallDriverType(trains[0]);
	    std::string sessionID = m_trainManager.getTrainStatusManager().getOwningSessionID(trains[0], TrainStatus::Resource_CallDriver);

        try 
	    {
	        // reset the callId in the train status
            m_trainManager.getTrainStatusManager().setCallAllocation(trains[0], TrainStatus::Resource_CallDriver, 0);
            m_trainManager.getTrainStatusManager().setCallDriverType(trains[0], TrainStatus::None);
            m_trainManager.getTrainStatusManager().setOwningSessionID(trains[0], TrainStatus::Resource_CallDriver, "", true);
            
            // send (79) end of voice call
            EndDriverCallCommand *x =
                new EndDriverCallCommand( *this,
                                          m_notifier,
                                          trains[0],
                                          sessionID,
                                          m_trainManager );

            m_workQueueManager.perform(x);
	    }
	    catch (...)
	    {
	    }
    }
    // multiple trains
    else
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
            "Audio lost for DRIVER call on multiple trains, cleaning up.");

        for (std::vector<unsigned char>::const_iterator iter = trains.begin();
            iter != trains.end(); ++iter)
        {
            std::string sessionID = m_trainManager.getTrainStatusManager().getOwningSessionID(*iter, TrainStatus::Resource_CallDriver);
            
            m_trainManager.getTrainStatusManager().setCallAllocation(*iter, TrainStatus::Resource_CallDriver, 0);
    	    m_trainManager.getTrainStatusManager().setCallDriverType(*iter, TrainStatus::None);
            m_trainManager.getTrainStatusManager().setOwningSessionID(*iter, TrainStatus::Resource_CallDriver, "", true);
            
            EndDriverCallCommand *x =
                new EndDriverCallCommand( *this,
                                          m_notifier,
                                          *iter,
                                          sessionID,
                                          m_trainManager );

            m_workQueueManager.perform(x);
        }
    }

    return true;
}


void TrainCallDriver::setupDriverCall(unsigned char trainID, bool override, std::string sessionID)
{
    setupDriverCallInternal(trainID, override, sessionID, true);
}

void TrainCallDriver::setupDriverCallInternal(unsigned char trainID, bool override, const std::string& sessionID, bool makeCall)
{
	TrainStateUpdateObserver(m_trainManager.getTrainStatusManager(), trainID, syncCallDriver);

    // Send command to train
    callRequestCallTrainCommand command; 
    
    if (override) 
    {
        command.setRequestCallStatus(callRequestCallTrainCommand::OVERRIDE);
    } 
    else
    {
        command.setRequestCallStatus(callRequestCallTrainCommand::OVERRIDE);
    }

    try
    {
        TrainEventPtr event = m_trainManager.sendTrainCommandWithWait(trainID, command, m_timerVC1);
        
        // Check event reply
        callTrainCommandReceivedEventReader* reader = dynamic_cast<callTrainCommandReceivedEventReader*>(event->getReader());
        if (0 == reader) 
        {
            /// @todo Cleanup TrainStatus, release any resources
            throw TrainGeneralFailureException();
        }

        switch (reader->getCommandReceivedStatus())
        {
        case callTrainCommandReceivedEventReader::CALL_TRAIN_PROCEEDING:
            // situation normal
			{
                // attempt to own the session
                m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_CallDriver, sessionID);

                if (makeCall)
                {
				    // now establish the audio - needs to be done here just in case the driver ends the calls
				    unsigned long callId = m_notifier.callRadioMakeCall(trainID, ITrainNotificationFacade::RADIO_DRIVER, sessionID);
				    if (callId == 0)
				    {
					    TA_THROW( TrainGeneralFailureException() );
				    }

				    // set the callId in the train status
				    m_trainManager.getTrainStatusManager().setCallAllocation(trainID, TrainStatus::Resource_CallDriver, callId);
				    m_trainManager.getTrainStatusManager().setCallDriverType(trainID, TrainStatus::OccCallTrain);
                }
			}
            break;
        case callTrainCommandReceivedEventReader::AUDIO_ROUTE_IN_USE:
            throw TrainBusyException("",false);
            break;
        case callTrainCommandReceivedEventReader::AUDIO_ROUTE_IN_USE_OVERRIDE:
            throw TrainBusyException("",true);
            break;
        case callTrainCommandReceivedEventReader::CALL_TRAIN_RESET:
            throw TrainGeneralFailureException("", TrainGeneralFailureException::RESET);
        }
    }
    catch (TrainMessageTimeoutException& e) 
    {
        TA_RETHROW( e ); 
    }
    
}


void TrainCallDriver::endDriverCall(unsigned char trainID, std::string sessionID)
{
	FUNCTION_ENTRY("endDriverCall");

	// check whether there is a call
	unsigned long callId = 
    m_trainManager.getTrainStatusManager().getCallAllocation(trainID, TrainStatus::Resource_CallDriver);

	if (callId == 0)
	{
		TA_THROW( TrainInvalidParameterException("No train call in progress for this train - must be another type of call to the train") );
	}

    endDriverCallInternal(trainID, sessionID);


	FUNCTION_EXIT;
}


void TrainCallDriver::endDriverCallInternal(unsigned char trainID, std::string sessionID)
{
    TrainStateUpdateObserver(m_trainManager.getTrainStatusManager(), trainID, syncCallDriver);

    unsigned long callId = 
    m_trainManager.getTrainStatusManager().getCallAllocation(trainID, TrainStatus::Resource_CallDriver);

    if (callId != 0)
    {
	    try
	    {
		    // Close down audio
		    m_notifier.callRadioReleaseCall(callId, sessionID);
	    }
	    catch (...)
	    {
		    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
			    "Audio Call %d not ended - cut off elsewhere. Sending SDS to TIMS anyway.", callId);
	    }
    }

	// reset the callId in the train status
	m_trainManager.getTrainStatusManager().setCallAllocation(trainID, TrainStatus::Resource_CallDriver, 0);
	m_trainManager.getTrainStatusManager().setCallDriverType(trainID, TrainStatus::None);

    // Send command to train
    callEndVoiceCallCommand command; 

    try
    {
        TrainEventPtr event = m_trainManager.sendTrainCommandWithWait(trainID, command, m_timerVC2);
        
        // Check event reply
        callTrainCommandReceivedEventReader* reader = dynamic_cast<callTrainCommandReceivedEventReader*>(event->getReader());
        if (0 == reader) 
        {
            /// @todo Cleanup TrainStatus, release any resources
            throw TrainGeneralFailureException();
        }
        
        // no status to check
    }
    catch (TrainMessageTimeoutException& e) 
    {
		m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_CallDriver, "", true);
        TA_RETHROW( e ); 
    }

	m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_CallDriver, "", true);
}


// TIP5 change, allow radio manager to trigger reply sds
void TrainCallDriver::answerDriverCall(unsigned char trainID, std::string sessionID)
{
	TrainStateUpdateObserver(m_trainManager.getTrainStatusManager(), trainID, syncCallDriver);

	// send (63) OCC Call Command Received
    callCommandReceivedCommand command; 

	m_trainManager.sendTrainCommandNoReply(trainID, command);

    // attempt to own the session
    m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_CallDriver, sessionID);

	// now establish the audio - needs to be done here just in case the driver ends the call
    unsigned long callId = m_notifier.callRadioMakeCall(trainID, ITrainNotificationFacade::RADIO_DRIVER, sessionID);
	if (callId == 0)
	{
		TA_THROW( TrainGeneralFailureException() );
	}

	// set the callId in the train status
	m_trainManager.getTrainStatusManager().setCallAllocation(trainID, TrainStatus::Resource_CallDriver, callId);
	m_trainManager.getTrainStatusManager().setCallDriverType(trainID, TrainStatus::TrainCallOcc);
}


void TrainCallDriver::onCallEndOfOCCCall(TrainEvent & event, callEndOfOCCCallEventReader * reader)
{
	FUNCTION_ENTRY("onCallEndOfOCCCall");

    unsigned int trainID = event.getTrainID();

	TrainStateUpdateObserver(m_trainManager.getTrainStatusManager(), trainID, syncCallDriver);

	// Close down audio
	try
	{
		// close down audio
		unsigned long callId = 
		m_trainManager.getTrainStatusManager().getCallAllocation(trainID, TrainStatus::Resource_CallDriver);

		// get the session ID that has the call
		std::string sessionID = m_trainManager.getTrainStatusManager().getOwningSessionID(trainID, TrainStatus::Resource_CallDriver);

		if ( (callId != 0) && (sessionID != "") )
		{
			m_notifier.callRadioReleaseCall(callId, sessionID);
		}
	}
	catch (...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
			"Train call not ended - cut off elsewhere. Sending SDS to TIMS anyway.");
	}

	// reset the callId in the train status
	m_trainManager.getTrainStatusManager().setCallAllocation(trainID, TrainStatus::Resource_CallDriver, 0);
	m_trainManager.getTrainStatusManager().setCallDriverType(trainID, TrainStatus::None);

    // send (69) OCC Call Reset
    callResetCommand command; 

    m_trainManager.sendTrainCommandNoReply(trainID, command);

	m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_CallDriver, "", true);

    // Call Radio -> driver call ending
	try 
	{
		m_notifier.callRadioCallDriver(false, trainID);
	}
	catch (...)
	{
	}

	FUNCTION_EXIT;
}


void TrainCallDriver::onCallRequestForOCCCall(TrainEvent & event, callRequestForOCCCallEventReader * reader)
{
    unsigned int trainID = event.getTrainID();

    TrainStateUpdateObserver(m_trainManager.getTrainStatusManager(), trainID, syncCallDriver);

	// send (63) OCC Call Command Received
    callCommandReceivedCommand command; 

	// TIP5 - don't automatically reply to the SDS -- RadioManager should do this
//    m_trainManager.sendTrainCommandNoReply(trainID, command);

    // Call Radio -> driver call waiting
	try 
	{
		m_notifier.callRadioCallDriver(true, trainID);
	}
	catch (...)
	{
	}
}



// parallel commands for multiple trains
std::vector<unsigned char> TrainCallDriver::setupDriverCallParallel(
    std::vector<unsigned char> trains,
    bool override,
    const std::string& sessionID,
    const std::string& groupTSI )
{
    FUNCTION_ENTRY("setupDriverCallParallel()");

    // the number of threads that need to block
    ACE_Barrier barrier(trains.size() + 1);

    // make the call on each train
    std::vector<unsigned char>::const_iterator iter;
    for (iter = trains.begin(); iter != trains.end(); ++iter)
    {
        SetupDriverCallCommand *x =
            new SetupDriverCallCommand( *this,
                                      m_notifier,
                                      *iter,
                                      override,
                                      sessionID,
                                      barrier,
                                      m_trainManager );

        m_workQueueManager.perform(x);
    }

    // now wait for each command to finish
    barrier.wait();

    std::vector<unsigned char> successfulTrains;

    // if successful, add to successful list
    for (iter = trains.begin(); iter != trains.end(); ++iter)
    {
        // it is successful if the owning session id matches
        if (m_trainManager.getTrainStatusManager().amOwningSessionID(*iter, TrainStatus::Resource_CallDriver, sessionID) )
        {
            successfulTrains.push_back( *iter );
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "SetupDriverCallCommand successful for train %d.", *iter);
        }
    }

    if ( successfulTrains.size() > 0 )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "SetupDriverCallCommand successful for %d trains, making call.", successfulTrains.size() );
            
        // make the call
        std::vector<std::string> ITSIList;
        ITSIList.push_back(groupTSI);
        unsigned long callId = m_notifier.callRadioMakeCall(ITSIList, ITrainNotificationFacade::RADIO_DRIVER_GROUP, sessionID);

        if (callId == 0)
		{
            // reset the owning session id
            for (iter = successfulTrains.begin(); iter != successfulTrains.end(); ++iter)
            {
                m_trainManager.getTrainStatusManager().setOwningSessionID(*iter, TrainStatus::Resource_CallDriver, "", true);
            }

			TA_THROW( TrainGeneralFailureException("Failure to make call") );
		}

        // set the call Id in each trains status
        for (iter = successfulTrains.begin(); iter != successfulTrains.end(); ++iter)
        {
            m_trainManager.getTrainStatusManager().setCallAllocation(*iter, TrainStatus::Resource_CallDriver, callId);
            m_trainManager.getTrainStatusManager().setCallDriverType(*iter, TrainStatus::OccCallTrain);
        }
    }
    else
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "SetupDriverCallCommand not successful for any trains, not making call." );
            
        TA_THROW( TrainGeneralFailureException("No Trains answered request") );
    }
    
    FUNCTION_EXIT;
    return successfulTrains;
}


void TrainCallDriver::endDriverCallParallel(
        std::vector<unsigned char> trains,
        const std::string& sessionID,
        unsigned long callId )
{
    FUNCTION_ENTRY("endDriverCallParallel()");
    
    // make the call on each train
    // the first will end the audio

    // the number of threads that need to block
    ACE_Barrier barrier(trains.size() + 1);

    for (std::vector<unsigned char>::const_iterator iter = trains.begin();
        iter != trains.end(); ++iter)
    {
        EndDriverCallCommand *x =
            new EndDriverCallCommand( *this,
                                      m_notifier,
                                      *iter,
                                      sessionID,
                                      barrier,
                                      m_trainManager );

        m_workQueueManager.perform(x);
    }

    // now wait for each command to finish
    barrier.wait();

    FUNCTION_EXIT;
}

std::string SetupDriverCallCommand::info()
{
	std::ostringstream s;
	s << "SetupDriverCallCommand [Train=" << static_cast<int>(m_trainId) << "]";

	return s.str();
}

int SetupDriverCallCommand::call()
{
    FUNCTION_ENTRY("SetupDriverCallCommand::call()");

    try
	{
        // simple, execute the command
        m_trainCallDriver.setupDriverCallInternal( m_trainId, m_override, m_sessionId, false );

        // audit the event
        TA_Base_Core::DescriptionParameters parameters;

	    std::stringstream trainIDstr;
	    trainIDstr << static_cast<int>(m_trainId) << "/" << m_trainManager.getTrainStatusManager().getTrainServiceNumber(m_trainId) << std::ends;
	    TA_Base_Core::NameValuePair nvpair("Train", trainIDstr.str().c_str());
	    parameters.push_back(&nvpair);

	    TA_Base_Core::NameValuePair nvlocn("Location", m_trainManager.getLocalTrainAgentEntity()->getLocationName().c_str());
	    parameters.push_back(&nvlocn);

	    m_notify.AuditOperation(TA_Base_Core::TrainAudit::TrainCallDriverSetupCall,parameters,m_sessionId);
    }
    catch (TrainException& e)
	{
        LOG_EXCEPTION_CATCH(SourceInfo, "TrainException", e.what());
	}
	catch (...)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "<unknown exception>", "Error in CallDriver worker thread");
	}

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
        "SetupDriverCallCommand completed for train %d, waiting for others to complete.", m_trainId);

    // wait on the barrier
    m_barrier.wait();

    FUNCTION_EXIT;
    return 0;
}

std::string EndDriverCallCommand::info()
{
	std::ostringstream s;
	s << "EndDriverCallCommand [Train=" << static_cast<int>(m_trainId) << "]";

	return s.str();
}

int EndDriverCallCommand::call()
{
    FUNCTION_ENTRY("EndDriverCallCommand::call()");

    try
	{
        // simple, execute the command
        m_trainCallDriver.endDriverCallInternal( m_trainId, m_sessionId );


        // audit the event
        TA_Base_Core::DescriptionParameters parameters;

	    std::stringstream trainIDstr;
	    trainIDstr << static_cast<int>(m_trainId) << "/" << m_trainManager.getTrainStatusManager().getTrainServiceNumber(m_trainId) << std::ends;
	    TA_Base_Core::NameValuePair nvpair("Train", trainIDstr.str().c_str());
	    parameters.push_back(&nvpair);

	    TA_Base_Core::NameValuePair nvlocn("Location", m_trainManager.getLocalTrainAgentEntity()->getLocationName().c_str());
	    parameters.push_back(&nvlocn);

	    m_notify.AuditOperation(TA_Base_Core::TrainAudit::TrainCallDriverEndCall,parameters,m_sessionId);
	}
    catch (TrainException& e)
	{
        LOG_EXCEPTION_CATCH(SourceInfo, "TrainException", e.what());
	}
	catch (...)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "<unknown exception>", "Error in CallDriver worker thread");
	}

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
        "EndDriverCallCommand completed for train %d, waiting for others to complete.", m_trainId);

    // wait on the barrier
    if (m_barrier != NULL)
    {
        m_barrier->wait();
    }

    FUNCTION_EXIT;
    return 0;
}


