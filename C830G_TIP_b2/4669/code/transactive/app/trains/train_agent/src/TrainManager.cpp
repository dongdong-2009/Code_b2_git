/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/trains/train_agent/src/TrainManager.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

///////////////////////////////////////////////////////////
//  TrainManager.cpp
//  Implementation of the Class TrainManager
//  Created on:      26-Nov-2003 02:56:57 PM
///////////////////////////////////////////////////////////

//#include <varargs.h>
#include <sstream>

#include "TrainManager.h"
#include "TrainTypes.h"
#include "app/trains/train_agent/src/TARethrow.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/TrainEvent.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/TrainEventReader.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/TrainCommand.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/miscBadCommandEventReader.h"
#include "app/trains/train_agent/src/TrainException.h"
#include "app/trains/train_agent/src/TrainTimer.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "ITrainAgent.h"

using namespace TA_IRS_App;
using TA_Base_Core::RunParams;

namespace
{
    static const unsigned int NO_RETRIES = -1;
};

TrainManager::TrainManager(ITrainAgent& callback,ITrainStatusUpdateMonitor& statusUpdateMonitor, ITrainNotificationFacade& notifier) 
             : m_trainAgent(callback),
			   m_trainStatusManager(statusUpdateMonitor, notifier, callback.getGlobalTrainAgentEntity()->getTimsAutoContinueAckLimit()),
			   m_atsDelegator(0),
               m_calldriverDelegator(0),
               m_pecDelegator(0),
               m_paDelegator(0),
               m_downloadDelegator(0)
{
	FUNCTION_ENTRY("Constructor");

	m_trainTransactionManager.setWaitTimeout(getGlobalTrainAgentEntity()->getTimsSingleMessageTimeout()); /// @note my default for testing - to beat corba timeout

    m_trainTimer = new TrainTimer(m_trainTransactionManager);

	m_defaultRetryLimit = getGlobalTrainAgentEntity()->getTimsRetryLimit();

	// Load location/origin mapping into a cache, it won't change online.
	m_origin = TA_Base_Core::LocationAccessFactory::getInstance().getTIMSOriginValues();

	if (RunParams::getInstance().get(RPARAM_TrainTSIUpdateStrategy) == "always")
	{
		m_trainStatusManager.setTSIUpdateStrategy(new TrainTSIUpdateStrategyAlways(m_trainStatusManager));
	} 
	else if (RunParams::getInstance().get(RPARAM_TrainTSIUpdateStrategy) == "strict")
	{
		m_trainStatusManager.setTSIUpdateStrategy(new TrainTSIUpdateStrategyStrictWakeup(m_trainStatusManager));
	}
	else //default 
	{
		/// @todo by default, use 'strict', for easier testing, leave as 'always'
		/// @todo 'strictwakeup' has bugs -> uncaught exception due to no train on anything other than wakeup call
		m_trainStatusManager.setTSIUpdateStrategy(new TrainTSIUpdateStrategyAlways(m_trainStatusManager));
		// not yet... m_trainStatusManager.setTSIUpdateStrategy(new TrainTSIUpdateStrategyStrictWakeup(m_trainStatusManager));
	}

	addDelegate(&m_trainStatusManager);

	FUNCTION_EXIT;


}



TrainManager::~TrainManager()
{
	FUNCTION_ENTRY("Destructor");


	FUNCTION_EXIT;

}

void TrainManager::addListener(ITrainEventListener *listener)
{
    m_eventListeners.push_back(listener);
}


void TrainManager::addDelegate(ITrainEventReaderDelegator *delegate, bool priorityEvent) 
{
    /// @todo watch for duplicates
    if (priorityEvent) 
    {
        m_priorityEventReaderDelegators.push_back(delegate);
    }
    else
    {    
        m_eventReaderDelegators.push_back(delegate);
    }
}

void TrainManager::addDelegate(ITrainATSDelegator *delegate, bool priorityEvent) 
{
	m_atsDelegator = delegate;
}

void TrainManager::addDelegate(ITrainStateUpdateDelegator *delegate, bool priorityEvent) 
{
	m_stateUpdateDelegator = delegate;
}

void TrainManager::addDelegate(ITrainDutyDelegator *delegate, bool priorityEvent) 
{
	m_dutyDelegator = delegate;
}

void TrainManager::addDelegate(ITrainDownloadDelegator *delegate)
{
	m_downloadDelegator = delegate;
}

void TrainManager::addDelegate(ITrainCallEndDelegator *delegate, TrainStatus::ResourceCategory resource)
{
    switch (resource)
    {
    case TrainStatus::Resource_CallDriver:
        m_calldriverDelegator = delegate;
        break;
    case TrainStatus::Resource_PEC:
        m_pecDelegator = delegate;
        break;
    case TrainStatus::Resource_PA:
        m_paDelegator = delegate;
        break;
    default:
        break;
    }
}

inline TrainTimer & TrainManager::getTrainTimer()
{
    return *m_trainTimer;
}

void TrainManager::onInvalidTrainEvent(TrainID t)
{
	FUNCTION_ENTRY("onInvalidTrainEvent");
	// abort any waiting event for this train (hmm, how to do this ?!?)
	m_trainTransactionManager.abortTransaction(t);
	FUNCTION_EXIT;
}

/**
 * Used to pass a new train event to the class for processing.
 */
void TrainManager::onTrainEvent(TrainEvent& event)
{
	FUNCTION_ENTRY("onTrainEvent");
	// Update the ITSI (incase the active cab has changed)
//	onTrainTSI(event.getTrainID(),event.getTrainITSI(),event.getOrigin());

	// Handle the incoming messages
	TrainEventReader*  reader = event.getReader();

	try 
	{
		m_trainStatusManager.setTrainTSI(event, reader);
	} 
	catch (TrainInvalidParameterException &)
	{
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  
			TA_Base_Core::DebugUtil::DebugError, "Cannot process event due to invalid data.");
		return;
	}


    bool consumedMessage = false;

    
	// If the message has not been handled then lets see if the 
	// train transaction manager is waiting for it.
    if (!consumedMessage && !m_trainTransactionManager.onTrainEvent(event))
	{
		// Message still unhandled - could arrive after timeout so don't assert, just log it?
//        char s[100];
//        sprintf(s, 99, "Train message unhandled (%s)", typeid(reader).name());
//        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  
//            TA_Base_Core::DebugUtil::DebugWarn, "Train message unhandled by TrainTransactionManager.");
		//TA_ASSERT(false,"Train message unhandled.");
	}
    else 
    {
        consumedMessage = true;
    }


    if (!consumedMessage)
    {
        // Try one of the priority events (eg. alarm/status)
        // Check for error tables - these are the most common coming from each
        // train every 9 seconds.
        for (delegatorCollection::iterator i = m_priorityEventReaderDelegators.begin(); i != m_priorityEventReaderDelegators.end(); ++i)
        {
            consumedMessage = (*i)->delegate(event);
            if (consumedMessage)
            {
                break; // only one consumer of message allowed
            }
        }
    }

    // Try one of our delegators...
    if (!consumedMessage) 
    {
        for (delegatorCollection::iterator i = m_eventReaderDelegators.begin(); i != m_eventReaderDelegators.end(); ++i)
        {
            consumedMessage = (*i)->delegate(event);
            if (consumedMessage)
            {
                break; // only one consumer of message allowed
            }
        }
    }

    // If message still not consumed, ignore it.
    if (!consumedMessage)
    {
		std::stringstream s;
		s << "SDS message IN: Train message (" << typeid(reader).name() << ") from train " << static_cast<int>(event.getTrainID()) << " unhandled by TrainManager." << std::ends;
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, s.str().c_str());
    }

    //
    // Cycle through the event listeners
    //
    for (ListenerCollection::iterator k = m_eventListeners.begin(); k != m_eventListeners.end(); ++k)
    {
        (*k)->receive(event);
    }

    FUNCTION_EXIT;
}


void TrainManager::onATSEvent(ATSTrainMessage & event) 
{
	FUNCTION_ENTRY("onATSEvent");
	
	/** @todo delegate to:
	* - TrainStatusManager for basic processing, 
	* - and also to TrainMisc for radio changeover handling (degraded/duty)
	*/

	m_atsDelegator->delegate(event);

	FUNCTION_EXIT;
}

void TrainManager::onCallEndEvent(unsigned long callId)
{
    FUNCTION_ENTRY("onCallEndEvent");

    // get who is involved in this call
    std::vector<unsigned char> trains;
    TrainStatus::ResourceCategory resource;

    m_trainStatusManager.getCallParticipants(callId, trains, resource);

    if (trains.size() == 0)
    {
        FUNCTION_EXIT;
        return;
    }

    // remove from the list of calls
    m_trainStatusManager.removeCallFromWatchList(callId);

    // delegate to the correct listener
    switch (resource)
    {
    case TrainStatus::Resource_CallDriver:
        TA_ASSERT(m_calldriverDelegator != NULL, "Delegate is NULL");
        m_calldriverDelegator->delegate(callId, trains);
        break;
    case TrainStatus::Resource_PEC:
        TA_ASSERT(m_pecDelegator != NULL, "Delegate is NULL");
        m_pecDelegator->delegate(callId, trains);
        break;
    case TrainStatus::Resource_PA:
        TA_ASSERT(m_paDelegator != NULL, "Delegate is NULL");
        m_paDelegator->delegate(callId, trains);
        break;
    default:
        break;
    }
	
	FUNCTION_EXIT;
}

void TrainManager::onTrainStateUpdate(std::vector<TrainStatus::TrainInformation> & stateUpdate)
{
    TA_ASSERT(m_stateUpdateDelegator != NULL, "Delegate is NULL");
	m_stateUpdateDelegator->delegate(stateUpdate);
}

void TrainManager::onDutyEvent(DutyTrainMessage & event)
{
    TA_ASSERT(m_dutyDelegator != NULL, "Delegate is NULL");
	m_dutyDelegator->delegate(event);
}

void TrainManager::onTrainLibraryDownloadEvent(unsigned char trainId, bool success, const std::string& failReason)
{
    TA_ASSERT(m_downloadDelegator != NULL, "Delegate is NULL");
	m_downloadDelegator->delegate(trainId, success, failReason);
}


void TrainManager::sendTrainCommandNoReply(unsigned char trainID, TrainCommand& trainCommand)
{
	FUNCTION_ENTRY("sendTrainCommandNoReply");

	sendTrainCommand(trainID, trainCommand, 0, m_defaultRetryLimit, false);

	FUNCTION_EXIT;
}

/**
 * This method is used to send a train event and wait for the reply.  It will
 * first check to see that another client is not waiting for a reply.  If there is
 * the function will sleep and retry after 500ms.  Eventually the train will
 * become available because the transaction manager will timeout cleints not
 * getting a reply.
 */
TrainEventPtr TrainManager::sendTrainCommandWithWait(unsigned char trainID, TrainCommand& trainCommand, unsigned int timeOutMillis, unsigned int retryLimit)
{
	FUNCTION_ENTRY("sendTrainCommandWithWait");

	return sendTrainCommand(trainID, trainCommand, timeOutMillis, retryLimit, true);

	FUNCTION_EXIT;
}

TrainEventPtr TrainManager::sendTrainCommandWithWaitNoRetries(unsigned char trainID, TrainCommand& trainCommand, unsigned int timeOutMillis)
{
    FUNCTION_ENTRY("sendTrainCommandWithWaitNoRetries");

	return sendTrainCommand(trainID, trainCommand, timeOutMillis, NO_RETRIES, true);

	FUNCTION_EXIT;
}

TrainEventPtr TrainManager::sendTrainCommand(unsigned char trainID, TrainCommand& trainCommand, unsigned int timeOutMillis, unsigned int retryLimit, bool wait)
{
	FUNCTION_ENTRY("sendTrainCommand");
		
	TrainEventPtr event; //=NULL;
	std::string trainTSI;

	// to ensure certain common parameters are set...
	trainCommand.setTrainID(trainID);
	std::map<std::string, unsigned char>::iterator originIter = m_origin.find(getLocalTrainAgentEntity()->getLocationName());
	if (originIter == m_origin.end())
	{
		std::stringstream s;
		s << "TIMS origin cannot be determined for location [" << getLocalTrainAgentEntity()->getLocationName() << "]" << std::ends;
		TA_THROW(TrainInvalidParameterException(s.str().c_str()));
	}
	trainCommand.setOrigin((*originIter).second);

    bool msgOK = false;

    if (retryLimit == NO_RETRIES)
    {
        // make sure there are actually no retries
        retryLimit = 0;
    }
    else if (retryLimit < 1)
    {
        retryLimit = m_defaultRetryLimit; // force at least one attempt.
    }

    // ECP 255
    // retries have changed, retries are now re-tries, not tries.
    // simple fix is to add one to the limit, changing the logic below would be a bad idea.
    ++retryLimit;

    for (unsigned int i = 0; i < retryLimit && msgOK == false; ++i)
    {
        try
        {
			// Get prepared for reply
			m_trainTransactionManager.newTransaction(trainID, trainCommand.getReplyEventMessageNumber(), timeOutMillis);	
		}
        catch (TrainTimeoutException & e) // so we can clean ourselves up
        {
			LOG_EXCEPTION_CATCH(SourceInfo, "TrainTimeoutException", e.what());

			// Transaction wasn't created so we can't remove it.

			if ( (i+1) >= retryLimit )
			{
				{
					std::ostringstream s;
					s << "SDS message OUT: [failure] (reply expected) " << trainCommand << " due to exception " << e.what();
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
				}
				
				TA_RETHROW(e);
			}

			continue;
        }

		try
		{
			// Send the message to the train
            trainTSI = m_trainStatusManager.getTrainTSI(trainID); 
			{
				std::ostringstream s;
				s << "SDS message OUT: [sending] (reply expected) " << trainCommand;
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
			}

            m_trainAgent.sendMessageToTrain(trainTSI,trainCommand.getMessageData());
			
			{
				std::ostringstream s;
				s << "SDS message OUT: [success] (reply expected) " << trainCommand;
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
			}

            if (wait)
			{
				// Wait for the reply
				//event = 0; // just being pedantic
				event = m_trainTransactionManager.waitForTransaction(trainID);
			}
			msgOK = true;
        }
        catch (TrainTimeoutException & e) // so we can clean ourselves up
        {
			LOG_EXCEPTION_CATCH(SourceInfo, "TrainTimeoutException", e.what());

			if ( (i+1) >= retryLimit )
			{
				m_trainTransactionManager.removeTransaction(trainID);
				{
					std::ostringstream s;
					s << "SDS message OUT: [failure] (reply expected) " << trainCommand << " due to exception " << e.what();
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
				}
				
				TA_RETHROW(e);
			}
        }
		catch (TrainException & e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TrainException", e.what());

            if ( (i+1) >= retryLimit )
			{
                m_trainTransactionManager.removeTransaction(trainID);

                {
                    std::ostringstream s;
                    s << "SDS message OUT: [failure] (reply expected) " << trainCommand << " due to exception " << e.what();
                    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
                }

                TA_RETHROW(e);
            }
		}
		catch (std::exception & e) // should get TransactiveExceptions
		{
            if ( (i+1) >= retryLimit )
			{
                m_trainTransactionManager.removeTransaction(trainID);

                {
                    std::ostringstream s;
                    s << "SDS message OUT: [failure] (reply expected) " << trainCommand << " due to exception " << typeid(e).name();
                    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
                }
			
                TA_RETHROW(e);
            }
		}
		catch (...)
		{
            if ( (i+1) >= retryLimit )
            {
                m_trainTransactionManager.removeTransaction(trainID);

                {
                    std::ostringstream s;
                    s << "SDS message OUT: [failure] (reply expected) " << trainCommand << " due to unknown exception";
                    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
                }
                LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  
                TA_Base_Core::DebugUtil::DebugError, "Unexpected exception ... removing transaction");

                throw; // pass it along
            }
		}
        
		m_trainTransactionManager.removeTransaction(trainID);
        
        // Check for (66) Bad Command
		if (event.get() != 0) // will be zero if we timed out
		{
			miscBadCommandEventReader* reader = dynamic_cast<miscBadCommandEventReader*>(event->getReader());
			if (0 != reader)
			{
				{
					std::ostringstream s;
					s << "SDS message  IN: [failure] [BAD_COMMAND] " << event;
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
				}
				switch (reader->getErrorCode())
				{
				case miscBadCommandEventReader::BAD_CHECKSUM:
					if ( (i+1) >= retryLimit )
					{
						TA_THROW(TrainGeneralFailureException("", TrainGeneralFailureException::BAD_COMMAND));
					}
					continue; // the only error, for which we can retry
				case miscBadCommandEventReader::UNKNOWN_COMMAND:
					TA_THROW(TrainGeneralFailureException("", TrainGeneralFailureException::BAD_COMMAND));
					break;
				case miscBadCommandEventReader::BAD_PARAMETERS:
					TA_THROW(TrainInvalidParameterException());
					break;
				case miscBadCommandEventReader::UNEXPECTED_COMMAND:
					TA_THROW(TrainGeneralFailureException());
					break;
				default:
					TA_THROW(TrainGeneralFailureException());
				}
			}
		}
    }

	// just in case - although it should never be false here
	if (!msgOK) 
	{
		TA_THROW(TrainGeneralFailureException("Message not OK, this shouldn't happen"));
	}

	FUNCTION_EXIT;
	return event;
}

TA_Base_Core::TrainSettingsEntityDataPtr TrainManager::getGlobalTrainAgentEntity()
{
	return m_trainAgent.getGlobalTrainAgentEntity();
}

TA_Base_Core::TrainAgentEntityDataPtr TrainManager::getLocalTrainAgentEntity()
{
	return m_trainAgent.getLocalTrainAgentEntity();
}

#if 0
/**
 * This method will be called when a train location changes.  This will handle the
 * sending of the ChangeArea (if DCC<>Mainline) or TETRA/ISCS mode (if
 * degraded<>normal) command to the train.
 */
void TrainManager::onTrainLocationChange()
{
	FUNCTION_ENTRY("onTrainLocationChange");
	FUNCTION_EXIT;
}
#endif

/**
 * This method is called to set the TSI for the train.  It will be called for each
 * incoming message. 
 */
void TrainManager::onTrainTSI(unsigned int trainId, std::string subscriberId, unsigned char origin)
{
	FUNCTION_ENTRY("onTrainTSI");
	// Send new TSI to the Train Status Manager
	TrainTypes::MPUState mpu;
	switch (origin)
	{
	case 0:
		mpu = TrainTypes::MPU_Primary;
		break;
	case 1:
		mpu = TrainTypes::MPU_Secondary;
		break;
	default:
		mpu = TrainTypes::MPU_Unknown;
	}
//    m_trainStatusManager.setTrainTSI(trainId, subscriberId, mpu);
    FUNCTION_EXIT;
}

#if 0
/**
 * This method is called by the servant to notify the object of changes to the
 * train information from ATS. 
 */
void TrainManager::setTrainDetails(unsigned int trainID, unsigned long currentLocation, std::string serviceNumber, bool inBound, bool isMute)
{
	FUNCTION_ENTRY("setTrainDetails");
	FUNCTION_EXIT;
}
#endif

#if 0
/**
 * This method is called when a TIMS change over event is received.
 */
void TrainManager::onTIMSChangeoverEvent()
{
	FUNCTION_ENTRY("onTIMSChangeoverEvent");
	FUNCTION_EXIT;
}


/**
 * This method will be called when a train test call event is received.  It will
 * send a test call result reply to confirm receipt of the message.
 */
void TrainManager::onTrainTestCallEvent()
{
	FUNCTION_ENTRY("onTrainTestCallEvent");
	FUNCTION_EXIT;
}
#endif

/**
 * Get the train status manager to enable updates to be made.
 */
TrainStatusManager& TrainManager::getTrainStatusManager()
{
//	FUNCTION_ENTRY("getTrainStatusManager");
//	FUNCTION_EXIT;
	return m_trainStatusManager;
}


/**
 * Get the train transaction manager.
 */
TrainTransactionManager& TrainManager::getTrainTransactionManager()
{
//	FUNCTION_ENTRY("getTrainTransactionManager");
//	FUNCTION_EXIT;
	return m_trainTransactionManager;
}


