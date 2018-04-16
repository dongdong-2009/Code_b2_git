/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/NewPlanStepDlg.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #20 $
  *
  * Last modification: $DateTime: 2015/10/26 13:03:00 $
  * Last modified by:  $Author: qi.huang $
  *
  * This class represents a simple dialog displayed to the operator when creating new steps within a plan.
  * It enables the operator to pick the step's type and insert position wrt currently selected step(s),
  * i.e. before or after. If the currently selected step is the End Step, the insert after option is disabled.
  *
  **/

#include "app/response_plans/plan_manager/src/StdAfx.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_manager/src/resource.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/ApplicationFont.h"
#include "app/response_plans/plan_manager/src/NewPlanStepDlg.h"
#include "app/response_plans/plan_manager/src/Utilities.h"
#include "app/response_plans/plan_manager/src/PlanManagerCommonDefs.h"
#include "app/response_plans/plan_manager/src/MftStepImplementationFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace TA_Base_App;

/////////////////////////////////////////////////////////////////////////////
// CNewPlanStepDlg dialog


CNewPlanStepDlg::CNewPlanStepDlg(bool disableBeforeSelection, bool disableAfterSelection, CWnd* pParent /*=NULL*/) :
    CDialog(CNewPlanStepDlg::IDD, pParent),
    m_disableBeforeSelection(disableBeforeSelection),
    m_disableAfterSelection(disableAfterSelection),
    m_insertPosition(IPT_INSERT_AFTER),
    m_selectedStepType(0u)
{
    FUNCTION_ENTRY( "CNewPlanStepDlg" );

    // {{AFX_DATA_INIT(CNewPlanStepDlg)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CNewPlanStepDlg::~CNewPlanStepDlg()
{
    FUNCTION_ENTRY( "~CNewPlanStepDlg" );
    FUNCTION_EXIT;
}


void CNewPlanStepDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CNewPlanStepDlg)
    DDX_Control(pDX, IDC_STEP_TYPE, m_stepTypeCombo);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CNewPlanStepDlg, CDialog)
    // {{AFX_MSG_MAP(CNewPlanStepDlg)
    ON_BN_CLICKED(IDOK, OnOK)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewPlanStepDlg message handlers

BOOL CNewPlanStepDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CDialog::OnInitDialog();

    static std::vector<IActivityComponent*> steps = EditorStepFactory::instance()->getStepTypes();
    
    // Populate the plan step type selection drop-down with the list of supported steps.
    // Filter step population according to step use or not use
    for (std::vector<IActivityComponent*>::const_iterator stepIt = steps.begin(); stepIt != steps.end(); ++stepIt)
    {
        IActivityComponent* pActivity = (*stepIt);

        if ( NULL == pActivity )
        {
            continue;
        }

        if ( pActivity->readonly() || (( pActivity->getAccessType() != ACCESS_CLASSIC_ONLY ) && ( pActivity->getAccessType() != ACCESS_ALL )))
        {
            continue;
        }

        int stepTypeIdx = m_stepTypeCombo.AddString(pActivity->getName().c_str());
        m_stepTypeCombo.SetItemData(stepTypeIdx, pActivity->getStepType());
    }

    if ( m_stepTypeCombo.GetCount() > 0 )
    {
        m_stepTypeCombo.SetCurSel( 0 );
    }

    GetDlgItem(IDC_INSERT_BEFORE_SELECTION_RADIO)->EnableWindow(!m_disableBeforeSelection);
    GetDlgItem(IDC_INSERT_AFTER_SELECTION_RADIO)->EnableWindow(!m_disableAfterSelection);
	CheckDlgButton(m_disableAfterSelection ? IDC_INSERT_BEFORE_SELECTION_RADIO : IDC_INSERT_AFTER_SELECTION_RADIO, BST_CHECKED);

    ApplicationFont::getInstance().adjustChildrenFont( GetSafeHwnd() );

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CNewPlanStepDlg::OnOK()
{
    FUNCTION_ENTRY( "OnOK" );

    // Get selected insert position
    m_insertPosition = IsDlgButtonChecked(IDC_INSERT_BEFORE_SELECTION_RADIO)? IPT_INSERT_BEFORE : IPT_INSERT_AFTER;

    // Get selected step type
    int selectedStepTypeIdx = m_stepTypeCombo.GetCurSel();
    TA_ASSERT(selectedStepTypeIdx != CB_ERR, "No current selection in step type combo");

    m_selectedStepType = static_cast<int>(m_stepTypeCombo.GetItemData(selectedStepTypeIdx));

    // Normal exit
    EndDialog(IDOK);

    FUNCTION_EXIT;
}


