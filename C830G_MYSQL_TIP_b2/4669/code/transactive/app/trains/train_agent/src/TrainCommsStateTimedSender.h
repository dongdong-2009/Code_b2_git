
#if !defined(TrainCommsStateTimedSender_H)
#define TrainCommsStateTimedSender_H
#include "app/trains/train_agent/src/IStateUpdateMessageSender.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/timers/src/ITimeoutCallback.h"
#include "app/trains/train_agent/src/TrainCommsState.h"
#include "bus/trains/TrainTransactionModel/src/TrainStateLock.h"
#include "app/trains/train_agent/src/TrainAgentStatusManager.h"
#include<map>
using std::map;
namespace TA_IRS_App
{
	
/**
* This handles the broadcasting of CommsState Messages at regular intervals.
	*/
    class TrainCommsStateTimedSender :TA_Base_Core::ITimeoutCallback
    {
		
    public:
		
        TrainCommsStateTimedSender( IStateUpdateMessageSender& stateUpdateSender,TrainAgentStatusManager & manager);
        virtual ~TrainCommsStateTimedSender();
		void setTrainCommsSyncState(TrainCommsStatePtr state);
        virtual void timerExpired(long timerId, void* userData);
		
	private:
        mutable TA_Base_Core::ReEntrantThreadLockable m_stateLock;
		
		IStateUpdateMessageSender& m_stateUpdateSender;
		TrainAgentStatusManager & m_statusManager;
		bool is_initialstate_loaded;
		long m_timerID;
		TrainCommsStatePtr m_commsState;
		TA_IRS_Bus::TrainIdToStateMap trainid_to_commstate_map;
    };
	
}  // TA_IRS_App

#endif // !defined(TrainCommsStateTimedSender_H)
