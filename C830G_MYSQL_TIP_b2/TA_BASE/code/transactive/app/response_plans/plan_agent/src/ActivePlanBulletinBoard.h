#ifndef ACTIVEPLANBULLETINBOARD_H_INCLUDED
#define ACTIVEPLANBULLETINBOARD_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #7 $
  *
  *  Last modification: $DateTime: 2011/06/20 20:46:25 $
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


namespace TA_Base_App
{
	// Class declaration
	// Note that a typedef following the class declaration declares the
	// type "ActivePlanBulletinBoard" and that "ActivePlanBulletinBoardImpl" can
	// not be used.

    class ActivePlanBulletinBoardImpl
	{
	// Public methods
	public:

		friend class ACE_Singleton<ActivePlanBulletinBoardImpl, ACE_Recursive_Thread_Mutex>;

		bool ActivePlanBulletinBoardImpl::isRegisteredManager (const std::string& session, const std::string pmId);
      void registerPlanManager(const std::string& session, const std::string& pmId, TA_Base_Bus::IPlanManagerCorbaDef_ptr planManagerRef);
		void deregisterPlanManager(const std::string& session, const std::string& pmId);

		bool isOwner(const std::string& session, const TA_Base_Core::ActivePlanId& plan);

		void setControlMode( bool controlMode );

        bool isControlMode(void);

        bool existsPlan(const TA_Base_Core::ActivePlanId plan);
		////////////////////////////////////////////////////////////////////
		//returns true for plans that come from GRC category
		//according to the Plan Management System Software Requirements spec
		//to differentiate between GRC and DSS plans is to check which category
		//a plan belongs to and if it contains any BRC steps
		//Hence, the method is implemented to reflect this requirement
		////////////////////////////////////////////////////////////////////////
		bool isGRCPlan (const TA_Base_Core::ActivePlanId plan);
		bool isManualGRC (const TA_Base_Core::ActivePlanId plan);
		bool isManualPlan (const TA_Base_Core::ActivePlanId plan);

		bool isActionPermitted(const std::string& session, unsigned long actionKey, std::string& reason);

		//zn++ CL-21150 add method to check concrete resource
		bool isActionPermitted(const std::string& session, unsigned long resouceKey, unsigned long actionKey, std::string& reason);
		//++zn

	// Read
		void getPlanManagerObjRef( const TA_Base_Core::ActivePlanId& plan, TA_Base_Bus::IPlanManagerCorbaDef_ptr& pmCorbaRef);
		
		std::string getSession( const TA_Base_Core::ActivePlanId& plan ) const;
		std::string getPlanManagerId( const TA_Base_Core::ActivePlanId& plan ) const;
		std::string getOwner( const std::string& idString );
		unsigned long getOwnerId( const std::string& idString );
        std::string getOperatorName( const TA_Base_Core::ActivePlanId& plan );
        bool getUserKeyFromSession( const std::string& session, unsigned long& userKey );
	    unsigned long getUserKey( const TA_Base_Core::ActivePlanId& plan );
        
		bool isAllPlanStopped() const;
		bool isActive( const TA_Base_Core::NodeId& plan,  const TA_Base_Core::ActivePlanId& ignoreInstance ) const;
		bool isActive( const TA_Base_Core::ActivePlanId& plan ) const;
		TA_Base_Core::ExecutionHistory* getPlanExecutionHistory(const TA_Base_Core::ActivePlanId& plan, const bool bSendIdUpdate = true);
		TA_Base_Core::CustomisationHistory* getPlanCustomisationHistory(const TA_Base_Core::ActivePlanId& plan, const bool bSendIdUpdate = true);
		ActivePlanList* getActiveInstances( const TA_Base_Core::NodeId& plan ) const;
		TA_Base_Core::ActivePlanDetails* getActivePlanDetail( bool stateUpdate = false ) const;
		TA_Base_Core::ActivePlanDetails* getActivePlanDetail( TA_Base_Core::NodeId plan ) const;
		TA_Base_Core::ActivePlanDetail*  getActivePlanDetail( TA_Base_Core::ActivePlanId plan ) const;
		TA_Base_Core::ActivePlanDetail* getAssocAlarmActivePlanDetail (const char* alarmId);
		TA_Base_Core::FailedStepDetails getFailedSteps( TA_Base_Core::ActivePlanId plan ); //DP-changes		
		void setAssociatedAlarmId (const TA_Base_Core::ActivePlanId& plan, const char* alarmId);
		TA_Base_Core::EActivePlanState getState( const TA_Base_Core::ActivePlanId& plan ) const;
		ACE_Future<TA_Base_Core::EActivePlanState> getFuture( const TA_Base_Core::ActivePlanId& plan ) const;
		EWaitResult waitForOperatorResponse ( const TA_Base_Core::ActivePlanId& plan, ACE_Time_Value& tv ) const;
		EWaitResult waitForCompletion ( const TA_Base_Core::ActivePlanId& plan, ACE_Time_Value& tv ) const;
		EWaitResult waitForTermination( TA_Base_Core::NodeId plan             , ACE_Time_Value& tv ) const;
	// Write
		bool reserveInstance( const std::string& session,
                              const std::string& pmId,
							  TA_Base_Core::ActivePlanId& plan,							  
							  TA_Base_Core::EActivePlanState initialState,
							  const ACE_Future<TA_Base_Core::EActivePlanState>& planResult, 
							  bool manExecuted);
		bool reserveInstance( const std::string& session,
                              const std::string& pmId,
							  const TA_Base_Core::ActivePlanId& parentPlan,							  
							  TA_Base_Core::ActivePlanId& childPlan,							  
							  TA_Base_Core::EActivePlanState initialState,
							  const ACE_Future<TA_Base_Core::EActivePlanState>& planResult );
        void undoReserveInstance( TA_Base_Core::ActivePlanId plan );
		void releaseInstance( const TA_Base_Core::ActivePlanId& plan );
		void update( const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::StepNumber step, const std::string& stepName, const std::string remark );
		void update( const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EActivePlanState state, const std::string remark );
		void update( const TA_Base_Core::ActivePlanId& plan, const std::string& session, const std::string& pmId );
		void update( const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::StepNumber step, TA_Base_Core::EActiveStepState state, const std::string remark );
        void update( const TA_Base_Core::ActivePlanId& plan, const TA_Base_Core::StepCustomisableDetail& detail );
        void updateStepStartTime( const TA_Base_Core::ActivePlanId& plan, const ACE_Time_Value& stepStartTime );
        void updateActivePlanDetail( const TA_Base_Core::ActivePlanDetail& refDetail );
		void sendStateUpdateRequestToControlAgent( void );
		void reassignInstance ( const std::string& session, TA_Base_Core::ActivePlanId& plan);

        void processExecutionHistoryUpdate( TA_Base_Core::ExecutionHistoryUpdate& historyUpdate );
        void processCustomisationHistoryUpdate( const TA_Base_Core::CustomisationHistoryUpdate& historyUpdate );

        void clearAllPlanStates();
		void cullStoppedPlans( void );
        void cullAllPlans( void );
		void removeExpiredActivePlans(void);
			void ActivePlanBulletinBoardImpl::pauseExpiredInteractivePlans (void);
		void deleteMessageSenders( void );
		//as per TD 11324
		void deleteRightsLib (void);
        
		void setSession( const TA_Base_Core::ActivePlanId& plan, const std::string& session );
        void setPlanManagerId( const TA_Base_Core::ActivePlanId& plan, const std::string& pmId );

		bool wasHangupMessageReceived(void);
		void setHangupMessageReceived(void);

		void sendConfigUpdateMessage( const TA_Base_Core::PlanConfigUpdateData& configData );

        void sendUpdateMessage(const TA_Base_Core::ActivePlanId plan);

		void sendStateUpdateMessage( TA_Base_Core::PASUpdateData& stateUpdateData );
		void sendStateUpdateMessage( const TA_Base_Core::ActivePlanDetail& planDetail );
        void sendStateUpdateMessage( const TA_Base_Core::UpdateId updateId );
        void sendAllStatesUpdateMessage( TA_Base_Core::PASUpdateDataSeq& refStateUpdateDataSeq );
        void sendOperatorPausedStateUpdate( const TA_Base_Core::ActivePlanId plan );
		void sendAllStateUpdatesToMonitorAgent(void);

		TA_Base_Core::UpdateId getNextUpdateId(void);
        TA_Base_Core::UpdateId getNextPasUpdateId(void);
        void setPasUpdateId(TA_Base_Core::UpdateId updateId);
        void setUpdateId(TA_Base_Core::UpdateId updateId);

        void cullStoppedPlan(TA_Base_Core::ActivePlanId plan);
	
	private:

		typedef std::map<TA_Base_Core::StepNumber, TA_Base_Core::ActiveStepDetail> ExeHistoryMap;
		typedef std::map<TA_Base_Core::StepNumber, TA_Base_Core::ActiveStepDetail>::iterator ExeHistoryMapIterator;
		typedef std::map<TA_Base_Core::StepNumber, TA_Base_Core::ActiveStepDetail>::const_iterator ConstExeHistoryMapIterator;

		typedef std::map<TA_Base_Core::StepNumber, TA_Base_Core::StepCustomisableDetail> CustHistoryMap;
		typedef std::map<TA_Base_Core::StepNumber, TA_Base_Core::StepCustomisableDetail>::iterator CustHistoryMapIterator;
		typedef std::map<TA_Base_Core::StepNumber, TA_Base_Core::StepCustomisableDetail>::const_iterator ConstCustHistoryMapIterator;
		
		ACE_Time_Value getStepTimeout (const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::StepNumber step);
        
		class PlanDetail
		{
		public:
			
			
			PlanDetail( const PlanDetail& );
			PlanDetail( const std::string& session,
				const std::string& pmId,
				const unsigned long userKey,
				const TA_Base_Core::UpdateId updateId,
				const std::string& owner,
				const TA_Base_Core::NodeId& planId,
				const TA_Base_Core::EActivePlanState& initialState,
				const std::string& remark,
				const ACE_Future<TA_Base_Core::EActivePlanState>& result , 
				const bool manExecuted, 
				const std::string& assocAlarmId);
			
			PlanDetail( const TA_Base_Core::ActivePlanDetail& detail );
			
			void set( const TA_Base_Core::ActivePlanId& plan );
			bool set( TA_Base_Core::EActivePlanState state );
			
			const TA_Base_Core::ActivePlanDetail& getDetail();
			
			TA_Base_Core::ActivePlanDetail m_detail;
			
			//the following are to help rectify TD 7183
			//total time-out for all steps in an interactive plan
			//to pause a plan in case connection is lost
			ACE_Time_Value m_timeOut;
			//time a step was started
			ACE_Time_Value m_stepStarted;
			//note m_timeOut and m_stepStarted are reset 
			//for each step in a plan

			std::string m_session;
			std::string m_pmId;
			unsigned long m_userKey;

			ExeHistoryMap m_exeHistoryMap;
			CustHistoryMap m_custHistoryMap;

			ACE_Time_Value  m_lastModified;
			ACE_Future<TA_Base_Core::EActivePlanState> m_result;
			
			bool isNonActive( void ) const;
			bool isActive ( void ) const;
			
			//alarm ID of the alarm
			//raised by this Plan
			std::string m_alarmID;

		private:
			PlanDetail();
			PlanDetail& operator=( const PlanDetail& );

			static std::string getGmtTime( void );
			static std::string getLocalTime( void );
        };

		typedef std::map<SortablePlanId,PlanDetail> PlanMap;
		typedef std::map<SortablePlanId,PlanDetail>::iterator PlanMapIterator;
		typedef std::map<SortablePlanId,PlanDetail>::const_iterator ConstPlanMapIterator;

		ActivePlanBulletinBoardImpl();
		~ActivePlanBulletinBoardImpl();
		ActivePlanBulletinBoardImpl( const ActivePlanBulletinBoardImpl& );
		ActivePlanBulletinBoardImpl& operator=( const ActivePlanBulletinBoardImpl& );

		void sendUpdateMessage( const PlanDetail& detail );
		void sendCustomUpdateMessage( const TA_Base_Core::PlanCustomisationUpdate& updateData );
		void getInstances( const TA_Base_Core::NodeId& plan, ConstPlanMapIterator& itLower, ConstPlanMapIterator& itUpper ) const;
		unsigned long getFirstAvailableInstanceNumber(const TA_Base_Core::NodeId& plan);

		std::map<SortablePlanId,PlanDetail> m_planMap;
		
		//CL-21338 PlanAgent wrong session and high CPU usage in monitor mode 
		// add for the error session info in monitor mode
		// @parameter [in] strCompositePmId format is: 51a7f5cd0153704b3b095a01ec524f9408ec297bc45f-fdd8-46bb-8c31-f5f88226192f
		// @parameter [out] strSessionID should be:    51a7f5cd0153704b3b095a01ec524f9408ec
		// @parameter [out] strPmID should be:                                             297bc45f-fdd8-46bb-8c31-f5f88226192f
		bool getSessionIDAndPmID(const std::string& strCompositePmId, std::string& strSessionID, std::string& strPmID);
		static const int INDICATION_SESSION_AND_PMID;

		//marvin++
		//TD13527
		//<entity key,planid> map
		typedef std::multimap<unsigned long, unsigned long> EntityPlanRestrictionMultiMap;
		typedef EntityPlanRestrictionMultiMap::iterator EntityPlanRestrictionMultiMapIterator;
		typedef EntityPlanRestrictionMultiMap::value_type EntityPlanRestrictionMultiMapValue;
		EntityPlanRestrictionMultiMap m_entityPlanRestrictionMultiMap;
		mutable ACE_RW_Mutex m_entityPlanRestrictionMultiMapGuard;

		void deleteEntityPlanMapRestrictionIfApplicable(const TA_Base_Core::ActivePlanId& plan,TA_Base_Core::EActivePlanState state);
		EntityPlanRestrictionMultiMapIterator findEntityPlanFromMultimap(const TA_Base_Core::ActivePlanId& plan);
		//marvin++
		//TD13527

		typedef std::map<std::string, TA_Base_Bus::IPlanManagerCorbaDef_ptr> PMOBJREFMap;
		typedef PMOBJREFMap::iterator PMOBJREFMapIterator;
		PMOBJREFMap	m_pmObjRef;
		mutable ACE_RW_Mutex m_pmObjMapLock;


		mutable ACE_RW_Mutex m_planMapLock;
		mutable ACE_Mutex m_cullLock;

		TA_Base_Core::CommsMessageSender* m_commsSender;
		TA_Base_Core::StateUpdateMessageSender* m_stateSender;

		TA_Base_Core::UpdateId m_updateId;
        TA_Base_Core::UpdateId m_pasUpdateId;

		mutable ACE_Mutex m_updateIdLock;
		mutable ACE_Mutex m_pasUpdateIdLock;

        static const long MAX_PLAN_AGE_IN_SECS;

		bool m_hangupMessageReceived;
		bool m_isControlMode;

		TA_Base_Bus::RightsLibrary* m_rightsLibrary;
        unsigned long m_resourceId;

        TA_Base_Bus::AuthenticationLibrary m_authenticationLib;
	
	}; // ActivePlanBulletinBoardImpl
	typedef ACE_Singleton<ActivePlanBulletinBoardImpl, ACE_Recursive_Thread_Mutex> ActivePlanBulletinBoard;
} // TA_Base_App

#endif //ACTIVEPLANBULLETINBOARD_H_INCLUDED
