#ifndef RUNPLANMETHODOBJECT_H_INCLUDED
#define RUNPLANMETHODOBJECT_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/RunPlanMethodObject.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #4 $
  *
  *  Last modification: $DateTime: 2014/11/04 13:46:22 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */

#include <string>

#include "ace/Method_Request.h"

namespace TA_Base_Core
{
    struct ActivePlanId;
}

namespace TA_Base_App
{
	class PlanExecutionEngine;
    class ActivePlan;

	//Class declaration
	class RunPlanMethodObject : public ACE_Method_Request
	{
	// Public methods
	public:
        RunPlanMethodObject( PlanExecutionEngine& task, std::auto_ptr<ActivePlan> activePlan );
		RunPlanMethodObject( PlanExecutionEngine& task, const std::string& strSession, const TA_Base_Core::ActivePlanId& refActiveId, 
            const bool bPending, const bool bManualStart, const bool bStartFromMonitorMode );
		virtual ~RunPlanMethodObject();

        void signalRunning();

		virtual int call();

	private:

		RunPlanMethodObject& operator=( const RunPlanMethodObject& );
		RunPlanMethodObject( const RunPlanMethodObject& );

		PlanExecutionEngine& m_engine;
		std::auto_ptr<ActivePlan> m_activePlan;
        std::string m_strSession;
        unsigned long m_ulPlanId;
        unsigned long m_ulInstance;
        unsigned long m_ulLocation;
        bool m_bPending;
        bool m_bMaualStart;
        bool m_bStartFromMonitorMode;
	}; // RunPlanMethodObject
} // TA_Base_App

#endif //RUNPLANMETHODOBJECT_H_INCLUDED
