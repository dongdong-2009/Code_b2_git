//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepParameterStepNumberEditDlg.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#include "StdAfx.h"

#include <vector>
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DataConversion.h"
#include "StepNumberStepParameter.h"
#include "PlanStep.h"
#include "IPlanNode.h"
#include "Resource.h"
#include "PlanStepParameterStepNumberEditDlg.h"


namespace TA_Base_App
{

const int PSIT_UNEXECUTED_STEP = 5;
/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterStepNumberEditDlg dialog

CPlanStepParameterStepNumberEditDlg::CPlanStepParameterStepNumberEditDlg(StepNumberStepParameter& parameter,bool canEdit)
: CPlanStepParameterEditDlg( parameter, IDD_PLAN_STEP_PARAMETER_STEP_NUMBER_EDIT_DLG, canEdit )

{
    FUNCTION_ENTRY( "CPlanStepParameterStepNumberEditDlg" );

    // {{AFX_DATA_INIT(CPlanStepParameterStepNumberEditDlg)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void CPlanStepParameterStepNumberEditDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepParameterEditDlg::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepParameterStepNumberEditDlg)
    DDX_Control(pDX, IDC_STEP_LIST, m_stepList);//--lixiaoxiaTODO
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepParameterStepNumberEditDlg, CPlanStepParameterEditDlg)
    // {{AFX_MSG_MAP(CPlanStepParameterStepNumberEditDlg)
    ON_BN_CLICKED(IDOK, onOK)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterStepNumberEditDlg message handlers

BOOL CPlanStepParameterStepNumberEditDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CPlanStepParameterEditDlg::OnInitDialog();

	m_stepList.InsertColumn(0, _T("Position"), LVCFMT_LEFT, 100);
	m_stepList.InsertColumn(1, _T("Name"), LVCFMT_LEFT, 150);
	m_stepList.InsertColumn(2, _T("Type"), LVCFMT_LEFT, 150);
	m_stepList.InsertColumn(3, _T("Description"), LVCFMT_LEFT, 200);

	m_stepList.SetExtendedStyle(m_stepList.GetExtendedStyle()|LVS_EX_FULLROWSELECT);
    StepNumberStepParameter* planStepParameter = dynamic_cast<StepNumberStepParameter*>(GetStepParameter());
    IPlanNode* plan = planStepParameter->getParentStep()->getParentPlan( );

    // Load all the steps for the selected plan
    //plan->addStepsToList(m_stepList);
	std::vector<PlanStep*> planStepVect = plan->getPlanSteps();
	for (std::vector<PlanStep*>::iterator itPlanStep = planStepVect.begin(); itPlanStep != planStepVect.end(); ++itPlanStep)
	{
		int item = m_stepList.InsertItem(m_stepList.GetItemCount(), _T(""), PSIT_UNEXECUTED_STEP);
	
		m_stepList.SetItemText(item, 0, _T(TA_Base_Core::DataConversion::toString((*itPlanStep)->getStepPosition())).c_str());
		m_stepList.SetItemText(item, 1, _T((*itPlanStep)->getStepName().c_str()));
		m_stepList.SetItemText(item, 2, _T((*itPlanStep)->getStepTypeName().c_str()));
		m_stepList.SetItemText(item, 3, _T((*itPlanStep)->getStepDescription().c_str()));
	
		m_stepList.SetItemData(item, reinterpret_cast<DWORD>(*itPlanStep));
	}

    // Select the current step
    StepNumberStepParameter* planStepNumberStepParameter = dynamic_cast<StepNumberStepParameter*>(GetStepParameter());
    TA_ASSERT(planStepNumberStepParameter!=NULL,"Not a StepNumberStepParameter");
    unsigned long planStepId = planStepNumberStepParameter->value();
    PlanStep* planStep = plan->getPlanStepByStepId(planStepId);

    SetSelectedStep(planStep, true);

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CPlanStepParameterStepNumberEditDlg::onOK()
{
    FUNCTION_ENTRY( "onOK" );

    // Get the selected step from the list control
	PlanStep *planStep = NULL;

	int selectedItem = m_stepList.GetNextItem(-1, LVNI_SELECTED);
	if (selectedItem != -1)
	{
		planStep = reinterpret_cast<PlanStep*>(m_stepList.GetItemData(selectedItem)); 
	}

    if (planStep!=NULL)
    {
        // Update the jump to step
        StepNumberStepParameter* planStepParameter = dynamic_cast<StepNumberStepParameter*>(GetStepParameter());
        TA_ASSERT(planStepParameter!=NULL,"Not a StepNumberStepParameter");
        planStepParameter->value(planStep->getStepId());
    }

    // Normal exit
    EndDialog(IDOK);

    FUNCTION_EXIT;
}

void CPlanStepParameterStepNumberEditDlg::SetSelectedStep(PlanStep *planStep, bool scrollIntoView)
{
	FUNCTION_ENTRY( "SetSelectedStep" );

	if (NULL != planStep)
	{
		// Find the item to select
		int item = FindStep(planStep->getStepPosition());
		if (item != -1)
		{
			m_stepList.SetItemState(item, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

			if (scrollIntoView)
			{
				m_stepList.EnsureVisible(item, FALSE);
			}
		}
	}

	FUNCTION_EXIT;
}

int CPlanStepParameterStepNumberEditDlg::FindStep(const unsigned long stepNumber)
{
	FUNCTION_ENTRY( "FindStep" );

	int itemCount = m_stepList.GetItemCount();
	if (stepNumber >= 1 && stepNumber <= static_cast<unsigned long>(itemCount))
	{
		// Start the search with the item at position (stepNumber - 1).
		// This should result in a direct hit.
		int item = 0;
		for (item = stepNumber - 1; item < m_stepList.GetItemCount(); item++)
		{
			PlanStep *planStep = reinterpret_cast<PlanStep *>(m_stepList.GetItemData(item));

			if (planStep->getStepPosition() == stepNumber)
			{
				FUNCTION_EXIT;
				return item;
			}
		}

		// Item not found at or above position (stepNumber - 1). Wrap around
		// to the top and search from there.
		for (item = 0; static_cast<unsigned long>(item) < stepNumber - 1; item++)
		{
			PlanStep *planStep = reinterpret_cast<PlanStep *>(m_stepList.GetItemData(item));

			if (planStep->getStepPosition() == stepNumber)
			{
				FUNCTION_EXIT;
				return item;
			}
		}
	}

	FUNCTION_EXIT;
	return -1;
}

}