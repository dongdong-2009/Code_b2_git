#ifndef ACTIVEPLAN_H_INCLUDED
#define ACTIVEPLAN_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlan.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #12 $
  *
  *  Last modification: $DateTime: 2015/06/03 11:12:40 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */

#include <list>
#include <map>
#include "ace/RW_Mutex.h"

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "app/response_plans/plan_agent/src/StepExecutionEngine.h"

namespace TA_Base_App
{
    class ActiveStep;

    typedef std::list<ActiveStep*> ActiveStepList;
    typedef ActiveStepList::iterator ActiveStepListIt;
    typedef std::map<unsigned long, ActiveStep*> NumToActiveStepMap;
    typedef NumToActiveStepMap::iterator NumToActiveStepMapIt;
    typedef std::multimap<unsigned long, ActiveStep*> NumToActiveStepMultiMap;
    typedef NumToActiveStepMultiMap::iterator NumToActiveStepMultiMapIt;
    typedef std::map<std::string, std::string> NameToValueMap;
    typedef std::map<std::string, std::string>::iterator NameToValueMapIt;
	typedef std::vector<std::string> StringVector;
    typedef std::map<unsigned long, StringVector> StepToDataMap;
    typedef std::map<unsigned long, StringVector>::iterator StepToDataMapIt;

	//Class declaration
	class ActivePlan : public IActivePlan
	{
	// Public methods
	public:

        ActivePlan( const TA_Base_Core::ActivePlanId& plan, const bool bPending, const bool bStateSyncDone );
		virtual ~ActivePlan();

		bool getIsActive();
		virtual void setIsActive( bool isActive );

        virtual bool stepCompleted( const unsigned long ulStepNum );
        virtual void planSharedData( const std::string& strName, const std::string& strValue );
        virtual void planSharedData( const std::map<std::string, std::string>& mapData );
        virtual const std::map<std::string, std::string> planSharedData();
        virtual const std::string planSharedData( const std::string& strName );
        virtual void stepResponsed( const unsigned long ulStepNum, const int nResponse );
        virtual const int stepResponsed( const unsigned long ulStepNum, const bool bErase = true );
        virtual void stepData( const unsigned long ulStepNum, const std::vector<std::string>& strData );
        virtual std::vector<std::string> stepData( const unsigned long ulStepNum, const bool bErase = true );
        virtual const TA_Base_Core::ActivePlanId& id() const;
        virtual const std::string name() const;
        virtual const std::string session() const;
        virtual const unsigned int userLocation() const;
        virtual const unsigned int userWorkstation() const;

		int execute();
        int runStep();
        void executeStep( ActiveStep& refStep );
        void stepCompleted();
        void stepStateUpdated( const unsigned long ulStepNum, const int nState );

        void stop();
		bool pause();
        void complete();
		void resume();
		void reassign(const std::string& sessionId);
		void unassign( void );
        bool customiseStep( const TA_Base_Core::StepCustomisableDetail& newDetail );
		void customiseSteps( const TA_Base_Core::StepCustomisableDetails& newDetails );

        bool anyPlanStepFailed();
        bool isPausePending();
        bool allStepPaused( const bool bIgnoreParallels );
        void planPaused();

        void setStateSyncDone( bool bSyncDone = true );

	private:
        ActivePlan& operator=( const ActivePlan& );
		ActivePlan( const ActivePlan& );
		std::string dumpContext( void );

    private:
        TA_Base_Core::ActivePlanId m_plan;
        StepExecutionEngine m_engStepExecution;
        ActiveStepList m_lstCurSteps;
        NumToActiveStepMultiMap m_mapExecutingSteps;
        NumToActiveStepMap m_mapAllSteps;
        ACE_RW_Mutex m_mtxCurStepsLock;
        ACE_RW_Mutex m_mtxAllStepsLock;

        std::string m_strSession;
        std::string m_strRemark;
        ACE_RW_Mutex m_mtxSharedDataLock;
        NameToValueMap m_mapSharedData;
        ACE_RW_Mutex m_mtxStepDataLock;
        StepToDataMap m_mapStepData;

        bool m_isActive;
        bool m_bPaused;
        bool m_bPending;
        bool m_bStateSyncDone;
	}; // ActivePlan
}; // TA_Base_App



#endif // ACTIVEPLAN_H_INCLUDED

