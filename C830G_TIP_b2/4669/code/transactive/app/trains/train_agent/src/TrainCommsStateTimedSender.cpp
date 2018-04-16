#include "app/trains/train_agent/src/TrainCommsStateTimedSender.h"

#include "app/trains/train_agent/src/TrainCommsState.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/timers/src/SingletonTimerUtil.h"


namespace TA_IRS_App
{
	
    TrainCommsStateTimedSender::TrainCommsStateTimedSender( IStateUpdateMessageSender& stateUpdateSender ,TrainAgentStatusManager & manager )
		: m_stateUpdateSender( stateUpdateSender ),is_initialstate_loaded(false),m_statusManager(manager)
    {
        FUNCTION_ENTRY( "TrainCommsStateTimedSender" );
		
        m_timerID=TA_Base_Core::SingletonTimerUtil::getInstance().startPeriodicTimeOutClock( this, 500, false );
		
        FUNCTION_EXIT;
    }
	
    TrainCommsStateTimedSender::~TrainCommsStateTimedSender()
    {
        FUNCTION_ENTRY( "~TrainCommsStateTimedSender" );
		
        TA_Base_Core::SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock(this,m_timerID);
		
        FUNCTION_EXIT;
    }
	void TrainCommsStateTimedSender::setTrainCommsSyncState(TrainCommsStatePtr state)
	{
		FUNCTION_ENTRY( "setTrainCommsSyncState" );

		boost::shared_ptr<TrainCommsState> newState(new TrainCommsState(*state));

		TA_THREADGUARD(m_stateLock);
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Assigning TrainCommsSyncState for train id: %d",state->getTrainId());
		trainid_to_commstate_map[state->getTrainId()]= newState;
		if(!is_initialstate_loaded)
			is_initialstate_loaded=true;

		FUNCTION_EXIT;
	}
	void TrainCommsStateTimedSender::timerExpired(long timerId, void* userData)
    {
        FUNCTION_ENTRY( "timerExpired" );
		TA_THREADGUARD(m_stateLock);
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Timer expired. Trying to send TrainCommsSyncState, IsInitialStateLoaded=%d",is_initialstate_loaded);
		if(is_initialstate_loaded && trainid_to_commstate_map.size()>0)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Sending TrainCommsSyncState to %d trains", trainid_to_commstate_map.size());
			std::vector<TA_IRS_Bus::TrainStateLock *> locks;
			TA_IRS_Bus::StateUpdateTypes::CommsStateList commsList;
			for (TA_IRS_Bus::TrainIdToStateMap::iterator stateIter = trainid_to_commstate_map.begin(); trainid_to_commstate_map.end() != stateIter; ++stateIter )
			{
				TrainCommsStatePtr commsState = boost::dynamic_pointer_cast< TrainCommsState >( stateIter->second );
				TA_ASSERT( NULL != commsState.get(), "Cannot cast to correct state" );
				//TA_IRS_Bus::TrainStateLock *lock=new TA_IRS_Bus::TrainStateLock(commsState);
				//locks.push_back(lock);
				
				commsList.push_back( commsState->getSyncState() );
			}
			m_stateUpdateSender.sendStateUpdate(commsList);

			/*for (TA_IRS_Bus::TrainIdToStateMap::iterator cstateIter = trainid_to_commstate_map.begin(); trainid_to_commstate_map.end() != cstateIter; ++cstateIter )
			{
				TrainCommsStatePtr commsState = boost::dynamic_pointer_cast< TrainCommsState >( cstateIter->second );
				commsState->setSynchronisationComplete();
			}
			trainid_to_commstate_map.clear();
			for(std::vector<TA_IRS_Bus::TrainStateLock *>::iterator it=locks.begin();it!=locks.end();it++)
			{
				delete *it;
			}
			locks.clear(); */

			commsList.clear();

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Completed sending TrainCommsSyncState messages. Cleared the locks.");
		}
        
        FUNCTION_EXIT;
    }
	
} // TA_IRS_App
