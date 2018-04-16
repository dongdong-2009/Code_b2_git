//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/plan_step_common/src/PlanStepParameter.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2014/06/03 09:20:18 $
// Last modified by:  $Author: huangjian $
//
// <description>

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// PlanStepParameter

template<typename ObjectType>
PlanStepParameter<ObjectType>::PlanStepParameter( PlanStep& parent, const std::string& name, ObjectType& refSrc ) :
m_name( name.c_str() ), 
m_refSrcValue( refSrc ),
m_oNewValue( refSrc ),
m_parent( parent )
{
}

template<typename ObjectType>
PlanStepParameter<ObjectType>::~PlanStepParameter()
{
}

template<typename ObjectType>
const std::string &PlanStepParameter<ObjectType>::name() const
{
    return m_name;
}

template<typename ObjectType>
const ObjectType PlanStepParameter<ObjectType>::value() const
{
    return m_refSrcValue;
}

template<typename ObjectType>
void PlanStepParameter<ObjectType>::value( const ObjectType oValue )
{
    m_refSrcValue = oValue;
}

template<typename ObjectType>
void PlanStepParameter<ObjectType>::editParameter()
{
    std::auto_ptr<IPlanStepParameterForm> ptrForm(getEditDialog( true ));
    if ( NULL != ptrForm.get() )
    {
        ptrForm->present();
    }
}


template<typename ObjectType>
void PlanStepParameter<ObjectType>::viewParameter()
{
    std::auto_ptr<IPlanStepParameterForm> ptrForm(getEditDialog( false ));
    if ( NULL != ptrForm.get() )
    {
        ptrForm->present();
    }
}

template<typename ObjectType>
PlanStep* PlanStepParameter<ObjectType>::getParentStep() const
{ 
	return &m_parent;
}

}
