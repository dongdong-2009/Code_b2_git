//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/InteractiveStep.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#if !defined(AFX_INTERACTIVESTEP_H__711C3829_932C_4693_B233_9AF5C6DA25DD__INCLUDED_)
#define AFX_INTERACTIVESTEP_H__711C3829_932C_4693_B233_9AF5C6DA25DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ActivePlanStep.h"

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // InteractiveStep

	class InteractiveStep : public ActivePlanStep
    {
    // Operations
    public:
		InteractiveStep(const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
			const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters);
        virtual ~InteractiveStep();

        virtual void execute();
        virtual void stop(); 

    protected:
        virtual void launch() = 0;
		virtual void close() = 0;
    };

} // namespace TA_Base_App

#endif // !defined(AFX_INTERACTIVESTEP_H__711C3829_932C_4693_B233_9AF5C6DA25DD__INCLUDED_)
