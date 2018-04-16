//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/ExecutableRunPlanStep.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#if !defined(AFX_EXECUTABLERUNPLANSTEP_H__A83A1B95_8238_4E25_BF29_C954D9C787DD__INCLUDED_)
#define AFX_EXECUTABLERUNPLANSTEP_H__A83A1B95_8238_4E25_BF29_C954D9C787DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
 
#include "app/response_plans/plan_step_plugin/common_editor/src/InteractiveStep.h"

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // ExecutableRunPlanStep
	class CRunPlanDlg;

    class ExecutableRunPlanStep : public InteractiveStep
    {
    // Operations
    public:
		ExecutableRunPlanStep(const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
			const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters);
        virtual ~ExecutableRunPlanStep();

        //InteractiveStep interface
        virtual void launch();
		virtual void close();

	private:
		CRunPlanDlg* m_runPlanDlg;
    };

} // namespace TA_Base_App

#endif // !defined(AFX_EXECUTABLERUNPLANSTEP_H__A83A1B95_8238_4E25_BF29_C954D9C787DD__INCLUDED_)
