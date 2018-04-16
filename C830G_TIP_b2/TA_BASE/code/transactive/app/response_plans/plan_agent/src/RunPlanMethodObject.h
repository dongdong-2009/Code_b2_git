#ifndef RUNPLANMETHODOBJECT_H_INCLUDED
#define RUNPLANMETHODOBJECT_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  <description>
  */

namespace TA_Base_Core
{
    struct ActivePlanId;
}

namespace TA_Base_App
{
	class PlanExecutionEngine;

	//Class declaration
	//class RunPlanMethodObject : public ACE_Method_Object
	class RunPlanMethodObject : public ACE_Method_Request
	{
	// Public methods
	public:
        RunPlanMethodObject( PlanExecutionEngine& task, std::auto_ptr<ActivePlan> activePlan );
		RunPlanMethodObject( PlanExecutionEngine& task, const std::string& strSession, 
            const std::string& strPmId,  const TA_Base_Core::ActivePlanId& refActiveId, 
            const TA_Base_Core::EActivePlanState& eInitialState, const bool bManualStart, 
            const bool bStartFromMonitorMode );
		virtual ~RunPlanMethodObject();

        void signalRunning();

		virtual int call();

	private:

		RunPlanMethodObject& operator=( const RunPlanMethodObject& );
		RunPlanMethodObject( const RunPlanMethodObject& );

		PlanExecutionEngine& m_engine;
		std::auto_ptr<ActivePlan> m_activePlan;
        std::string m_strSession;
        std::string m_strPmId;
        const TA_Base_Core::ActivePlanId m_activePlanId;
        TA_Base_Core::EActivePlanState m_eInitialState;
        bool m_bMaualStart;
        bool m_bStartFromMonitorMode;
	}; // RunPlanMethodObject
} // TA_Base_App

#endif //RUNPLANMETHODOBJECT_H_INCLUDED
