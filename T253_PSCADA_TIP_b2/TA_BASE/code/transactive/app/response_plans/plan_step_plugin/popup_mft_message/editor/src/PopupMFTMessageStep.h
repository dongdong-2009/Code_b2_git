#if !defined(AFX_DECISIONSTEP_H__37441840_28F6_42a1_B421_3680B445EAFA__INCLUDED_)
#define AFX_DECISIONSTEP_H__37441840_28F6_42a1_B421_3680B445EAFA__INCLUDED_
//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PopupMFTMessageStep.h $
// @author:  Andy Parker
// @version: $Revision: #3 $
//
// Last modification: $DateTime: 2014/07/10 16:23:57 $
// Last modified by:  $Author: qi.huang $
//
// <description>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_step_plugin/popup_mft_message/common/PopupMftMessageStepParameter.h"
#include "app/response_plans/plan_step_plugin/popup_mft_message/editor/src/PopupMftMessageEditorParameter.h"

namespace TA_Base_App
{
/////////////////////////////////////////////////////////////////////////////
// PopupMFTMessageStep

class PopupMFTMessageStep : public PlanStep
{
    // Operations
public:
    PopupMFTMessageStep( IPlanNode* parent, const BasicParameter& stepDetail );
    PopupMFTMessageStep( IPlanNode* parent, const unsigned long stepId, const int stepType ); // For new plan steps
    virtual ~PopupMFTMessageStep();

    virtual PlanStep* clone();
    virtual bool hasCustomOutgoingStep() const;
    virtual bool canIgnoreFailure() const;
    virtual std::string getStepTypeName() const;
    virtual void outgoingStepChanged( const unsigned int unPrevOutgoing, const unsigned int unNewOutgoing );
	virtual bool retrieveOutgoings( std::vector<unsigned int>& vecOutgoings );
    virtual void clearFlowParameters();
    virtual bool allowMultipleIncoming();
    virtual bool allowMultipleOutgoing();
    virtual IActivityParameter& getActivityParameter();
protected:
    PopupMFTMessageStep( const PopupMFTMessageStep& other );
    virtual void createPlanStepParameters();
    virtual bool isParametersValid( const std::vector<std::string>& parameters );
    virtual CPlanStepEditParameterPage* getParameterEditPage();

private:
    PopupMFTMessageStep& operator=( const PopupMFTMessageStep& other );


    // Attributes
private:
    PopupMftMessageStepParameter m_params;
    PopupMftMessageStepEditorParameter m_parActivityParameter;
};
}

#endif // defined (AFX_DECISIONSTEP_H__37441840_28F6_42a1_B421_3680B445EAFA__INCLUDED_)
