/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/plan_step_common/src/PlanStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2014/06/03 09:20:18 $
  * Last modified by:  $Author: huangjian $
  *
  * <description>
  *
  **/

#if !defined(AFX_PLANSTEPPARAMETER_H__6E86DB81_546B_4AEF_8498_D480C3E5E5BF__INCLUDED_)
#define AFX_PLANSTEPPARAMETER_H__6E86DB81_546B_4AEF_8498_D480C3E5E5BF__INCLUDED_

#include <vector>
#include <string>
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanStepParameterForm.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanStepParameter.h"

namespace TA_Base_App
{
	class PlanStep;
    
  /////////////////////////////////////////////////////////////////////////////
    // PlanStepParameter
    template<typename ObjectType>
    class PlanStepParameter : public IPlanStepParameter
    {
    // Operations
    public:
        PlanStepParameter( PlanStep& parent, const std::string& name, ObjectType& refSrc );
        virtual ~PlanStepParameter();

		virtual std::vector<std::string> getStepParameterValues() = 0;

        virtual void editParameter();
        virtual void viewParameter();

        virtual const std::string& name() const;

        const ObjectType value() const;
        void value( const ObjectType oValue );

        PlanStep* getParentStep() const;

    protected:
        virtual IPlanStepParameterForm* getEditDialog( bool canEdit ) = 0;

    private:
        PlanStepParameter(const PlanStepParameter &other);
        PlanStepParameter &operator=(const PlanStepParameter &other);

    // Attributes
    protected:
        std::string m_name;
        ObjectType& m_refSrcValue;
        ObjectType m_oNewValue;
		PlanStep& m_parent;
    };
}

#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameter.inl"

#endif // !defined(AFX_PLANSTEPPARAMETER_H__6E86DB81_546B_4AEF_8498_D480C3E5E5BF__INCLUDED_)
