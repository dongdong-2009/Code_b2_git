/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/DataPointStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  **/

#if !defined(AFX_DATAPOINTSTEPPARAMETER_H__E0865C1C_94EC_4AEF_813B_3749443C60D6__INCLUDED_)
#define AFX_DATAPOINTSTEPPARAMETER_H__E0865C1C_94EC_4AEF_813B_3749443C60D6__INCLUDED_

#include <string>
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/DllExport.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // DataPointStepParameter

    class PLAN_COMM_DLL DataPointStepParameter : public PlanStepParameter<std::string>
    {
    // Operations
    public:
        DataPointStepParameter(PlanStep& parent, const std::string& name, std::string& dataPoint);
        virtual ~DataPointStepParameter();

        //virtual void addToParameterList(CPlanStepParameterListCtrl& listCtrl);
        virtual std::vector<std::string> getStepParameterValues();

        virtual const std::string displayValue() const;

    protected:
        virtual IPlanStepParameterForm* getEditDialog( bool canEdit );

    private:
        DataPointStepParameter& operator=(const DataPointStepParameter& parameter);
    };
}

#endif // !defined(AFX_DATAPOINTSTEPPARAMETER_H__E0865C1C_94EC_4AEF_813B_3749443C60D6__INCLUDED_)
