/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/trains/train_agent/src/TrainCCTV.cpp $
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
//  TrainCCTV.cpp
//  Implementation of the Class TrainCCTV
//  Created on:      26-Nov-2003 02:15:47 PM
///////////////////////////////////////////////////////////

#include "TrainCCTV.h"
#include "ITrainManager.h"
#include "app/trains/train_agent/src/TARethrow.h"
#include "app/trains/train_agent/src/TrainStatusManager.h"
#include "app/trains/train_agent/src/OperationModeManager.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/TrainEvent.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/cctvCarrierOnCommand.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/cctvCarrierOffCommand.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/cctvCommandReceivedEventReader.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/cctvQuadScreenCommand.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/cctvSequenceCommand.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/cctvSingleScreenCommand.h"
#include "bus/trains/TrainAgentCorbaDef/idl/src/ICCTVTrainCommandsCorbaDef.h"
#include "app/trains/train_agent/src/TrainException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/message/types/TrainAudit_MessageTypes.h"
#include <sstream>

namespace {
    using namespace TA_IRS_App;

    // TODO: Confirm exceptions
    void checkBusy(cctvCommandReceivedEventReader* reader)    
    {
        switch (reader->getStatus())
        {
        case cctvCommandReceivedEventReader::CCTV_IN_PROCEEDING:
            break;		
        case cctvCommandReceivedEventReader::CCTV_IN_USE:
            TA_THROW(TrainBusyException("CCTV in use", false));
            break;
        case cctvCommandReceivedEventReader::CCTV_FAILED:
            TA_THROW(TrainGeneralFailureException("", TrainGeneralFailureException::COMMAND_FAILED));
            break;
        }
    }
}

using namespace TA_IRS_App;


TrainCCTV::TrainCCTV(ITrainManager& callback, ITrainNotificationFacade& notifier, OperationModeManager& operationModeManager) 
: m_trainManager(callback),
m_notifier(notifier),
m_operationModeManager(operationModeManager)
{
	m_timerCCTV1 = m_trainManager.getGlobalTrainAgentEntity()->getTimerCCTV1();
    m_trainManager.addDelegate(this);
}



TrainCCTV::~TrainCCTV()
{

}

bool TrainCCTV::delegate(TrainEvent& event)
{
    return false;  // no Events to handle
}

void TrainCCTV::aboutToActivate(unsigned int trainID)
{
    // checks conflicts and sets or clears pre conflict
	CarrierConflict conflict = checkConflicts(trainID, true, false);
	if(conflict.type != imminentConflict)
	{
		if(conflict.type == preConflict)
		{
			// we cant stop an operator from looking at a pre conflict train
			// as this is usually an entire station, a train can be here for 30sec - 1minute,
			// so we will allow it (train could be broken down )
			// but we will raise an alarm warning the operator of the pre conflict state
			m_notifier.raiseCctvPreConflictAlarm(conflict.offendingTrain, conflict.owningTrain);
			m_trainManager.getTrainStatusManager().setPreConflict(conflict.offendingTrain, conflict.owningTrain);
		}
		else
		{
			m_trainManager.getTrainStatusManager().setPreConflict(conflict.offendingTrain, 0);
			m_notifier.closeCctvPreConflictAlarm(conflict.offendingTrain);
		}
	}
	else
	{
		std::ostringstream reason;
		int owningTrain = conflict.owningTrain;
		double trainZone = m_trainManager.getTrainStatusManager().getTrainCctvZone(trainID);
	
		reason << "Cannot activate Train " << trainID << " as Train " << owningTrain << ", is already broadcasting in stage " << getStageId(trainZone) <<std::ends;
		
		LOG(SourceInfo,	TA_Base_Core::DebugUtil::ExceptionConstruction, "TrainCCTVConflictException", reason.str().c_str());
		
		throw TA_Base_Bus::ICCTVTrainCommandsCorbaDef::TrainCCTVConflictException(reason.str().c_str(), conflict.owningTrain);
	}
}

void TrainCCTV::cctvCarrierOn(unsigned int trainID,std::string sessionID)
{
	TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncCctv);
    
    try
    {
        // checks conflicts and sets/clears pre conflict alarms
        // throws an exception if in conflict
        aboutToActivate(trainID);
		
        // getting here means no conflict
        // pre conflict is possible but activation is OK
        cctvCarrierOnCommand command; 
        command.setTrainID(trainID);
		TrainEventPtr event = m_trainManager.sendTrainCommandWithWait(trainID, command);
    
		// Check event reply
		cctvCommandReceivedEventReader* reader = dynamic_cast<cctvCommandReceivedEventReader*>(event->getReader());
		if (0 == reader)
		{
			TA_THROW( TrainGeneralFailureException() );
		}
    
		// busy?
        try
        {
    		checkBusy(reader);
        }
        catch(TrainBusyException&)
        {
            // assume it passed if we get a train busy - this just means the train was previously activated
        }
    
		
		m_trainManager.getTrainStatusManager().setCctvActivated(trainID, true);
		double trainZone = m_trainManager.getTrainStatusManager().getTrainCctvZone(trainID);
	
		TrainStatus::CctvSwitchState switchState;
		switchState.mode=TrainStatus::seq;
		switchState.item1 = 1;
		switchState.item2 = 0;
		switchState.item3 = 0;
		switchState.item4 = 0;

        // store the fact its a sequence now
        m_trainManager.getTrainStatusManager().setCctvSwitchState(trainID, switchState);
		
		m_notifier.notifyCCTV(trainID, true, m_trainManager.getTrainStatusManager().isPreConflict(trainID), getStageId(trainZone), switchState);
    }
    catch (TrainException & e) 
    {
		m_trainManager.getTrainStatusManager().setCctvActivated(trainID, false);
        TA_RETHROW( e );
    }
}


void TrainCCTV::cctvCarrierOff(unsigned int trainID,std::string sessionID)
{
	TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncCctv);

    // Send command to train
    cctvCarrierOffCommand command; 
    
    command.setTrainID(trainID);
    
    try
    {
		try
		{
			TrainEventPtr event = m_trainManager.sendTrainCommandWithWait(trainID, command, m_timerCCTV1);
		
			// Check event reply
			cctvCommandReceivedEventReader* reader = dynamic_cast<cctvCommandReceivedEventReader*>(event->getReader());
			if (0 == reader)
			{
				TA_THROW( TrainGeneralFailureException() );
			}
        
			// busy?
			checkBusy(reader);
		}
		catch (TrainGeneralFailureException & e) 
		{
			if (e.reason() == TrainGeneralFailureException::BAD_COMMAND)
			{
				// this will happen if the CCTV is already on
				// just ignore the exception and treat it as a pass
			}
			else
			{
				TA_RETHROW( e );
			}
		}
		
		//TODO: uncomment above and remove line below when Glen fixes ats/radio locking
		//m_trainManager.sendTrainCommandNoReply(trainID, command);
        
		resetCCTVState(trainID);
    }
    catch (TrainException & e) 
    {
		//TD 15954
		//zhou yuan++
		resetCCTVState(trainID);
		//++zhou yuan
        TA_RETHROW( e );
    }
}


void TrainCCTV::cctvQuadScreen(unsigned int trainID,std::string sessionID,unsigned char cameraW,unsigned char cameraX,
                               unsigned char cameraY,unsigned char cameraZ)
{
	TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncCctv);
 
	// Send command to train
    cctvQuadScreenCommand command; 

    command.setTrainID(trainID);
    command.setCameras(cameraW, cameraX, cameraY, cameraZ);
    
    try
    {    
        TrainEventPtr event = m_trainManager.sendTrainCommandWithWait(trainID, command, m_timerCCTV1);
        
        // Check event reply
        cctvCommandReceivedEventReader* reader = dynamic_cast<cctvCommandReceivedEventReader*>(event->getReader());
        if (0 == reader)
        {
            TA_THROW( TrainGeneralFailureException() );
        }
        
        // busy?
        checkBusy(reader);
		
		TrainStatus::CctvSwitchState switchState;
		switchState.mode=TrainStatus::quad;
		switchState.item1 = cameraW;
		switchState.item2 = cameraX;
		switchState.item3 = cameraY;
		switchState.item4 = cameraZ;
		
        m_trainManager.getTrainStatusManager().setCctvActivated(trainID, true);
		m_trainManager.getTrainStatusManager().setCctvSwitchState(trainID, switchState);
		double trainZone = m_trainManager.getTrainStatusManager().getTrainCctvZone(trainID);
		m_notifier.notifyCCTV(trainID, true, m_trainManager.getTrainStatusManager().isPreConflict(trainID), getStageId(trainZone), switchState);
    }
    catch (TrainException  & e) 
    {
        TA_RETHROW( e );
    }
    
}


void TrainCCTV::cctvSingleScreen(unsigned int trainID, std::string sessionID, unsigned char cameraX)
{
	TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncCctv);
 
	// Send command to train
    cctvSingleScreenCommand command; 
    
    command.setTrainID(trainID);
    command.setCamera(cameraX);
    
    try
    {    
        TrainEventPtr event = m_trainManager.sendTrainCommandWithWait(trainID, command, m_timerCCTV1);
        
        // Check event reply
        cctvCommandReceivedEventReader* reader = dynamic_cast<cctvCommandReceivedEventReader*>(event->getReader());
        if (0 == reader)
        {
            TA_THROW( TrainGeneralFailureException() );
        }
        
        // busy?
        checkBusy(reader);

		TrainStatus::CctvSwitchState switchState;
		switchState.mode=TrainStatus::cam;
		switchState.item1 = cameraX;
		switchState.item2 = 0;
		switchState.item3 = 0;
		switchState.item4 = 0;
		
        m_trainManager.getTrainStatusManager().setCctvActivated(trainID, true);
		m_trainManager.getTrainStatusManager().setCctvSwitchState(trainID, switchState);
		double trainZone = m_trainManager.getTrainStatusManager().getTrainCctvZone(trainID);
		m_notifier.notifyCCTV(trainID, true, m_trainManager.getTrainStatusManager().isPreConflict(trainID), getStageId(trainZone), switchState);
    }
    catch (TrainException & e) 
    {
        TA_RETHROW( e );
    }
    
}


void TrainCCTV::cctvSequence(unsigned int trainID,std::string sessionID,unsigned char sequenceNo, unsigned char sequenceMode)
{
	TrainStateUpdateObserver tsuo(m_trainManager.getTrainStatusManager(), trainID, syncCctv);

    // Send command to train
    cctvSequenceCommand command; 
    
    command.setTrainID(trainID);
    command.setSequence(sequenceNo, sequenceMode);
    
    try
    {    
        TrainEventPtr event = m_trainManager.sendTrainCommandWithWait(trainID, command, m_timerCCTV1);
        
        // Check event reply
        cctvCommandReceivedEventReader* reader = dynamic_cast<cctvCommandReceivedEventReader*>(event->getReader());
        if (0 == reader)
        {
            TA_THROW( TrainGeneralFailureException() );
        }
        
        // busy?
        checkBusy(reader);

		TrainStatus::CctvSwitchState switchState;
		switchState.mode=TrainStatus::seq;
		switchState.item1 = sequenceNo;
		switchState.item2 = 0;
		switchState.item3 = 0;
		switchState.item4 = 0;
		
        m_trainManager.getTrainStatusManager().setCctvActivated(trainID, true);
    	m_trainManager.getTrainStatusManager().setCctvSwitchState(trainID, switchState);
		double trainZone = m_trainManager.getTrainStatusManager().getTrainCctvZone(trainID);
		m_notifier.notifyCCTV(trainID, true, m_trainManager.getTrainStatusManager().isPreConflict(trainID), getStageId(trainZone), switchState);
    }
    catch (TrainException & e) 
    {
        TA_RETHROW( e );
    }
    
}

void TrainCCTV::onCctvZoneUpdate(TA_IRS_App::ATSTrainMessage &atsMsg)
{
	
	//should be handled differently
	CarrierConflict conflict = checkConflicts(atsMsg.trainID, false, false);
	switch(conflict.type)
	{
		case noConflict:
			if(m_trainManager.getTrainStatusManager().isPreConflict(atsMsg.trainID))
			{
				m_trainManager.getTrainStatusManager().setPreConflict(atsMsg.trainID, 0);
				m_notifier.closeCctvPreConflictAlarm(atsMsg.trainID);
			}
			break;
		case preConflict:
			m_notifier.raiseCctvPreConflictAlarm(conflict.offendingTrain, conflict.owningTrain);
			m_trainManager.getTrainStatusManager().setPreConflict(atsMsg.trainID, conflict.owningTrain);
			break;
		case imminentConflict:
			//TD15147
//			m_notifier.raiseCctvConflictAlarm(conflict.offendingTrain, conflict.owningTrain);
			std::string sessionid("");
			
			TA_Base_Core::DescriptionParameters parameters;
			
			TA_Base_Core::NameValuePair param1("OffendingTrain", ""+conflict.offendingTrain);
			parameters.push_back(&param1);
			
			TA_Base_Core::NameValuePair param2("OwningTrain", ""+conflict.owningTrain);
			parameters.push_back(&param2);

			m_notifier.AuditOperation(TA_Base_Core::TrainAudit::TrainCCTVConflict, parameters, sessionid);

			m_trainManager.getTrainStatusManager().setPreConflict(atsMsg.trainID, 0);
			m_notifier.closeCctvPreConflictAlarm(atsMsg.trainID);
			cctvCarrierOff(conflict.offendingTrain,"");
			break;
	}

	
	if(m_trainManager.getTrainStatusManager().isCctvActivated(atsMsg.trainID))
	{
		if( isStationBoundary(m_trainManager.getTrainStatusManager().getTrainCctvZone(atsMsg.trainID)) &&
			!m_operationModeManager.IsGroupOnline())
		{
			cctvCarrierOff(atsMsg.trainID, "");
			//trains leaving a station when in degraded mode need to be switched off
		}
		m_notifier.notifyCCTV(atsMsg.trainID, 
			m_trainManager.getTrainStatusManager().isCctvActivated(atsMsg.trainID), 
			m_trainManager.getTrainStatusManager().isPreConflict(atsMsg.trainID), 
			getStageId(m_trainManager.getTrainStatusManager().getTrainCctvZone(atsMsg.trainID)), 
			m_trainManager.getTrainStatusManager().getCctvSwitchState(atsMsg.trainID));
	}
}

void TrainCCTV::CCTVAddAlarmImage(unsigned int trainID, std::string sessionID, const std::vector<unsigned char>& cameras )
{
    TrainStatus::CctvSwitchState state = m_trainManager.getTrainStatusManager().getCctvSwitchState(trainID);

    // if the carrier is off, do the conflict checks, but dont activate
    if(!m_trainManager.getTrainStatusManager().isCctvActivated(trainID))
	{
        // do the conflict checks
		aboutToActivate(trainID);

        // ensure switch state is blank
		state.mode=TrainStatus::seq;
		state.item1 = 1;
		state.item2 = 0;
		state.item3 = 0;
		state.item4 = 0;
	}

    // work out the camera assignments to make
    unsigned char newItem1 = 0;
    unsigned char newItem2 = 0;
    unsigned char newItem3 = 0;
    unsigned char newItem4 = 0;

    if( (state.mode == TrainStatus::seq) || (state.item1 == 0) )
	{
        // was a sequence, initialise the new state blank (already is)
	}
    else
	{
        // otherwise there was a quad or single assigned
        // intialise the new state to the current state
        newItem1 = state.item1;
        newItem2 = state.item2;
        newItem3 = state.item3;
        newItem4 = state.item4;
    }

    // add each new camera (do it backwards so the order is maintained)
    for (std::vector<unsigned char>::const_reverse_iterator iter = cameras.rbegin(); iter != cameras.rend(); ++iter)
    {
        unsigned char cameraX = *iter;

        // now see if the new camera is in the top 4 in the list

        if(cameraX == newItem1)
		{
			// do nothing - already the first in the quad
		}
		else if(cameraX == newItem2)
		{
            // it is the second in the quad, swap it with the first
            newItem2 = newItem1;
            newItem1 = cameraX;
		}
		else if(cameraX == newItem3)
		{
            // it is the third in the quad, move it to the front and shuffle items 1 and 2 down
            newItem3 = newItem2;
            newItem2 = newItem1;
            newItem1 = cameraX;
		}
		else
		{
            // not in the existing quad, or the last item in the quad
            // push it on the front, shuffle the rest down
            newItem4 = newItem3;
            newItem3 = newItem2;
            newItem2 = newItem1;
            newItem1 = cameraX;
		}
    }

    // now work out what to assign to the train
    // after all this, there should be at least one item
    TA_ASSERT(newItem1 != 0, "There are no items in the new state");

    // if there is more than one item
    if ( newItem2 != 0 )
    {
        // assign a quad
        cctvQuadScreen(trainID, sessionID, newItem1, newItem2, newItem3, newItem4);
    }
    else
    {
        // only a single screen
        cctvSingleScreen(trainID, sessionID, newItem1);
    }
}

void TrainCCTV::CCTVRemoveAlarmImage(unsigned int trainID, std::string sessionID, unsigned char cameraX)
{
	if(m_trainManager.getTrainStatusManager().isCctvActivated(trainID))
	{
		TrainStatus::CctvSwitchState state = m_trainManager.getTrainStatusManager().getCctvSwitchState(trainID);
		
        // if it is not a quad, or it is a quad with a single assignment
		if(state.mode != TrainStatus::quad || state.item2==0)
		{
            // turn the video off
			cctvCarrierOff(trainID, sessionID);
		}
        // tis a quad
		else
		{
			try
			{
				// if there are only 2 items, figure out which item is being cleared and
				// put the other one on full screen
				if(state.item3==0)
				{
					if(state.item1 == cameraX)
					{
						cctvSingleScreen(trainID, sessionID, state.item2);
					}
					else if(state.item2 == cameraX)
					{
						cctvSingleScreen(trainID, sessionID, state.item1);
					}
				}
				else
				{
					if(state.item1 == cameraX)
					{
						cctvQuadScreen(trainID, sessionID, state.item2, state.item3, state.item4, 0);
					}
					else if(state.item2 == cameraX)
					{
						cctvQuadScreen(trainID, sessionID, state.item1, state.item3, state.item4, 0);
					}
					else if(state.item3 == cameraX)
					{
						cctvQuadScreen(trainID, sessionID, state.item1, state.item2, state.item4, 0);
					}
					else if(state.item4 == cameraX)
					{
						cctvQuadScreen(trainID, sessionID, state.item1, state.item2, state.item3, 0);
					}
				}
			}
			catch(const TrainException& e)
			{
				//TD 15954
				//zhou yuan++
				resetCCTVState(trainID);
				//++zhou yuan
				TA_RETHROW(e);
			}
		}
	}
}

//TD 15954
//zhou yuan++
void TrainCCTV::resetCCTVState(unsigned int trainID)
{
	TrainStatus::CctvSwitchState switchState;
	switchState.mode=TrainStatus::seq;
	switchState.item1 = 1;
	switchState.item2 = 0;
	switchState.item3 = 0;
	switchState.item4 = 0;
	
	m_trainManager.getTrainStatusManager().setCctvActivated(trainID, false);
	m_trainManager.getTrainStatusManager().setCctvSwitchState(trainID, switchState);
	
	CarrierConflict conflict = checkConflicts(trainID, false, true);
	m_trainManager.getTrainStatusManager().setPreConflict(conflict.offendingTrain, 0);
	m_notifier.closeCctvPreConflictAlarm(conflict.offendingTrain);
	
	if(conflict.owningTrain!=0)
	{
		m_trainManager.getTrainStatusManager().setPreConflict(conflict.owningTrain, 0);
		m_notifier.closeCctvPreConflictAlarm(conflict.owningTrain);
	}
	
	double trainZone = m_trainManager.getTrainStatusManager().getTrainCctvZone(trainID);

	m_notifier.notifyCCTV(trainID, false, m_trainManager.getTrainStatusManager().isPreConflict(trainID), getStageId(trainZone), switchState);
}
//++zhou yuan

CarrierConflict TrainCCTV::checkConflicts(TrainID trainID, bool willBeActivated, bool hasBeenDeactivated)
{
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Checking Conflicts For %d", static_cast<int>(trainID));

	CarrierConflict conflict;
	conflict.offendingTrain=trainID;
	conflict.owningTrain = 0;
	conflict.type = noConflict;
	conflict.stage = 0;
	bool degraded = false;
	//we dont want our logic changing in the middle of our algorythm
	degraded = !m_operationModeManager.IsGroupOnline();

	if(willBeActivated || hasBeenDeactivated || m_trainManager.getTrainStatusManager().isCctvActivated(trainID))
	{
		//train one will be the offending train as it is the one that has just moved or is trying to be turned
		//on or off
		int train1CurrentStage;
		int train2CurrentStage;
		int train1NextStage;
		int train2NextStage;
		bool train1PreConflict = false;
		bool train2PreConflict = false;

		double offenderZone;
		if(!degraded)
		{
			offenderZone = m_trainManager.getTrainStatusManager().getTrainCctvZone(trainID);
		}
		else
		{
			//if we are degraded we just car about the trains location, not its stage
			offenderZone = m_trainManager.getTrainStatusManager().getTrainLocation(trainID);
		}
		
		train1PreConflict = isPreConflict(offenderZone);
		train1CurrentStage = getStageId(offenderZone);
		train1NextStage = getNextStageId(offenderZone);

		TrainList allTrains = m_trainManager.getTrainStatusManager().getTrainList(false);

		//second number in the zoneID is the next stage number
		// and is where we have to check for other broadcasting trains
		TrainList::iterator it;
		for(it=allTrains.begin(); it!=allTrains.end(); it++)
		{
			if((*it)!=trainID && m_trainManager.getTrainStatusManager().isCctvActivated(*it))
			{					
				double owningTrainZone;
				if(!degraded)
				{
					owningTrainZone = m_trainManager.getTrainStatusManager().getTrainCctvZone(*it);
				}
				else
				{
					owningTrainZone = m_trainManager.getTrainStatusManager().getTrainLocation(*it);
				}

				
				train2CurrentStage = getStageId(owningTrainZone);
				train2NextStage = getNextStageId(owningTrainZone);
				train2PreConflict = isPreConflict(owningTrainZone);

				// if a train is in the next station (first number being the same as our
				// next number (regardsless of sign (pre/imminent conflict)
				// we are in conflict
				
				if((train1NextStage == train2CurrentStage && !(train2NextStage != train1CurrentStage && !train2PreConflict && train2NextStage>0)) || 
					(train1NextStage == train2NextStage && !train2PreConflict && train1NextStage >0 && train2NextStage>0) &&
					!(train1CurrentStage == train2CurrentStage))
				{
					if(train1PreConflict && !(train2NextStage == train1CurrentStage && !train2PreConflict && train2NextStage>0)) 
					{
						conflict.stage = train2CurrentStage;
						conflict.type = preConflict;
						LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Pre Conflict For %d with %d", static_cast<int>(trainID), *it);
					}
					else
					{
						if(train2CurrentStage == train1NextStage && train1NextStage>0 && !train1PreConflict)
						{
							conflict.stage = train1NextStage;
						}
						else
						{
							conflict.stage = train1CurrentStage;
						}
						conflict.type = imminentConflict;
						LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Imminent Conflict For %d with %d", static_cast<int>(trainID), *it);
					}
					conflict.owningTrain = (*it);
				}
				else
				{
					if((train1CurrentStage == train2CurrentStage) || 
						(train1CurrentStage == train2NextStage && !train2PreConflict && train2NextStage>0))
					{
						conflict.type = imminentConflict;
						conflict.stage = train1CurrentStage;
						conflict.owningTrain = (*it);
					}
				}

////////////////////////////////////////////////////////////////////////////////////////////////////////
				//we also need to check conflicts where our train is not the offending train, but is actually the owning train
				// eg. if train two is in preconflict zone -2.1 (stge 2 moving to stage one, it doesnt actually own stage one yet
				// so if an operator activates train 1 in stage 1, train one owns the stage. but wont be in conflict, train 2 will be


				CarrierConflict conflict2;
				conflict2.offendingTrain=0;
				conflict2.owningTrain = trainID;
				conflict2.type = noConflict;
				conflict2.stage = 0;

				if((train2NextStage == train1CurrentStage) || 
					(train2NextStage == train1NextStage && !train1PreConflict && train2NextStage >0 && train1NextStage>0))
				{
					if(train2PreConflict)
					{
						conflict2.type = preConflict;
						LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Pre Conflict For %d with %d", static_cast<int>(trainID), *it);
					}
					else
					{
						conflict2.type = imminentConflict;
						LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Imminent Conflict For %d with %d", static_cast<int>(trainID), *it);
					}
					conflict2.offendingTrain = (*it);
					conflict2.stage = train2NextStage;
				}
				else
				{
					if((train2CurrentStage == train1CurrentStage) || 
						(train2CurrentStage == train1NextStage && !train1PreConflict && train1NextStage>0))
					{
						conflict2.type = imminentConflict;
						conflict2.stage = train2CurrentStage;
						conflict2.offendingTrain = (*it);
					}
				}

				//should be handled differently
				if(conflict2.offendingTrain != 0)
				{
							
					switch(conflict2.type)
					{
						case noConflict:
							m_trainManager.getTrainStatusManager().setPreConflict(conflict2.offendingTrain, 0);
							m_notifier.closeCctvPreConflictAlarm(conflict.offendingTrain);
							break;
						case preConflict:
							if(hasBeenDeactivated)
							{
								m_trainManager.getTrainStatusManager().setPreConflict(conflict2.offendingTrain, 0);
								m_notifier.closeCctvPreConflictAlarm(conflict2.offendingTrain);
							}
							else if(conflict.type!=imminentConflict && willBeActivated==true)
							{
								m_notifier.raiseCctvPreConflictAlarm(conflict2.offendingTrain, conflict2.owningTrain);
								m_trainManager.getTrainStatusManager().setPreConflict(conflict2.offendingTrain, conflict2.owningTrain);
							}
							break;
						case imminentConflict:
							//TA_ASSERT(false, "This should never occur as the previous conflict would pick this case up");
							break;
					}
					m_notifier.notifyCCTV(conflict2.offendingTrain, 
						true, 
						m_trainManager.getTrainStatusManager().isPreConflict(conflict2.offendingTrain), 
						train2CurrentStage, 
						m_trainManager.getTrainStatusManager().getCctvSwitchState(conflict2.offendingTrain));
				}

				//and last but not least we need to check if this train went out of conflict by itself
				if(m_trainManager.getTrainStatusManager().getPreConflict(*it)==trainID)
				{
					if(!((train2NextStage == train1CurrentStage) || 
						(train2NextStage == train1NextStage && !train1PreConflict && train2NextStage >0 && train1NextStage>0)))
					{
						m_trainManager.getTrainStatusManager().setPreConflict(*it, 0);
						m_notifier.closeCctvPreConflictAlarm(*it);
			
						m_notifier.notifyCCTV(*it,
												true, 
												false, 
												train2CurrentStage, 
												m_trainManager.getTrainStatusManager().getCctvSwitchState(*it));
				
					}
				}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
			}
		}//end for
	}//end if(willBeActivated || hasBeenD......


	return conflict;
}

unsigned long TrainCCTV::getStageId(double cctvZone)
{
	if(isPreConflict(cctvZone))
	{
		cctvZone *= -1;
	}
	unsigned long stage = 0;
	
	std::stringstream stageExtract;
	stageExtract << cctvZone;

	stageExtract >> stage;
	
	return stage;
}

unsigned long TrainCCTV::getNextStageId(double cctvZone)
{
	if(isPreConflict(cctvZone))
	{
		cctvZone *= -1;
	}
	unsigned long nextStage = 0;
	char dot;
	unsigned long currentStage = 0;
	
	std::stringstream stageExtract;
	stageExtract << cctvZone;

	if (stageExtract >> currentStage)
	{			
		stageExtract >> dot;
		stageExtract >> nextStage;
	}

	if( nextStage==currentStage )
	{
		nextStage=0;
		//this config only exsists for degraded and really means the same as no next stage
		// ie. 1.1 = 1
	}


	return nextStage;
}

bool TrainCCTV::isStationBoundary(double cctvZone)
{
	if(isPreConflict(cctvZone))
	{
		return false;
	}
	
	unsigned long nextStage = 0;
	char dot;
	unsigned long currentStage = 0;
	
	std::stringstream stageExtract;
	stageExtract << cctvZone;

	if (stageExtract >> currentStage)
	{			
		stageExtract >> dot;
		stageExtract >> nextStage;
	}

	if( nextStage!=0 )
	{
		return true;
		//zone in the form + 1.something means we are on the boundary
	}

	return false;
}

bool TrainCCTV::isPreConflict(double cctvZone)
{
	return (cctvZone<0);
}

//decoupling stupid Corba crap to achieve asyncronous message handling
std::string TrainCCTVDefaultTrainCommand::info()
{
	std::ostringstream s;
	s << "TrainCCTVDefaultTrainCommand [Train=" << static_cast<int>(m_trainId) << "]";

	return s.str();
}


int TrainCCTVDefaultTrainCommand::call()
{
	FUNCTION_ENTRY("TrainCCTVDefaultTrainCommand::call()");

	try
	{
		m_trainCctv.cctvSequence(m_trainId, m_session, 1, 0x3);
		m_trainCctv.cctvCarrierOff(m_trainId, m_session);
	}
	catch (TrainException &e)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TrainException", e.what());
	}
	catch (...)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "<unknown exception>", "Error in Cctv worker thread");
	}
	
	FUNCTION_EXIT;
	return 0;
	
}



	
