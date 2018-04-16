/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/TimeSpanStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Represents a generic time span parameter (e.g. as used by TIS and PA steps for message duration).
  * The supported time resolutions are: days, hours, minutes, seconds.
  *
  **/

#if !defined(AFX_TIMESPANSTEPPARAMETER_H__92240FA4_FBDF_4378_86C2_3E46C2251217__INCLUDED_)
#define AFX_TIMESPANSTEPPARAMETER_H__92240FA4_FBDF_4378_86C2_3E46C2251217__INCLUDED_

#include "app/response_plans/plan_step_plugin/common_editor/src/NumericStepParameter.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // TimeSpanStepParameter

    class PLAN_COMM_DLL TimeSpanStepParameter : public NumericStepParameter
    {
    // Types
    public:
        typedef enum
                {
                    TR_SECONDS,
                    TR_MINUTES,
                    TR_HOURS,
                    TR_DAYS,
                    TR_UNDEFINED
                }
                ETimeResolution;

    // Operations
    public:
        TimeSpanStepParameter(PlanStep& parent, const std::string& name, unsigned long& number, ETimeResolution resolution, bool isHidden = false);

        virtual ~TimeSpanStepParameter();

		virtual std::vector<std::string> getStepParameterValues();

    private:
        TimeSpanStepParameter& operator=(const TimeSpanStepParameter& parameter);

    // Attributes
    private:
        ETimeResolution m_resolution;
    };
}

#endif // !defined(AFX_TIMESPANSTEPPARAMETER_H__92240FA4_FBDF_4378_86C2_3E46C2251217__INCLUDED_)

