//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/ApprovedPlanTreeView.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#ifndef AFX_CONTROLPLANTHREAD_H__INCLUDED_
#define AFX_CONTROLPLANTHREAD_H__INCLUDED_

#include "core/threads/src/Thread.h"

namespace TA_Base_App
{
    class PlanNode;

	// CL16116++
    /////////////////////////////////////////////////////////////////////////////
    // ControlPlanThread, create thread to handle control Plan dialog
	class ControlPlanThread : public TA_Base_Core::Thread
	{
	public:
        ControlPlanThread(PlanNode* pNode, unsigned long location, unsigned long instanceId = -1, const std::string& planName = "", bool exec=false);
        virtual ~ControlPlanThread();

        virtual void run();
        virtual void terminate();

	private:
        ControlPlanThread();

    private:
        PlanNode* m_pPlanNode;	// PlanNode pointer
        std::string m_planName;	// for audit message use
		unsigned long m_Location;		// Plan Manager Location
        unsigned long	m_InstanceId;	// Plan Instance Id
		bool m_bStop;
        bool m_execPlan;
	};
	// ++CL16116

} // namespace TA_Base_App

#endif // !defined(AFX_CONTROLPLANTHREAD_H__INCLUDED_)
