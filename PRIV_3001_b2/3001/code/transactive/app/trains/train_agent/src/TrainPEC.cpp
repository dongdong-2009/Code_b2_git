/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/trains/train_agent/src/TrainPEC.cpp $
  * @author:  Glen Kidd
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * <description>
  *
  */

///////////////////////////////////////////////////////////
//  TrainPEC.cpp
//  Implementation of the Class TrainPEC
//  Created on:      26-Nov-2003 02:15:47 PM
///////////////////////////////////////////////////////////

#include <sstream>
#include <time.h>

#include "ace/ACE.h"

#include "TrainPEC.h"
#include "ITrainManager.h"
#include "app/trains/train_agent/src/TARethrow.h"
#include "app/trains/train_agent/src/TrainException.h"
#include "app/trains/train_agent/src/TrainTimer.h"
#include "app/trains/train_agent/src/TrainManager.h"
#include "app/trains/train_agent/src/OperationModeManager.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/TrainEvent.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/pecActivatedEventReader.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/pecAnswerCommand.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/pecCommandReceivedEventReader.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/pecContinueCommand.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/pecPECNumberReader.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/pecReadyForConversationEventReader.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/pecRequestForResetEventReader.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/pecResetCommand.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/pecResetEventReader.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/pecSelectedByDriverEventReader.h"
#include "core/data_access_interface/entity_access/src/TrainSettingsEntityData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/message/types/TrainAudit_MessageTypes.h"
#include "core/utilities/src/RunParams.h"//TD17298 liqiang++

// ss++
// TD #13763
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/ILocation.h"
// ++ss

using namespace TA_IRS_App;

namespace
{
	/// Helper method to eliminate some duplication of code
	void removePECFromCollection( PECPointCollection & collection, const PECPoint & point )
	{
	    PECPointCollection::iterator i = std::find_if(
			collection.begin(), 
			collection.end(), 
			std::bind2nd(TrainStatusManager::comparePECPoint(), point));
		if (i != collection.end())
		{
			collection.erase(i);
		}
	};
}


TrainPEC::TrainPEC(ITrainManager& callback, ITrainNotificationFacade& notifier, OperationModeManager& operationModeManager) : 
m_trainManager(callback),
m_notifier(notifier),
m_operationModeManager(operationModeManager)//,
//m_pec3TimerManager(*this)
{
    m_trainManager.addDelegate( dynamic_cast<ITrainEventReaderDelegator*>(this) );
    m_trainManager.addDelegate( dynamic_cast<ITrainCallEndDelegator*>(this), TrainStatus::Resource_PEC );

    // Get PECx Timer values (from TrainAgent entity data)
    m_timerPEC2 = m_trainManager.getGlobalTrainAgentEntity()->getTimerPEC2();
    m_timerPEC3 = m_trainManager.getGlobalTrainAgentEntity()->getTimerPEC3();
    m_timerPEC4 = m_trainManager.getGlobalTrainAgentEntity()->getTimerPEC4();
    m_retryLimit = m_trainManager.getGlobalTrainAgentEntity()->getTimsRetryLimit();
}



TrainPEC::~TrainPEC()
{
}

//ITrainEventReaderDelegator::delegatorCallback TrainPEC::delegate(TrainEvent& event)
bool TrainPEC::delegate(TrainEvent& event)
{
    TrainEventReader* r = event.getReader();
    bool consumed = true;
    if (dynamic_cast<pecActivatedEventReader*>(r) != 0)
    {
        onPECActivation(event, dynamic_cast<pecActivatedEventReader*>(r));
    }
    else if (dynamic_cast<pecSelectedByDriverEventReader*>(r) != 0)
    {
        onPECDriverSelection(event, dynamic_cast<pecSelectedByDriverEventReader*>(r));
    }
    else if (dynamic_cast<pecRequestForResetEventReader*>(r) != 0)
    {
        onPECResetRequest(event, dynamic_cast<pecRequestForResetEventReader*>(r));
    }
    else if (dynamic_cast<pecResetEventReader*>(r) != 0)
    {
        onPECReset(event, dynamic_cast<pecResetEventReader*>(r));
    }
    else if (dynamic_cast<pecReadyForConversationEventReader*>(r) != 0)
    {
        onPECReadyForConversation(event, dynamic_cast<pecReadyForConversationEventReader*>(r));
    }
    else 
    {
        consumed = false;
    }
    return consumed;
}

bool TrainPEC::delegate(unsigned long callId, std::vector<unsigned char>& trains)
{
    // A PEC call has been disconnected

    // trains can only contain a single entry
    TA_ASSERT(trains.size() == 1, "Multiple trains associated with a PEC call");

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
        "Audio lost for PEC call on train %d, notifying operator", trains[0] );

    // Just invalidate the PEC, and send a message out
    // do not send anything to the train
    unsigned char trackX = 
        static_cast<unsigned char>(m_trainManager.getTrainStatusManager().getTrainDirection(trains[0]));
    PECPoint point = m_trainManager.getTrainStatusManager().getConnectedPECPoint(trains[0]);
    std::string sessionId =
        m_trainManager.getTrainStatusManager().getOwningSessionID(trains[0], TrainStatus::Resource_PEC);
    
    m_notifier.notifyPEC(PEC_CallLost, trains[0], point, trackX, sessionId);

    return true;
}

TrainStatus::FullPECPointCollection TrainPEC::getPECCalls()
{
    return m_trainManager.getTrainStatusManager().getActivePECPoints();
}

void TrainPEC::answerPECCall(unsigned char trainID,
                             const struct PECPoint& my_point,
                             std::string sessionID)
{
	// verify noone has started this call already
    std::string owningSession = m_trainManager.getTrainStatusManager().getOwningSessionID(trainID, TrainStatus::Resource_PEC);
    if (owningSession != "")
    {
        // then a PEC is already in progress for this train
        TA_THROW( TrainBusyException(false) );
    }

	TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncPassengerEmergencyCall);

    // attempt to own the session
    m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_PEC, sessionID);   

    struct PECPoint point = my_point;
    // find track (for later)
	unsigned char trackX = static_cast<unsigned char>(m_trainManager.getTrainStatusManager().getTrainDirection(trainID));

    unsigned int numberOfRetries = 0;
    bool success = false;
    
    do
    {
	    // since we now own the resource, lets reset the counter (doesn't matter if we proceed or fail the tims request)
        m_trainManager.getTrainStatusManager().resetContinueCounter(trainID, TrainStatus::Resource_PEC);

        int callId = 0;
    
        try 
        {
            // Send command to train
            pecAnswerCommand command; 
            command.setTrainID(trainID);
            command.setPECNumber(point.car, point.PEC);

            // track the start time
            ACE_Time_Value startTime = ACE_OS::gettimeofday();

            // handle the timeout and retry limit in here, its a special funny case
            // where we wait for the response, but the timer ends on an unsolicited
            // event we get later (after audio)
            TrainEventPtr event = m_trainManager.sendTrainCommandWithWaitNoRetries(trainID, command, m_timerPEC2);

            ACE_Time_Value endTime = ACE_OS::gettimeofday();

            unsigned int elapsedTimeMillis = (startTime.msec() - endTime.msec());

            // find the ET for this command, and the time left for the next message to be received
            unsigned int remainingPEC2time = m_timerPEC2 - elapsedTimeMillis;

            // Check event reply
            pecCommandReceivedEventReader* reader = dynamic_cast<pecCommandReceivedEventReader*>(event->getReader());
		    // debug - checking for out-of-order error
		    if (0 != dynamic_cast<pecReadyForConversationEventReader*>(event->getReader()) )
		    {
			    m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_PEC, "", true);    
			    TA_THROW( TrainGeneralFailureException() );
            }
            if (0 == reader)
            {
			    m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_PEC, "", true);    
                TA_THROW( TrainGeneralFailureException() );
            }

            switch (reader->getCommandReceivedStatus())
            {
            case pecCommandReceivedEventReader::PEC_PROCEEDING:
                break;
            case pecCommandReceivedEventReader::PEC_IN_USE:
			    m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_PEC, "", true);    
			    TA_THROW( TrainBusyException(false) );
                break;
            case pecCommandReceivedEventReader::PEC_FAILED:
			    m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_PEC, "", true);    
                TA_THROW( TrainGeneralFailureException("", TrainGeneralFailureException::COMMAND_FAILED) );
                break;
            default:
			    m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_PEC, "", true);    
                TA_THROW( TrainGeneralFailureException() );
            }
        
            /// Initiate Audio setup
            callId = m_notifier.callRadioMakeCall(trainID, ITrainNotificationFacade::RADIO_PEC, sessionID);
		    if (callId == 0)
		    {
	            //m_trainManager.getTrainTimer().endTimer(trainID);
			    m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_PEC, "", true);    
			    TA_THROW( TrainGeneralFailureException() );
		    }

		    // set the callId in the train status
		    m_trainManager.getTrainStatusManager().setCallAllocation(trainID, TrainStatus::Resource_PEC, callId);

            // wait until timeout or onPECReadyForConversation() tells us it is ok (or not) to continue

            // start the timer PEC2 - ET for the first command
	        m_trainManager.getTrainTimer().startTimer(trainID, remainingPEC2time, "PEC2Timer");

            m_trainManager.getTrainTimer().waitTimer(trainID);

            m_trainManager.getTrainTimer().endTimer(trainID);

            // answer succeeded
            success = true;
        }
        catch (TrainTimerTimeoutException & )
        {
            // This means the first command succeeded, but the ready for convo
            // didnt come in before the train timer expired

            m_trainManager.getTrainTimer().endTimer(trainID);

            // clean up the call
            if (callId != 0)
            {
                // Close down audio
		        m_notifier.callRadioReleaseCall(callId, sessionID);
            }
            m_trainManager.getTrainStatusManager().setCallAllocation(trainID, TrainStatus::Resource_PEC, 0);

            // check the number of retries
		    if (numberOfRetries < m_retryLimit)
            {
                // increment retries
                ++numberOfRetries;
            }
            else // exceeded retry count
            {
                // throw exception on
                m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_PEC, "", true);
                TA_THROW( TrainTimeoutException() );
            }
        }
        catch (TrainTimeoutException & e)
        {
            // This will be thrown from the original command,
            // not the TrainTimer

            // check the number of retries
		    if (numberOfRetries < m_retryLimit)
            {
                // increment retries
                ++numberOfRetries;
            }
            else // exceeded retry count
            {
                // throw exception on
                m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_PEC, "", true);
                TA_RETHROW( e );
            }
        }
        catch (TrainException & e)
        {
            m_trainManager.getTrainTimer().endTimer(trainID);
		    m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_PEC, "", true);    
            TA_RETHROW( e );
        }

    // the retry condition here is redundant, once the max retries are reached
    // an exception will be thrown before here anyway
    } while (!success && (numberOfRetries <= m_retryLimit));


    // The answer succeeded if we get here


	// update state
	point.source = TrainTypes::PEC_OPERATOR;
	PECPoint updatedPoint;
	updatedPoint = m_trainManager.getTrainStatusManager().setActivePECPoint(trainID, point);

	// Successfully negotiated with TIMS and established audio line, so tell everyone else.
    m_notifier.notifyPEC(PEC_Operator, trainID, updatedPoint, trackX, sessionID);

    // notify listeners of points to same train
    PECPointCollection otherPEC = m_trainManager.getTrainStatusManager().getActivePECPoints(trainID);
	removePECFromCollection( otherPEC, updatedPoint );
    m_notifier.notifyPEC(PEC_Operator, trainID, otherPEC, trackX, sessionID);
}


void TrainPEC::endPECCall(unsigned char trainID, std::string sessionID)
{
    // verify the session
    //m_trainManager.getTrainStatusManager().verifyOwningSessionID(trainID, TrainStatus::Resource_PEC, sessionID);

	TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncPassengerEmergencyCall);

	try
	{
		// get the PEC callId
		unsigned long callId = 
        m_trainManager.getTrainStatusManager().getCallAllocation(trainID, TrainStatus::Resource_PEC);

		// Close down audio
		m_notifier.callRadioReleaseCall(callId, sessionID);

		// reset the callId in the train status
		m_trainManager.getTrainStatusManager().setCallAllocation(trainID, TrainStatus::Resource_PEC, 0);

		// Put connected pec back into list 
		struct PECPoint point = 
			m_trainManager.getTrainStatusManager().getConnectedPECPoint(trainID);

		m_trainManager.getTrainStatusManager().removeActivePECPoint(trainID, point);
	}
    catch (TrainException & e) 
    {
		m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_PEC, "", true);    
        TA_RETHROW( e ); 
    }

	// release session so other operators can aquire it
	m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_PEC, "", true);    

    // notify listeners of points to same train
    PECPointCollection allPEC = m_trainManager.getTrainStatusManager().getActivePECPoints(trainID);
	// find track 
	unsigned char trackX = static_cast<unsigned char>(m_trainManager.getTrainStatusManager().getTrainDirection(trainID));
	m_notifier.notifyPEC(PEC_Activated, trainID, allPEC, trackX, sessionID);
}


void TrainPEC::resetPEC(unsigned char trainID,
                        const struct PECPoint & my_point,
                        bool sendReset,
                        std::string sessionID)
{
    // verify the session

    //m_trainManager.getTrainStatusManager().verifyOwningSessionID(trainID, TrainStatus::Resource_PEC, sessionID);

    // stop the PEC 3 timer
    stopPEC3Timer(trainID);
    
	TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncPassengerEmergencyCall);

    struct PECPoint point = my_point;

    // ss++
    // TD14378
    std::string errorMsg;
    bool hasTimeout = false;
    // ++ss

    // only send the command if true.
    // otherwise dont send the command and forcefully remove the PEC
    if (sendReset)
    {
        // Send command to train
        pecResetCommand command; 
	    command.setTrainID(trainID);    
        command.setPECNumber(point.car, point.PEC); 
    
        try
        {
		    // get the PEC callId
		    unsigned long callId = 
            m_trainManager.getTrainStatusManager().getCallAllocation(trainID, TrainStatus::Resource_PEC);

		    // Close down audio
		    m_notifier.callRadioReleaseCall(callId, sessionID);

		    // reset the callId in the train status
		    m_trainManager.getTrainStatusManager().setCallAllocation(trainID, TrainStatus::Resource_PEC, 0);

            // ss++
            // TD14378
            try
            {
                // Use ISCS-PEC-4-Timer for this message
                TrainEventPtr event = m_trainManager.sendTrainCommandWithWait(trainID, command, m_timerPEC4);
        
                // Check event reply
                pecResetEventReader* reader = dynamic_cast<pecResetEventReader*>(event->getReader());
                if (0 == reader)
                {
                    TA_THROW( TrainGeneralFailureException() );
                }
            }
            catch(TrainTimeoutException& e)
            {
                hasTimeout = true;
                errorMsg = e.what();
            }
            // ++ss
        
        }
        catch (TrainException & e) 
        {
		    m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_PEC, "", true);    
            TA_RETHROW( e ); 
        }
    }
    
    m_trainManager.getTrainStatusManager().deleteActivePECPoint(trainID, point);
    m_trainManager.getTrainStatusManager().resetContinueCounter(trainID, TrainStatus::Resource_PEC);
    // no need to notify OCC, as method return sufficient to indicate success

    /// @todo Notify next PEC handler
	// find track
	unsigned char trackX = static_cast<unsigned char>(m_trainManager.getTrainStatusManager().getTrainDirection(trainID));

	m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_PEC, "", true);    

	// notify any interested subscribers, that the call has ended
    m_notifier.notifyPEC(PEC_Reset, trainID, point, trackX, sessionID);

    // notify listeners of points to same train
    PECPointCollection otherPEC = m_trainManager.getTrainStatusManager().getActivePECPoints(trainID);
	removePECFromCollection( otherPEC, point );
    m_notifier.notifyPEC(PEC_Activated, trainID, otherPEC, trackX, sessionID);

    // ss++
    // TD14378
    if(hasTimeout)
    {
        TA_THROW(TrainTimeoutException (errorMsg));
    }
    // ++ss

}


void TrainPEC::resetAllPEC(unsigned char trainID, bool sendReset, std::string sessionID)
{
    // verify the session

    //m_trainManager.getTrainStatusManager().verifyOwningSessionID(trainID, TrainStatus::Resource_PEC, sessionID);

    // stop the PEC 3 timer
    stopPEC3Timer(trainID);

	TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncPassengerEmergencyCall);

    // ss++
    // TD14378
    std::string errorMsg;
    bool hasTimeout = false;
    // ++ss

    // only send the command if true.
    // otherwise dont send the command and forcefully remove the PEC
    if (sendReset)
    {
        // Send command to train
        pecResetCommand command; 
	    command.setTrainID(trainID);    
    
        struct PECPoint point = TrainStatusManager::emptyPoint;
        command.setPECNumber(point.car, point.PEC); 
    
        try
        {
            // get the PEC callId
		    unsigned long callId = 
            m_trainManager.getTrainStatusManager().getCallAllocation(trainID, TrainStatus::Resource_PEC);

		    // Close down audio
		    m_notifier.callRadioReleaseCall(callId, sessionID);

		    // reset the callId in the train status
		    m_trainManager.getTrainStatusManager().setCallAllocation(trainID, TrainStatus::Resource_PEC, 0);
		    
            // ss++
            // TD14378
            try
            {
		        // Use ISCS-PEC-4-Timer for this message
                TrainEventPtr event = m_trainManager.sendTrainCommandWithWait(trainID, command, m_timerPEC4);

                // Check event reply
                pecResetEventReader* reader = dynamic_cast<pecResetEventReader*>(event->getReader());
                if (0 == reader)
                {
                    TA_THROW( TrainGeneralFailureException() );
                }
            }
            catch(TrainTimeoutException& e)
            {
                hasTimeout = true;
                errorMsg = e.what();
            }
            // ++ss

        }
        catch (TrainException & e) 
        {
		    m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_PEC, "", true);    
            TA_RETHROW( e ); 
        }
    }

	// find track
	unsigned char trackX = static_cast<unsigned char>(m_trainManager.getTrainStatusManager().getTrainDirection(trainID));

	// notify any interested subscribers, that all pecs have been reset
	//    m_notifier.notifyPEC(PEC_Reset, trainID, point, trackX, sessionID);
    PECPointCollection activePoints = m_trainManager.getTrainStatusManager().getActivePECPoints(trainID);
	m_notifier.notifyPEC(PEC_Reset, trainID, activePoints, trackX, sessionID);

    // Delete all PEC's for this trainID - do it the hard way...
    for (PECPointCollection::iterator i = activePoints.begin(); i != activePoints.end(); ++i) 
    {
        m_trainManager.getTrainStatusManager().deleteActivePECPoint(trainID, *i);
        m_trainManager.getTrainStatusManager().resetContinueCounter(trainID, TrainStatus::Resource_PEC);
    }


	m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_PEC, "", true);   
    
    // ss++
    // TD14378
    if(hasTimeout)
    {
        TA_THROW(TrainTimeoutException (errorMsg));
    }
    // ++ss


}


void TrainPEC::continuePECCall(unsigned char trainID, std::string sessionID)
{
    // verify the session
    //m_trainManager.getTrainStatusManager().verifyOwningSessionID(trainID, TrainStatus::Resource_PEC, sessionID);

	TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncPassengerEmergencyCall);

	try
	{
		// Send command to train
		pecContinueCommand command; 
		command.setTrainID(trainID);    
		
		PECPoint point = m_trainManager.getTrainStatusManager().getConnectedPECPoint(trainID);
		
		command.setPECNumber(point.car, point.PEC); /// @todo check this
		
		// don't wait for reply
		m_trainManager.sendTrainCommandNoReply(trainID, command);
		
		// Update continueCount to zero
		m_trainManager.getTrainStatusManager().resetContinueCounter(trainID, TrainStatus::Resource_PEC);

        // start the PEC 3 timer
        startPEC3Timer(trainID);
	}
    catch (TrainException & e) 
    {
		m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_PEC, "", true);    
        TA_RETHROW( e ); 
    }
}

void TrainPEC::onPECActivation(TrainEvent &event, pecActivatedEventReader * reader)
{
    unsigned int trainID = event.getTrainID();

	TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncPassengerEmergencyCall);

	// start ISCS-PEC-2-Timer
// wrong timer...		m_trainManager.getTrainTimer().startTimer(trainID, m_timerPEC2, "PEC2Timer");

    PECPoint point;
    point.car = reader->getCarID();
    point.PEC = reader->getPECID();
    point.source = TrainTypes::PEC_UNASSIGNED;
	point.creationTime = time(NULL);
    point.validity = true;

	// we update the creation time, if this is a duplicate message sent by TIMS
	PECPoint updatedPoint;
    updatedPoint = m_trainManager.getTrainStatusManager().setActivePECPoint(trainID, point);   

	// find track
	unsigned char trackX = static_cast<unsigned char>(m_trainManager.getTrainStatusManager().getTrainDirection(trainID));
	
	// Notify OCC (PECActivation/PECDriver/PECOperator)
	// If no other pecs in use, sent Activated
	// If another pec is active (check currently connected point) then send Driver/Operator as appropriate
	// Easiest way to check is to look at the source field of the connected point
	// if Unassigned, then either empty, or not yet in use
    PECPoint activePEC = m_trainManager.getTrainStatusManager().getConnectedPECPoint(trainID);
	switch (activePEC.source) 
	{
	case TrainTypes::PEC_OPERATOR:
		m_notifier.notifyPEC(PEC_Operator, trainID, updatedPoint, trackX, m_trainManager.getTrainStatusManager().getOwningSessionID(trainID, TrainStatus::Resource_PEC));
		break;
	case TrainTypes::PEC_DRIVER:
		m_notifier.notifyPEC(PEC_Driver, trainID, updatedPoint, trackX, "");
		break;
	case TrainTypes::PEC_UNASSIGNED:
	default:
	    m_notifier.notifyPEC(PEC_Activated, trainID, updatedPoint, trackX, "");
		break;
	}

    // ss++
    // TD #13763
    // Log an event for a new Activation PEC
    // Check to ensure that we only submit an audit message for a new activation request
    if(point.creationTime == updatedPoint.creationTime)
    {
        // audit the operation
        TA_Base_Core::DescriptionParameters parameters;
			    
        std::stringstream pecStr;
		pecStr << getPecStringForPecId((int)(updatedPoint.PEC)) << std::ends;
		TA_Base_Core::NameValuePair pec("PEC", pecStr.str().c_str());
		parameters.push_back(&pec);
		
        std::stringstream carStr;
		carStr << static_cast<int>(updatedPoint.car) << std::ends;
		TA_Base_Core::NameValuePair car("CAR", carStr.str().c_str());
		parameters.push_back(&car);

        std::stringstream servStr;
		//TD17298 liqiang++
		std::ios_base::fmtflags flags = servStr.setf(std::ios::right);
        int width = servStr.width(2);
        char padding = servStr.fill('0');
		//TD17298 liqiang++

        servStr << static_cast<int>(trainID);

		//TD17298 liqiang++
		servStr.setf(flags);
        servStr.width(width);
        servStr.fill(padding);
		//TD17298 ++liqiang
		
        servStr << "/";
        servStr << m_trainManager.getTrainStatusManager().getTrainServiceNumber(trainID) << std::ends;
        TA_Base_Core::NameValuePair nvServStr("Train", servStr.str().c_str());//TD17298 liqiang
		parameters.push_back(&nvServStr);

        std::stringstream location;
        try
        {
//            TA_Base_Core::ILocation* iLoc = 
//                TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(m_trainManager.getTrainStatusManager().getTrainLocation(trainID));
			//TD17298 liqiang++
			unsigned long locationkey=atoi(TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str());
			TA_Base_Core::ILocation* iLoc = 
                TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(locationkey);
            location << iLoc->getName();			
            if(iLoc)
                delete iLoc;
			
			location<<" / ";
			switch( m_trainManager.getTrainStatusManager().getTrainDirection(trainID) )
			{
				case TrainTypes::Track_Inbound:
					location << "IT";
					break;
				case TrainTypes::Track_Outbound:
					location << "OT";
					break;
				case TrainTypes::Track_Unknown:
					location << "Unknown";
					break;
			}
		//TD17298 ++liqiang
        }
        catch(...)
        {
            location << "Unknown" << std::ends;
        }
        TA_Base_Core::NameValuePair nvLoc("Location", location.str().c_str());//TD17298 liqiang
	    parameters.push_back(&nvLoc);

		/*m_notifier.AuditOperation(TA_Base_Core::TrainAudit::TrainPECActivated, parameters, "");*/
		m_notifier.AuditOperation(TA_Base_Core::TrainAudit::TrainPECActivated, parameters, TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID));//TD17298 liqiang
    }
    // ++ss

    
	if(false == m_operationModeManager.isInControlOfTrain(trainID))
	{
		invalidatePec(trainID);
	}

}


void TrainPEC::onPECDriverSelection(TrainEvent &event, pecSelectedByDriverEventReader * reader)
{
    unsigned int trainID = event.getTrainID();

	TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncPassengerEmergencyCall);

    // stop the PEC 3 timer
    stopPEC3Timer(trainID);

    PECPoint point;
    point.car = reader->getCarID();
    point.PEC = reader->getPECID();
    point.source = TrainTypes::PEC_DRIVER;
    point.validity = true;

	PECPoint updatedPoint;
    updatedPoint = m_trainManager.getTrainStatusManager().setActivePECPoint(trainID, point);   
	/// @note signal just in case operator is waiting (although it will timeout eventually anyway)
	m_trainManager.getTrainTimer().signalTimer(trainID);
// testing	m_trainManager.getTrainTimer().waitTimer(trainID);

	// find track
	unsigned char trackX = static_cast<unsigned char>(m_trainManager.getTrainStatusManager().getTrainDirection(trainID));

    // Notify OCC (PECDriver)
    m_notifier.notifyPEC(PEC_Driver, trainID, updatedPoint, trackX, "");

    // notify listeners of points to same train
    PECPointCollection otherPEC = m_trainManager.getTrainStatusManager().getActivePECPoints(trainID);
	removePECFromCollection( otherPEC, updatedPoint );
    m_notifier.notifyPEC(PEC_Driver, trainID, otherPEC, trackX, "");

}


void TrainPEC::onPECResetRequest(TrainEvent &event, pecRequestForResetEventReader * reader)
{
    unsigned int trainID = event.getTrainID();

	TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncPassengerEmergencyCall);

    // stop the PEC 3 timer
    stopPEC3Timer(trainID);

    PECPoint notifiedPoint = TrainStatusManager::emptyPoint;
    notifiedPoint.car = reader->getCarID();
    notifiedPoint.PEC = reader->getPECID();
    notifiedPoint.source = TrainTypes::PEC_UNASSIGNED; // for completeness
    notifiedPoint.validity = true;

    struct PECPoint connectedPoint = 
        m_trainManager.getTrainStatusManager().getConnectedPECPoint(trainID);

    // Check the points - they should match !!!
    if (!TrainStatusManager::comparePECPoint()(notifiedPoint, connectedPoint))
    {
        // they don't match ! - ignore the request
        return;
    }

    // Update TrainStatus - increment retries
    m_trainManager.getTrainStatusManager().incrementContinueCounter(trainID, TrainStatus::Resource_PEC);

    // If retries less than 'limit' autosend the continue
    if (!m_trainManager.getTrainStatusManager().exceededContinueLimit(trainID, TrainStatus::Resource_PEC))
    {
        // Send an automatic reply
        pecContinueCommand command; 
		command.setTrainID(trainID);    
        command.setPECNumber(connectedPoint.car, connectedPoint.PEC); /// @todo check this    
        try
        {
            // no reply expected
			m_trainManager.sendTrainCommandNoReply(trainID, command);

            // start the PEC 3 timer
            startPEC3Timer(trainID);
        }
        catch (TrainException &) 
        {
            // don't care about reply
        }
    }
    else 
    {
        m_trainManager.getTrainStatusManager().resetContinueCounter(trainID, TrainStatus::Resource_PEC);

		// find track
		unsigned char trackX = static_cast<unsigned char>(m_trainManager.getTrainStatusManager().getTrainDirection(trainID));

        // Notify OCC (PECContinue)
        std::string sessionID = m_trainManager.getTrainStatusManager().getOwningSessionID(trainID, TrainStatus::Resource_PEC);
        m_notifier.notifyPEC(PEC_Continue, trainID, connectedPoint, trackX, sessionID);
    }
}


void TrainPEC::onPECReset(TrainEvent &event, pecResetEventReader * reader)
{
    unsigned int trainID = event.getTrainID();

   	TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncPassengerEmergencyCall);

    // stop the PEC 3 timer
    stopPEC3Timer(trainID);

	PECPoint notifiedPoint = TrainStatusManager::emptyPoint;
    notifiedPoint.car = reader->getCarID();
    notifiedPoint.PEC = reader->getPECID();
    notifiedPoint.source = TrainTypes::PEC_UNASSIGNED; // for completeness
    notifiedPoint.validity = true;

	m_trainManager.getTrainStatusManager().setOwningSessionID(trainID, TrainStatus::Resource_PEC, "", true);

    m_trainManager.getTrainStatusManager().deleteActivePECPoint(trainID, notifiedPoint);
    m_trainManager.getTrainStatusManager().resetContinueCounter(trainID, TrainStatus::Resource_PEC);

	// need to ack a reset all
    if (0 == notifiedPoint.PEC)
    {
        // Must have been a reset all event, so send acknowledgement to TIMS
        pecResetCommand command; 
		command.setTrainID(trainID);
		/// @note hack to prevent simulator spinning for driver initiated PEC reset all
		//command.setOrigin(20); /// @todo remove override (20 is invalid occ origin)
        command.setPECNumber(notifiedPoint.car, notifiedPoint.PEC); 
        try
        {
            m_trainManager.sendTrainCommandNoReply(trainID, command);
            // no reply expected
        }
        catch (TrainException &) 
        {
            // don't care about reply
        }
    }

 	// find track
	unsigned char trackX = static_cast<unsigned char>(m_trainManager.getTrainStatusManager().getTrainDirection(trainID));

    // Notify OCC (PECReset)
    if (0 != notifiedPoint.PEC) // single point reset
    {
	    m_notifier.notifyPEC(PEC_Reset, trainID, notifiedPoint, trackX, "");

		// notify listeners of points to same train, they are available again
		PECPointCollection otherPEC = m_trainManager.getTrainStatusManager().getActivePECPoints(trainID);
		removePECFromCollection( otherPEC, notifiedPoint );
		m_notifier.notifyPEC(PEC_Activated, trainID, otherPEC, trackX, "");
	}
	else // all points reset
    {
	    PECPointCollection activePoints = m_trainManager.getTrainStatusManager().getActivePECPoints(trainID);
		m_notifier.notifyPEC(PEC_Reset, trainID, activePoints, trackX, "");

	    // Cleanup - delete all remaining PEC's for this trainID - do it the hard way...
		for (PECPointCollection::iterator i = activePoints.begin(); i != activePoints.end(); ++i) 
		{
			m_trainManager.getTrainStatusManager().deleteActivePECPoint(trainID, *i);
			m_trainManager.getTrainStatusManager().resetContinueCounter(trainID, TrainStatus::Resource_PEC);
		}

	}

    // ss++
    // TD #13763
    // Log an event for a new Activation PEC
    // audit the operation
    TA_Base_Core::DescriptionParameters parameters;
			
    std::stringstream pecStr;
	pecStr << getPecStringForPecId((int)(notifiedPoint.PEC)) << std::ends;
	TA_Base_Core::NameValuePair pec("PEC", pecStr.str().c_str());
	parameters.push_back(&pec);
	
    std::stringstream carStr;
	carStr << static_cast<int>(notifiedPoint.car) << std::ends;
	TA_Base_Core::NameValuePair car("CAR", carStr.str().c_str());
	parameters.push_back(&car);

    std::stringstream servStr;

	//TD17298 liqiang++
	std::ios_base::fmtflags flags = servStr.setf(std::ios::right);
    int width = servStr.width(2);
    char padding = servStr.fill('0');
	//TD17298 liqiang++  
		
     servStr << static_cast<int>(trainID);

	//TD17298 liqiang++
	servStr.setf(flags);
    servStr.width(width);
    servStr.fill(padding);
	//TD17298 ++liqiang 
    
    servStr << "/";
    servStr << m_trainManager.getTrainStatusManager().getTrainServiceNumber(trainID) << std::ends;
    TA_Base_Core::NameValuePair nvServStr("Train", servStr.str().c_str());//TD17298 liqiang
	parameters.push_back(&nvServStr);

	//TD17298 liqiang
//    std::stringstream location;
//    try
//    {
//        TA_Base_Core::ILocation* iLoc = 
//            TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(m_trainManager.getTrainStatusManager().getTrainLocation(trainID));
//        location << iLoc->getName();
//        if(iLoc)
//            delete iLoc;
//    }
//    catch(...)
//    {
//        location << "Unknown" << std::ends;
//    }
//    TA_Base_Core::NameValuePair nvLoc("Location", location.str().c_str());//liqinag
	//TD17298 liqiang++
	std::stringstream locationStr;
	unsigned long locationkey=atoi(TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str());
	TA_Base_Core::ILocation* iLoc = 
         TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(locationkey);
    locationStr << iLoc->getName();		
         if(iLoc)
           delete iLoc;
	
	locationStr <<" / ";
	switch( m_trainManager.getTrainStatusManager().getTrainDirection(trainID))
	{
		case TrainTypes::Track_Inbound:
			locationStr << "IT";
			break;
		case TrainTypes::Track_Outbound:
			locationStr << "OT";
			break;
		case TrainTypes::Track_Unknown:
			locationStr << "Unknown";
			break;
	}
	//TD17298++liqiang

	TA_Base_Core::NameValuePair nvlocn("Location", locationStr.str().c_str());//TD17298 liqiang
	parameters.push_back(&nvlocn);
	
	m_notifier.AuditOperation(TA_Base_Core::TrainAudit::TrainPECResetByTIMS, parameters, "");
    // ++ss
}


void TrainPEC::onPECReadyForConversation(TrainEvent &event, pecReadyForConversationEventReader * reader)
{
    // update semaphore waited on by answerCall() to indicate ok to proceed
    unsigned int trainID = event.getTrainID();

	TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncPassengerEmergencyCall);
    
	m_trainManager.getTrainTimer().signalTimer(trainID);

    // start the PEC 3 timer
    startPEC3Timer(trainID);
}


void TrainPEC::handleTimeout(unsigned char trainId)
{
    onPEC3Timeout(trainId);
}


void TrainPEC::startPEC3Timer(unsigned char trainId)
{
    //m_pec3TimerManager.startTimer(trainId, m_timerPEC3, true, "PEC3Timer");
}

void TrainPEC::stopPEC3Timer(unsigned char trainId)
{
 //   m_pec3TimerManager.endTimer(trainId);
}

void TrainPEC::onPEC3Timeout(unsigned char trainId)
{
    // An expected TIMS reset request was not received

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
        "TIMS communication lost for PEC call on train %d, notifying operator", trainId );

    // Just invalidate the PEC, and send a message out
    // do not send anything to the train
    unsigned char trackX = 
        static_cast<unsigned char>(m_trainManager.getTrainStatusManager().getTrainDirection(trainId));
    PECPoint point = m_trainManager.getTrainStatusManager().getConnectedPECPoint(trainId);

    std::string sessionId =
        m_trainManager.getTrainStatusManager().getOwningSessionID(trainId, TrainStatus::Resource_PEC);

    m_notifier.notifyPEC(PEC_ContinueTimeout, trainId, point, trackX, sessionId);
}


void TrainPEC::invalidatePec(unsigned char trainId)
{
	//TD14205
	//zhou yuan ++
	try
	{
		// get all PEC points for this train
		PECPointCollection points = 
			m_trainManager.getTrainStatusManager().getActivePECPoints(trainId);

		if (points.size() == 0)
		{
			return;
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"Invalidating PEC calls on train %d", trainId );

		unsigned char trackX = static_cast<unsigned char>(m_trainManager.getTrainStatusManager().getTrainDirection(trainId));

		TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainId, syncPassengerEmergencyCall);

		// invalidate each one that is not in progress
		for (PECPointCollection::iterator iter = points.begin();
		iter != points.end(); ++iter)
		{
			// if it is in progress
			if (iter->source == TrainTypes::PEC_OPERATOR)
			{
				// leave this one alone
				continue;
			}

			// otherwise
			// set its validity false
			iter->validity = false;

			// update it in train status manager
			PECPoint updatedPoint;
			updatedPoint = m_trainManager.getTrainStatusManager().setActivePECPoint(trainId, *iter);

			m_notifier.notifyPEC(PEC_Invalidated, trainId, updatedPoint, trackX, "");

			// audit the operation
			TA_Base_Core::DescriptionParameters parameters;
					
			std::stringstream pecStr;
			pecStr << getPecStringForPecId((int)(updatedPoint.PEC)) << std::ends;
			TA_Base_Core::NameValuePair pec("PEC", pecStr.str().c_str());
			parameters.push_back(&pec);
			
			std::stringstream carStr;
			carStr << static_cast<int>(updatedPoint.car) << std::ends;
			TA_Base_Core::NameValuePair car("CAR", carStr.str().c_str());
			parameters.push_back(&car);

			std::stringstream pvStr;
			pvStr << static_cast<int>(trainId) << std::ends;
			TA_Base_Core::NameValuePair pv("PV", pvStr.str().c_str());
			parameters.push_back(&pv);

			std::stringstream serviceStr;
			serviceStr << m_trainManager.getTrainStatusManager().getTrainServiceNumber(trainId) << std::ends;
			TA_Base_Core::NameValuePair service("SERVICE", serviceStr.str().c_str());
			parameters.push_back(&service);
			
			m_notifier.AuditOperation(TA_Base_Core::TrainAudit::TrainPECCallInvalid,parameters,"");
		}
	}
	catch (TrainInvalidTrainException&)
	{
		// nothing to invalidate
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  
			TA_Base_Core::DebugUtil::DebugInfo, "train %d is not in the map of m_trainInfoMap",
			trainId);
	}
	//zhou yuan++
}

//TD 15938
//zhou yuan++
bool TrainPEC::isOperatorBusy(const std::string& sessionID)
{
	// verify the operator has started any call before
	return m_trainManager.getTrainStatusManager().isOperatorBusy(sessionID, TrainStatus::Resource_PEC);		
}
//++zhou yuan

std::string TrainPEC::getPecStringForPecId(int pecNumber)
{
	std::string strPEC;
	switch(pecNumber)
	{
	case 1:
		strPEC = "1A";
		break;
	case 2:
		strPEC = "1B";
		break;
	case 3:
		strPEC = "2A";
		break;
	case 4:
		strPEC = "2B";
		break;
	default:
		strPEC = "";
	}
	return strPEC;
}
