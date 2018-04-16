/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/ExecutableLaunchSchematicStep.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Facilitates the launching of a display when executing a Launch Schematic step.
  *
  */

#if !defined(AFX_EXECUTABLELaunchSchematicSTEP_H__1958B41C_198B_4BEC_97DF_BBDC94A86BC6__INCLUDED_)
#define AFX_EXECUTABLELaunchSchematicSTEP_H__1958B41C_198B_4BEC_97DF_BBDC94A86BC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/response_plans/plan_step_plugin/common_editor/src/InteractiveStep.h"

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // ExecutableLaunchSchematicStep
	class CLaunchSchematicDlg;

    class ExecutableLaunchSchematicStep : public InteractiveStep
    {
    // Operations
    public:
		ExecutableLaunchSchematicStep(const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
			const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters);
        virtual ~ExecutableLaunchSchematicStep();

        //InteractiveStep interface
        virtual void launch();
		virtual void close();

	private:
		CLaunchSchematicDlg* m_launchSchematicDlg;
    };

} // namespace TA_Base_App
#endif // !defined(AFX_EXECUTABLELaunchSchematicSTEP_H__1958B41C_198B_4BEC_97DF_BBDC94A86BC6__INCLUDED_)
