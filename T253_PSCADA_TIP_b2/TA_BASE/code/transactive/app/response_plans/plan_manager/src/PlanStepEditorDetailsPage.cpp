//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditorDetailsPage.cpp $
// @author:  Andy Parker
// @version: $Revision: #9 $
//
// Last modification: $DateTime: 2015/03/04 16:03:13 $
// Last modified by:  $Author: huang.wenbo $
//
// <description>

#include "app/response_plans/plan_manager/src/stdafx.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "app/response_plans/plan_manager/src/PlanManager.h"
#include "app/response_plans/plan_manager/src/PlanStepEditorDetailsPage.h"
#include "app/response_plans/plan_manager/src/Utilities.h"
#include "app/response_plans/plan_manager/src/PlanManagerCommonDefs.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/ApplicationFont.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;

//as per TD 11659
static const UINT STEP_NAME_LIMIT = 150;
static const UINT STEP_DESCRIPTION_LIMIT = 950;

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditorDetailsPage property page

IMPLEMENT_DYNAMIC(CPlanStepEditorDetailsPage, CPropertyPage)

CPlanStepEditorDetailsPage::CPlanStepEditorDetailsPage(PlanStep* step, bool canEdit) :
    CPropertyPage(CPlanStepEditorDetailsPage::IDD),
    m_planStep(step),
    m_customStepDetail(NULL),
    m_canEdit(canEdit),
    m_canCustomise(false)
{
    FUNCTION_ENTRY( "CPlanStepEditorDetailsPage" );

    // {{AFX_DATA_INIT(CPlanStepEditorDetailsPage)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CPlanStepEditorDetailsPage::CPlanStepEditorDetailsPage(PlanStep* step, TA_Base_Core::StepCustomisableDetail* customStepDetail) :
    CPropertyPage(CPlanStepEditorDetailsPage::IDD),
    m_planStep(step),
    m_customStepDetail(customStepDetail),
    m_canEdit(false),
    m_canCustomise(true)
{
    FUNCTION_ENTRY( "CPlanStepEditorDetailsPage" );
    FUNCTION_EXIT;
}


CPlanStepEditorDetailsPage::~CPlanStepEditorDetailsPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditorDetailsPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditorDetailsPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPropertyPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditorDetailsPage)
    DDX_Control(pDX, IDC_DELAY_PICKER, m_delayPicker);
    DDX_Control(pDX, IDC_IGNORE_FAILURE_CHECK, m_ignoreFailureCheck);
    DDX_Control(pDX, IDC_SKIPABLE_CHECK, m_skippableCheck);
    DDX_Control(pDX, IDC_SKIP_CHECK, m_skipCheck);
    DDX_Control(pDX, IDC_STEP_DESCRIPTION, m_descriptionEdit);
    DDX_Control(pDX, IDC_STEP_NAME, m_nameEdit);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditorDetailsPage, CPropertyPage)
    // {{AFX_MSG_MAP(CPlanStepEditorDetailsPage)
    ON_BN_CLICKED(IDC_SKIP_CHECK, EnableControls)
    ON_BN_CLICKED(IDC_SKIPABLE_CHECK, EnableControls)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditorDetailsPage message handlers

BOOL CPlanStepEditorDetailsPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    TA_ASSERT(m_planStep != NULL, "Plan step is NULL");

    CPropertyPage::OnInitDialog();

    // as per TD 11659
    // limiting the number of chars in
    // step names to prevent DB write errors
    m_nameEdit.SetLimitText (STEP_NAME_LIMIT);

    // Initialise the delay date/time control. The only fields the user is able to see and edit are minutes and seconds.
    m_delayPicker.SetFormat("mm:ss"); // 00:00 -> 59:59

    // Set the controls
    bool isStepSkipped(m_canCustomise? m_customStepDetail->skip : m_planStep->isStepSkipped());
    bool ignoreFailure(m_canCustomise? m_customStepDetail->ignoreFailure : m_planStep->ignoreFailure());
    COleDateTimeSpan stepDelay(0, 0, 0, m_canCustomise? m_customStepDetail->delay : m_planStep->getStepDelay());

	m_descriptionEdit.SetLimitText(STEP_DESCRIPTION_LIMIT); ////CL-14921	
    m_nameEdit.SetWindowText(m_planStep->getStepName().c_str());
    m_descriptionEdit.SetWindowText(m_planStep->getStepDescription().c_str());
    SetDlgItemText(IDC_STEP_TYPE, m_planStep->getStepTypeName().c_str());
    m_skippableCheck.SetCheck(m_planStep->isStepSkippable());
    m_skipCheck.SetCheck(isStepSkipped);
    m_ignoreFailureCheck.SetCheck(ignoreFailure);

    // Even though the date is not show, we still must use a valid date with the date/time control.
    COleDateTime stepDelayAsTime(COleDateTime::GetCurrentTime());
    stepDelayAsTime.SetTime(0, stepDelay.GetMinutes(), stepDelay.GetSeconds());
    m_delayPicker.SetTime(stepDelayAsTime);

    if (m_canEdit || m_canCustomise)
    {
        EnableControls();
    }

    if (!m_canEdit)
    {
        // Disable controls if not in edit mode
        m_nameEdit.SetReadOnly(TRUE);
        m_descriptionEdit.SetReadOnly(TRUE);
        m_skippableCheck.EnableWindow(FALSE);

        if (!m_canCustomise)
        {
            // These controls should only be disabled in view mode
            m_skipCheck.EnableWindow(FALSE);
            m_ignoreFailureCheck.EnableWindow(FALSE);
            m_delayPicker.EnableWindow(FALSE);

            // Hide the Cancel button as it is redundant.
            // Position the OK button in the middle of the editor dialog.
            CWnd* propSheet = GetParent();
            if (propSheet)
            {
                CWnd* editorDlg = propSheet->GetParent();
                if (editorDlg)
                {
                    CWnd* cancelBtn = editorDlg->GetDlgItem(IDCANCEL);
                    if (cancelBtn)
                    {
                        cancelBtn->ShowWindow(SW_HIDE);
                    }

                    CWnd* okBtn = editorDlg->GetDlgItem(IDOK);
                    if (okBtn)
                    {
                        CRect editorDlgRect, okBtnRect;

                        editorDlg->GetWindowRect(editorDlgRect);
                        okBtn->GetWindowRect(okBtnRect);
                        editorDlg->ScreenToClient(okBtnRect);

                        int xCentreEditorDlg = editorDlgRect.left + editorDlgRect.Width() / 2;
                        int xCentreOkBtn = okBtnRect.left + okBtnRect.Width() / 2;

                        okBtnRect.OffsetRect(xCentreEditorDlg - xCentreOkBtn, 0);

                        okBtn->MoveWindow(okBtnRect);
                    }
                }
            }
        }
    }

    ApplicationFont::getInstance().adjustChildrenFont( GetSafeHwnd() );

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CPlanStepEditorDetailsPage::EnableControls()
{
    FUNCTION_ENTRY( "EnableControls" );

    TA_ASSERT(m_planStep != NULL, "Plan step is NULL");

    if (m_canEdit || m_canCustomise)
    {
        if (m_skippableCheck.GetCheck() == BST_CHECKED)
        {
            // Can skip the step
            m_skipCheck.EnableWindow(TRUE);

            // Cannot ignore failure or delay if step skipped
            if (m_skipCheck.GetCheck() == BST_CHECKED)
            {
                m_ignoreFailureCheck.SetCheck(BST_UNCHECKED);
                m_ignoreFailureCheck.EnableWindow(FALSE);

                COleDateTime stepDelayAsTime(COleDateTime::GetCurrentTime());
                stepDelayAsTime.SetTime(0, 0, 0); // reset delay to 00:00
                m_delayPicker.SetTime(stepDelayAsTime);
                m_delayPicker.EnableWindow(FALSE);
            }
            else
            {
                m_ignoreFailureCheck.EnableWindow(m_planStep->canIgnoreFailure());
                   m_delayPicker.EnableWindow(TRUE);
            }
        }
        else
        {
            // Cannot skip step but can ignore failure or delay
            m_skipCheck.SetCheck(BST_UNCHECKED);
            m_skipCheck.EnableWindow(FALSE);
            m_ignoreFailureCheck.EnableWindow(m_planStep->canIgnoreFailure());
            m_delayPicker.EnableWindow(TRUE);
        }
    }

    FUNCTION_EXIT;
}


void CPlanStepEditorDetailsPage::UpdateStepDetails()
{
    FUNCTION_ENTRY( "UpdateStepDetails" );

    TA_ASSERT(m_planStep != NULL, "Plan step is NULL");

    if (!m_canEdit)
    {
        FUNCTION_EXIT;
        return;
    }

    // Save the settings
    CString    text;
    m_nameEdit.GetWindowText(text);
    m_planStep->setStepName(text.operator LPCSTR());

    m_descriptionEdit.GetWindowText(text);
    m_planStep->setStepDescription(text.operator LPCSTR());

    m_planStep->setStepSkippable(m_skippableCheck.GetCheck() == BST_CHECKED);
    m_planStep->setStepSkipped(m_skipCheck.GetCheck() == BST_CHECKED);
    m_planStep->setIgnoreFailure(m_ignoreFailureCheck.GetCheck() == BST_CHECKED);

    COleDateTime stepDelayAsTime;
    m_delayPicker.GetTime(stepDelayAsTime);
    COleDateTimeSpan stepDelay(0, 0, stepDelayAsTime.GetMinute(), stepDelayAsTime.GetSecond());
    m_planStep->setStepDelay(static_cast<TA_Base_Core::Seconds>(stepDelay.GetTotalSeconds()));

    FUNCTION_EXIT;
}


void CPlanStepEditorDetailsPage::UpdateCustomDetails()
{
    FUNCTION_ENTRY( "UpdateCustomDetails" );

    TA_ASSERT(m_planStep != NULL, "Plan step is NULL");

    if (!m_canCustomise)
    {
        FUNCTION_EXIT;
        return;
    }

    // Save the settings
    COleDateTime stepDelayAsTime;
    m_delayPicker.GetTime(stepDelayAsTime);
    COleDateTimeSpan stepDelay(0, 0, stepDelayAsTime.GetMinute(), stepDelayAsTime.GetSecond());

    TA_ASSERT(m_customStepDetail, "Custom step detail is null");

    m_customStepDetail->id = m_planStep->getStepId();
    m_customStepDetail->skip = (m_skipCheck.GetCheck() == BST_CHECKED);
    m_customStepDetail->ignoreFailure = (m_ignoreFailureCheck.GetCheck() == BST_CHECKED);
    m_customStepDetail->delay = static_cast<TA_Base_Core::Seconds>(stepDelay.GetTotalSeconds());

    FUNCTION_EXIT;
}


