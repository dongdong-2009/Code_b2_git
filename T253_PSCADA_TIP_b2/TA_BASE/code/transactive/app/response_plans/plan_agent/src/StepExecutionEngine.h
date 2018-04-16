/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/StepExecutionEngine.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2012/12/27 15:59:31 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */

#if !defined(AFX_StepExecutionEngine_H__12F91821_8BB4_43EA_893E_E541DAED23FC__INCLUDED_)
#define AFX_StepExecutionEngine_H__12F91821_8BB4_43EA_893E_E541DAED23FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ace/Task.h"
#include "ace/Activation_Queue.h"
#include "app/response_plans/plan_agent/src/ActivePlanMessageQueue.h"

namespace TA_Base_App
{
    class ActiveStep;
    class ActivePlan;

	class StepExecutionEngine : public ACE_Task<ACE_MT_SYNCH>
	{
	public:
		StepExecutionEngine( ActivePlan& refTask );
		virtual ~StepExecutionEngine();

	public:
		virtual int open(void*);
		virtual int close(u_long flags = 0 );
		virtual int svc(void);

        void start( const int nCount );
        void stop();
        void parallelAll( const std::list<ActiveStep*>& lstNewSteps, const int nResuedStepCount );
        void parallelAll( const int nNewTaskCount );

	private:
		ACE_Activation_Queue m_oTaskQueue;
        ActivePlan& m_refTask;
        volatile bool m_bInSvc;
	};
}

#endif // !defined(AFX_StepExecutionEngine_H__12F91821_8BB4_43EA_893E_E541DAED23FC__INCLUDED_)
