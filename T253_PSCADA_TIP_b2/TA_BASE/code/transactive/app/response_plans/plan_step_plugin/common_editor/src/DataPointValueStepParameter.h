/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/DataPointValueStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  **/

#if !defined(AFX_DATAPOINTVALUESTEPPARAMETER_H__ACCA7354_7CD6_42F7_8D1B_CD536F3C9275__INCLUDED_)
#define AFX_DATAPOINTVALUESTEPPARAMETER_H__ACCA7354_7CD6_42F7_8D1B_CD536F3C9275__INCLUDED_

#include <string>
#include "app/response_plans/plan_step_plugin/common/src/DataPointEnum.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/DllExport.h"

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // DataPointValueStepParameter

    class PLAN_COMM_DLL DataPointValueStepParameter : public PlanStepParameter<std::string>
    {
    // Operations
    public:
		DataPointValueStepParameter(PlanStep& parent, const std::string& name, const int valueType, std::string& value);
        virtual ~DataPointValueStepParameter();
		virtual std::vector<std::string> getStepParameterValues();

        virtual const std::string displayValue() const;
		const int getDataPointValueType() const { return m_valueType; }
        void setDataPointValueType( const int valueType );

    protected:
        virtual IPlanStepParameterForm* getEditDialog( bool canEdit );

    private:
        DataPointValueStepParameter& operator=(const DataPointValueStepParameter& parameter);

    // Attributes
    private:
		int m_valueType;
    };
}

#endif // !defined(AFX_DATAPOINTVALUESTEPPARAMETER_H__ACCA7354_7CD6_42F7_8D1B_CD536F3C9275__INCLUDED_)
