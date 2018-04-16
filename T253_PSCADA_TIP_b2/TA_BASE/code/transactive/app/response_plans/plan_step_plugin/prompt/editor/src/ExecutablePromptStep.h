//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/ExecutablePromptStep.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#if !defined(AFX_EXECUTABLEPROMPTSTEP_H__20522C28_336C_4624_9E69_0BA656E5A139__INCLUDED_)
#define AFX_EXECUTABLEPROMPTSTEP_H__20522C28_336C_4624_9E69_0BA656E5A139__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/response_plans/plan_step_plugin/common_editor/src/InteractiveStep.h"

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // ExecutablePromptStep
	class CPromptDlg;

    class ExecutablePromptStep : public InteractiveStep
    {
    // Operations
    public:
		ExecutablePromptStep(const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
			const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters);
        virtual ~ExecutablePromptStep();

        //InteractiveStep interface
        virtual void launch();
		virtual void close();

	private:
		CPromptDlg* m_promptDlg;
    };

} // namespace TA_Base_App
#endif // !defined(AFX_EXECUTABLEPROMPTSTEP_H__20522C28_336C_4624_9E69_0BA656E5A139__INCLUDED_)
