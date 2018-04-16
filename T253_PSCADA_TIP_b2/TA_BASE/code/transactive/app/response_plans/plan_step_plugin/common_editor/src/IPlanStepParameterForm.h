//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepParameterForm.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#ifndef IPLANSTEPPARAMETERFORM_H_INCLUDED_
#define IPLANSTEPPARAMETERFORM_H_INCLUDED_

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // PlanStepParameterForm

	class IPlanStepParameterForm
    {
    // Operations
    public:
        virtual ~IPlanStepParameterForm() {};
        virtual void present() = 0;
        virtual void close() = 0;
    };

} // namespace TA_Base_App

#endif // !defined(IPLANSTEPPARAMETERFORM_H_INCLUDED_)
