/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/app/trains/TrainBorne_PA/src/TrainLiveStatusModel.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by: $Author: grace.koh $
  * 
  * TrainLiveStatusModel controls additional layer of business logic for Live
  * broadcast dialog
  *
  */

// Disable std::deque warnings
#pragma warning(disable: 4018 4146)

#include "app/trains/TrainBorne_PA/src/StdAfx.h"

#include "app/trains/TrainBorne_PA/src/CachedConfig.h"
#include "app/trains/TrainBorne_PA/src/TrainLiveStatusModel.h"

#include <algorithm>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

TrainLiveStatusModel::TrainLiveStatusModel(GenericStatusPage& page, const std::string& broadcastId) :
LiveStatusModel(page, broadcastId, GenericStatusModel::TRAIN),
m_hasOverrideOption(false),
m_existTrainsWaitingToBeginBroadcast(false)
{
    FUNCTION_ENTRY( "TrainLiveStatusModel" );
    FUNCTION_EXIT;
}


TrainLiveStatusModel::~TrainLiveStatusModel()
{
    FUNCTION_ENTRY( "~TrainLiveStatusModel" );
    FUNCTION_EXIT;
}


bool TrainLiveStatusModel::getHasOverrideOption()
{
    FUNCTION_ENTRY( "getHasOverrideOption" );
    FUNCTION_EXIT;
    return m_hasOverrideOption;
}


void TrainLiveStatusModel::processProgressUpdateData(const TA_Base_Bus::IPAAgentCorbaDef::BroadcastProgressUpdateData& lockedData)
{
    FUNCTION_ENTRY( "processProgressUpdateData" );

    // Pass up to main implementation
    LiveStatusModel::processProgressUpdateData(lockedData);

    // Additionally have to track the "continue required" states of trains
    const TA_Base_Bus::IPAAgentCorbaDef::LocalProgressSeq& trainData = lockedData.localProgresses;

    bool oneOrMoreTrainsConnected = false;
    bool oneOrMoreTrainsInProgress = false;

    for (unsigned int i = 0; i < trainData.length(); i ++)
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "BroadcastState: %d, Global state: %d, TrainId: %d, Local state : %d,", lockedData.broadcastState, lockedData.globalState, trainData[i].zoneKeyOrTrainId, trainData[i].localState );
		if ( TA_Base_Bus::IPAAgentCorbaDef::Runtime == lockedData.broadcastState)
		{
			processContinueRequiredStateChange(trainData[i].zoneKeyOrTrainId, trainData[i].localState);
		}

        if (TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_TRAIN_READY ==  trainData[i].localState)
        {
            oneOrMoreTrainsConnected = true;
        }
        else if (TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_IN_PROGRESS ==  trainData[i].localState)
        {
            oneOrMoreTrainsInProgress = true;
        }
    }

    // Only when train hits connected state is it ready for receive a broadcast request
    m_existTrainsWaitingToBeginBroadcast = oneOrMoreTrainsConnected && !oneOrMoreTrainsInProgress;

    FUNCTION_EXIT;
}


void TrainLiveStatusModel::processContinueRequiredStateChange(int trainId,
                                                              TA_Base_Bus::IPAAgentCorbaDef::ELocalState state)
{
    FUNCTION_ENTRY( "processContinueRequiredStateChange" );

    // Whenever there is a state change from anything to 'continue required',
    // or from 'continue required' to 'In Progress' respond
    // by adding (or removing) these items from 'continue required' queue
    std::map<int, TA_Base_Bus::IPAAgentCorbaDef::ELocalState>::iterator itr =
                                        m_continueRequiredMap.find(trainId);

    bool queueUpdateRequired = false;

    if (itr == m_continueRequiredMap.end())
    {
        // If the item defined by trainId doesn't exist in queue, it only
        // gets added to the queue if continueRequired is true
        queueUpdateRequired =
            (TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_CONTINUE_LIVE_ANNOUNCEMENT_REQUIRED ==  state);
    }
    else
    {
        if (itr->second != state)
        {
            // Item exists in queue and state has changed, determine updateRequired state
            if (TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_CONTINUE_LIVE_ANNOUNCEMENT_REQUIRED ==  state)
            {
                // Going from any state to continue is accepted
                queueUpdateRequired = true;
            }
            else if(TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_IN_PROGRESS ==  state)
            {
                // If entering an In Progress state, only add to queue if currently in a
                // continue required state (means confirmation has occurred)
                if (TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_CONTINUE_LIVE_ANNOUNCEMENT_REQUIRED == itr->second)
                {
                    queueUpdateRequired = true;
                }
            }
        }
    }

    // Update the item in the map
    m_continueRequiredMap[trainId] = state;

    if (queueUpdateRequired)
    {
        TrainContinueState newState;
        newState.trainId = trainId;
        newState.continueRequired =
            (TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_CONTINUE_LIVE_ANNOUNCEMENT_REQUIRED ==  state);

        // Add the item to the statechange FIFO queue -
        // do not expect the client code to query this class for
        // the continueRequired state - important it receives
        // the older copy if the state is changed again before
        // the state change queue is purged (for correctness)
        m_continueStateChangeQueue.push_back(newState);
    }

    FUNCTION_EXIT;
}


bool TrainLiveStatusModel::doesNewContinueRequiredTrainExist()
{
    FUNCTION_ENTRY( "doesNewContinueRequiredTrainExist" );
    FUNCTION_EXIT;
    return m_continueStateChangeQueue.size() > 0;
}


TrainLiveStatusModel::TrainContinueState TrainLiveStatusModel::popFrontNewContinueRequiredTrain()
{
    FUNCTION_ENTRY( "popFrontNewContinueRequiredTrain" );

    TA_ASSERT(doesNewContinueRequiredTrainExist(), "Invalid call to popFrontContinueRequiredTrain");

    TrainContinueState result = m_continueStateChangeQueue.front();
    m_continueStateChangeQueue.pop_front();

    FUNCTION_EXIT;
    return result;
}


void TrainLiveStatusModel::requestContinueOnTrain()
{
    FUNCTION_ENTRY( "requestContinueOnTrain" );

    CWaitCursor hourGlassCursor;

    // Allow exceptions to propogate
    CORBA_CALL( CachedConfig::getInstance()->getAgentObject(), continueTrainLiveAnnouncement, ( getBroadcastId().c_str() ) );//limin

    FUNCTION_EXIT;
}


void TrainLiveStatusModel::endLiveAnnouncement()
{
    FUNCTION_ENTRY( "endLiveAnnouncement" );

    CWaitCursor hourGlassCursor;

    // Allow exceptions to propogate
    CORBA_CALL( CachedConfig::getInstance()->getAgentObject(), endTrainLiveAnnouncement, ( getBroadcastId().c_str() ) );//limin

    FUNCTION_EXIT;
}


void TrainLiveStatusModel::removeAnnouncement()
{
    FUNCTION_ENTRY( "endLiveAnnouncement" );

    try
    {
        CORBA_CALL(CachedConfig::getInstance()->getAgentObject(), removeBroadcast, (getBroadcastId().c_str(), ""));//limin
    }
    catch (const TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Bus::IPAAgentCorbaDef::PaErrorException", expWhat.strWhat.in());
    }
    catch (const TA_Base_Core::ObjectResolutionException& expWhat)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException", expWhat.what());
    }
    catch (const CORBA::Exception&)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", "Agent fail");
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Caught unknown exception when removeAnnouncement");
    }    

    FUNCTION_EXIT;
}


void TrainLiveStatusModel::beginLiveAnnouncement()
{
    FUNCTION_ENTRY( "beginLiveAnnouncement" );

    CWaitCursor hourGlassCursor;

    // Allow exceptions to propogate
    CORBA_CALL( CachedConfig::getInstance()->getAgentObject(), beginTrainLiveAnnouncement, ( getBroadcastId().c_str() ) );//limin

    FUNCTION_EXIT;
}


void TrainLiveStatusModel::resetContinueRequiredStates()
{
    FUNCTION_ENTRY( "resetContinueRequiredStates" );

    m_continueRequiredMap.clear();
    m_continueStateChangeQueue.clear();

    FUNCTION_EXIT;
}


