//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/plan_step_common/src/PlanStepCommonDef.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2014/06/03 09:20:18 $
// Last modified by:  $Author: huangjian $
//
// Definitions commonly used by various Plan Manager classes.

#if !defined(AFX_PLANSTEPCOMMONDEFS_H__9A958DFD_A9E3_46FC_8EF8_61E132D3C1D0__INCLUDED_)
#define AFX_PLANSTEPCOMMONDEFS_H__9A958DFD_A9E3_46FC_8EF8_61E132D3C1D0__INCLUDED_

#include <vector>

namespace TA_Base_App
{
    // Types
    typedef unsigned long PlanStepId;
	typedef unsigned long PlanStepNumber;
	typedef unsigned long TreeNodeId;

	const unsigned long MAX_MESSAGEBOX_STYLE = 6;

	#define WM_INTERACTIVE_STEP_EXECUTION_INTERRUPTED   (WM_USER + 650)
    #define WM_DISPLAY_STEP_PARAMETERS                  (WM_USER + 660)
}

#endif //AFX_PLANSTEPCOMMONDEFS_H__9A958DFD_A9E3_46FC_8EF8_61E132D3C1D0__INCLUDED_