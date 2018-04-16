#ifndef ACTIVEPLANBULLETINBOARD_H_INCLUDED
#define ACTIVEPLANBULLETINBOARD_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #13 $
  *
  *  Last modification: $DateTime: 2015/04/29 15:01:53 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  A thread-safe Singleton factory for converting database data into
  *  application data and vice-versa.
  */

#include <map>
#include <list>

#include "ace/RW_Mutex.h"
#include "ace/Mutex.h"
#include "ace/Future.h"
#include "ace/Singleton.h"

namespace TA_Base_Core
{
    class StateUpdateMessageSender;
    class CommsMessageSender;
    class ActivePlanDetails;
    class ActivePlanStepsDetails;
    class PASUpdateDataSeq;
    class ActivePlanSharedDataSeq;
    struct ActiveStepDetail;
    struct ActivePlanDetail;
    struct ActivePlanSharedData;
    struct ActivePlanId;
    struct ExecutionHistory;
    struct CustomisationHistory;
    struct ExecutionHistoryUpdate;
    struct CustomisationHistoryUpdate;
    struct PlanCustomisationUpdate;
    struct StepCustomisableDetail;
    struct PASUpdateData;
    struct PlanConfigUpdateData;
    struct CurrentPlanStepDetail;
    struct PlanExecuteStepUpdate;
}

namespace TA_Base_Bus
{
    class RightsLibrary;
    class AuthenticationLibrary;
    class SortableActiveStepId;
}

namespace TA_Base_App
{
	// Class declaration
	// Note that a typedef following the class declaration declares the
	// type "ActivePlanBulletinBoard" and that "ActivePlanBulletinBoardImpl" can
	// not be used.
    class SortablePlanId;
    class PlanDetail;

    class ActivePlanBulletinBoardImpl
	{
	// Public methods
	public:

		friend class ACE_Singleton<ActivePlanBulletinBoardImpl, ACE_Recursive_Thread_Mutex>;

		bool isOwner(const std::string& session, const TA_Base_Core::ActivePlanId& plan);

		void setControlMode( bool controlMode );

        bool isControlMode(void);

        bool existsPlan(const TA_Base_Core::ActivePlanId& plan);
		////////////////////////////////////////////////////////////////////
		//returns true for plans that come from GRC category
		//according to the Plan Management System Software Requirements spec
		//to differentiate between GRC and DSS plans is to check which category
		//a plan belongs to and if it contains any BRC steps
		//Hence, the method is implemented to reflect this requirement
		////////////////////////////////////////////////////////////////////////
		bool isGRCPlan (const TA_Base_Core::ActivePlanId& plan);
		bool isManualPlan (const TA_Base_Core::ActivePlanId& plan);

		bool isActionPermitted(const std::string& session, unsigned long actionKey, std::string& reason);
		bool isActionPermitted(const std::string& session, unsigned long resouceKey, unsigned long actionKey, std::string& reason);

		std::string getSession( const TA_Base_Core::ActivePlanId& plan ) const;
        std::string getOwner( const std::string& session );
        unsigned long getOwnerId( const std::string& session );
        std::string getOperatorName( const TA_Base_Core::ActivePlanId& plan );
        bool getUserInfoFromSession( const std::string& session, unsigned long& userKey, unsigned long& userLocation, unsigned long& userWorkstation );
	    unsigned long getUserKey( const TA_Base_Core::ActivePlanId& plan );

        bool isAllPlanStopped() const;
        bool isActive( const unsigned long plan,  const TA_Base_Core::ActivePlanId& ignoreInstance ) const;
        bool isActive( const TA_Base_Core::ActivePlanId& plan ) const;
        TA_Base_Core::ExecutionHistory* getPlanExecutionHistory(const TA_Base_Core::ActivePlanId& plan, const bool bSendIdUpdate = true);
        TA_Base_Core::CustomisationHistory* getPlanCustomisationHistory(const TA_Base_Core::ActivePlanId& plan, const bool bSendIdUpdate = true);
        void getActiveInstances( const unsigned long plan, std::list<TA_Base_Core::ActivePlanId>& lstActiveInstances, const unsigned long ulFilterEntity = 0u, const bool bFilter = false ) const;
        TA_Base_Core::ActivePlanStepsDetails* getAllActivePlanDetails() const;
        TA_Base_Core::ActivePlanDetails* getActivePlanDetails( unsigned long plan ) const;
        const PlanDetail& getActivePlanDetail( const TA_Base_Core::ActivePlanId& plan ) const;
        const TA_Base_Core::ActivePlanDetail& getAssocAlarmActivePlanDetail (const std::string& alarmId);
        std::map<unsigned long, std::string> getAssocAlarmIds( const TA_Base_Core::ActivePlanId& plan );
        void resetAssocAlarmId( const TA_Base_Core::ActivePlanId& plan, const unsigned long ulStepNum );
        void setAssociatedAlarmId(const TA_Base_Core::ActivePlanId& plan, const unsigned long ulStepNum, const std::string& alarmId);
        int getPlanState( const TA_Base_Core::ActivePlanId& plan ) const;
        int getStepState( const TA_Base_Core::ActivePlanId& plan, const unsigned long ulStepNum ) const;

        bool reserveInstance( const std::string& session, TA_Base_Core::ActivePlanId& plan, const unsigned long entity, const bool bPending, bool manExecuted );
		bool reserveInstance( const std::string& session, const TA_Base_Core::ActivePlanId& parentPlan, TA_Base_Core::ActivePlanId& childPlan, const bool bPending );
        void undoReserveInstance( TA_Base_Core::ActivePlanId plan );
		void releaseInstance( const TA_Base_Core::ActivePlanId& plan );

        void initStep( const TA_Base_Core::ActivePlanId& plan, const unsigned long stepId, const unsigned long stepPos, const std::string& stepName );
		void updatePlanState( const TA_Base_Core::ActivePlanId& plan,const int state, const std::string& remark );
		void updateOwner( const TA_Base_Core::ActivePlanId& plan, const std::string& session );
		void updateStepState( const TA_Base_Core::ActivePlanId& plan, const unsigned long step, const int state, const std::string& remark );
		void updateStepRemark( const TA_Base_Core::ActivePlanId& plan, const unsigned long step, const std::string& remark );
        void updateCustom( const TA_Base_Core::ActivePlanId& plan, const TA_Base_Core::StepCustomisableDetail& detail );
        void updateActivePlanTimer( const TA_Base_Core::ActivePlanId& plan, const unsigned long ulTimer );
        void updateActivePlanDetail( const TA_Base_Core::ActivePlanDetail& refDetail );
        void updateActiveStepDetail( const TA_Base_Core::ActivePlanId& refPlanId, const TA_Base_Core::ActiveStepDetail& refDetail );
		void sendStateUpdateRequestToControlAgent( void );

        void processExecutionHistoryUpdate( const TA_Base_Core::ExecutionHistoryUpdate& historyUpdate );
        void processCustomisationHistoryUpdate( const TA_Base_Core::CustomisationHistoryUpdate& historyUpdate );

        void clearAllPlanStates();
		void cullStoppedPlans();
        void cullAllPlans();
		void deleteMessageSenders();
		void deleteRightsLib (void);

		void setSession( const TA_Base_Core::ActivePlanId& plan, const std::string& session );

		bool wasHangupMessageReceived(void);
		void setHangupMessageReceived(void);

		void sendConfigUpdateMessage( const TA_Base_Core::PlanConfigUpdateData& configData );

        void sendUpdateMessage(const TA_Base_Core::ActivePlanId& plan);

		void sendStateUpdateMessage( TA_Base_Core::PASUpdateData& stateUpdateData );
        void sendActivePlanStateMessage( const TA_Base_Core::ActivePlanDetail& planDetail );
        void sendActiveStepStateMessage( const TA_Base_Core::PlanExecuteStepUpdate& detail );
        void sendStateUpdateMessage( const unsigned long updateId );
        void sendAllStatesUpdateMessage( TA_Base_Core::PASUpdateDataSeq& refStateUpdateDataSeq );
		void sendAllStateUpdatesToMonitorAgent(void);

		unsigned long getNextUpdateId(void);
        unsigned long getNextPasUpdateId(void);
        void setPasUpdateId(unsigned long updateId);
        void setUpdateId(unsigned long updateId);

        void cullStoppedPlan(const TA_Base_Core::ActivePlanId& plan);

        void updatePlanSharedData(const TA_Base_Core::ActivePlanId& plan, const std::map<std::string, std::string>& mapKeyValues);
        TA_Base_Core::ActivePlanSharedData* getPlanSharedData(const TA_Base_Core::ActivePlanId& plan);

        TA_Base_Core::ActivePlanSharedDataSeq* getAllPlanSharedData();
        void updateAllPlanSharedData(const TA_Base_Core::ActivePlanSharedDataSeq& statusSeq);

        void broadcastAgentResetMessage();

    private:
        ActivePlanBulletinBoardImpl();
        ~ActivePlanBulletinBoardImpl();
        ActivePlanBulletinBoardImpl( const ActivePlanBulletinBoardImpl& );
        ActivePlanBulletinBoardImpl& operator=( const ActivePlanBulletinBoardImpl& );

		typedef std::map<SortablePlanId,PlanDetail> PlanMap;
		typedef std::map<SortablePlanId,PlanDetail>::iterator PlanMapIterator;
		typedef std::map<SortablePlanId,PlanDetail>::const_iterator ConstPlanMapIterator;

        void broadcastActivePlanDetailMessage( const TA_Base_Core::ActivePlanDetail& detail );
        void broadcastActiveStepDetailMessage( const TA_Base_Core::PlanExecuteStepUpdate& detail );
        void sendActivePlanUpdateMessage( const TA_Base_Core::ActivePlanDetail& detail );
        void sendActiveStepUpdateMessage( const TA_Base_Core::ActivePlanId& refPlan, const TA_Base_Core::ActiveStepDetail& detail );
		void sendCustomUpdateMessage( const TA_Base_Core::PlanCustomisationUpdate& updateData );
		void getInstances( const unsigned long plan, ConstPlanMapIterator& itLower, ConstPlanMapIterator& itUpper ) const;
        unsigned long getFirstAvailableInstanceNumber( const unsigned long plan );

    private:
        std::map<SortablePlanId,PlanDetail> m_planMap;

		mutable ACE_RW_Mutex m_planMapLock;
		mutable ACE_Mutex m_cullLock;

		TA_Base_Core::CommsMessageSender* m_commsSender;
		TA_Base_Core::StateUpdateMessageSender* m_stateSender;

		unsigned long m_updateId;
        unsigned long m_pasUpdateId;

		mutable ACE_Mutex m_updateIdLock;
		mutable ACE_Mutex m_pasUpdateIdLock;

		bool m_hangupMessageReceived;
		bool m_isControlMode;

		TA_Base_Bus::RightsLibrary* m_rightsLibrary;
        unsigned long m_resourceId;

        TA_Base_Bus::AuthenticationLibrary* m_authenticationLib;
	}; // ActivePlanBulletinBoardImpl
	typedef ACE_Singleton<ActivePlanBulletinBoardImpl, ACE_Recursive_Thread_Mutex> ActivePlanBulletinBoard;
} // TA_Base_App

#endif //ACTIVEPLANBULLETINBOARD_H_INCLUDED
