//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditDecisionParameterPage.cpp $
// @author:  Andy Parker
// @version: $Revision: #2 $
//
// Last modification: $DateTime: 2012/02/06 16:15:14 $
// Last modified by:  $Author: haijun.li $
//
// <description>

#include "stdafx.h"

#if defined (STEPTYPE_POPUP_MFT_MESSAGE)

#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/src/IProfile.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "PlanManager.h"
#include "PlanStepEditPopupMFTMessagePage.h"
#include "PopupMFTMessageStep.h"
#include "StepNumberStepParameter.h"
#include "TextualStepParameter.h"
#include "YesNoStepParameter.h"
#include "PlanStepParameterStepNumberEditDlg.h"
#include "PlanNode.h"
#include "PlanManagerCommonDefs.h"
#include "app/response_plans/plan_manager/src/ProfileConfigAccess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;

static const unsigned long MAX_BUTTON_NUM = 3;
static const unsigned long MAX_BUTTON_CONTROL_NUM = 3;
static const std::string ABORT_LABEL = "Abort Step:";
static const std::string RETRY_LABEL = "Retry Step:";
static const std::string IGNORE_LABEL = "Ignore Step:";
static const std::string OK_LABEL = "OK Step:";
static const std::string CANCEL_LABEL = "Cancel Step:";
static const std::string YES_LABEL = "Yes Step:";
static const std::string NO_LABEL = "No Step:";

static const unsigned long STEP_CONTROL_MAP[MAX_BUTTON_NUM][MAX_BUTTON_CONTROL_NUM] = {
    {IDC_FIRST_BUTTON_STATIC, IDC_FIRST_OPTION_STEP, IDC_FIRST_PICK_STEP_BUTTON},
    {IDC_SECOND_BUTTON_STATIC, IDC_SECOND_OPTION_STEP, IDC_SECOND_PICK_STEP_BUTTON},
    {IDC_THIRD_BUTTON_STATIC, IDC_THIRD_OPTION_STEP, IDC_THIRD_PICK_STEP_BUTTON},
};

// {Style description, button1 name, button2 name, ...}
static const std::string STYLE_STEP_MAP[MAX_MESSAGEBOX_STYLE][4] = {
    {"OK", OK_LABEL, "", ""},//MB_OK 0x00000000L
    {"OK and Cancel", OK_LABEL, CANCEL_LABEL, ""},//MB_OKCANCEL 0x00000001L
    {"Abort, Retry, and Ignore", ABORT_LABEL, RETRY_LABEL, IGNORE_LABEL},//MB_ABORTRETRYIGNORE 0x00000002L
    {"Yes, No, and Cancel", YES_LABEL, NO_LABEL, CANCEL_LABEL},//MB_YESNOCANCEL 0x00000003L
    {"Yes and No", YES_LABEL, NO_LABEL, ""},//MB_YESNO 0x00000004L
    {"Retry and Cancel", RETRY_LABEL, CANCEL_LABEL, ""},//MB_RETRYCANCEL 0x00000005L
};

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditPopupMFTMessagePage property page

IMPLEMENT_DYNAMIC(CPlanStepEditPopupMFTMessagePage, CPlanStepEditParameterPage)

CPlanStepEditPopupMFTMessagePage::CPlanStepEditPopupMFTMessagePage(PopupMFTMessageStep* step) :
CPlanStepEditParameterPage(CPlanStepEditPopupMFTMessagePage::IDD, step),
m_okStepParameter(*step->getOkParameter()),
m_noStepParameter(*step->getNoParameter()),
m_yesStepParameter(*step->getYesParameter()),
m_abortStepParameter(*step->getAbortParameter()),
m_ignoreStepParameter(*step->getIgnoreParameter()),
m_retryStepParameter(*step->getRetryParameter()),
m_cancelStepParameter(*step->getCancelParameter()),
m_pendingStepParameter(*step->getPendingParameter()),
m_messageboxStyleParameter(*step->getMessageboxStyle()),
m_operatorProfileParameter(*step->getOperatorProfile()),
m_timeSpanParameter(*step->getTimeSpanParameter())
{
    FUNCTION_ENTRY( "CPlanStepEditPopupMFTMessagePage" );

    // {{AFX_DATA_INIT(CPlanStepEditPopupMFTMessagePage)
    m_decisionMessage = _T("");
    m_pendingStep = _T("");
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CPlanStepEditPopupMFTMessagePage::~CPlanStepEditPopupMFTMessagePage()
{
    FUNCTION_ENTRY( "~CPlanStepEditPopupMFTMessagePage" );
    FUNCTION_EXIT;
}


void CPlanStepEditPopupMFTMessagePage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditPopupMFTMessagePage)
    DDX_Text(pDX, IDC_DECISION_MESSAGE, m_decisionMessage);
    DDX_Text(pDX, IDC_FIRST_OPTION_STEP, m_firstButtonStep);
    DDX_Text(pDX, IDC_SECOND_OPTION_STEP, m_secondButtonStep);
    DDX_Text(pDX, IDC_THIRD_OPTION_STEP, m_thirdButtonStep);
    DDX_Text(pDX, IDC_SUCCESS_STEP, m_pendingStep);
    DDX_Control(pDX, IDC_OPERATOR_PROFILE_COMBO, m_operatorProfileCombo);
    DDX_Control(pDX, IDC_MESSAGEBOX_STYLE_COMBO, m_messageboxStyleCombo);
    DDX_Control(pDX, IDC_TIMESPAN_PICKER, m_timeSpan);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditPopupMFTMessagePage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditPopupMFTMessagePage)
    ON_BN_CLICKED(IDC_FIRST_PICK_STEP_BUTTON, OnFirstPickStepButton)
    ON_BN_CLICKED(IDC_SECOND_PICK_STEP_BUTTON, OnSecondPickStepButton)
    ON_BN_CLICKED(IDC_THIRD_PICK_STEP_BUTTON, OnThirdPickStepButton)
    ON_BN_CLICKED(IDC_PICK_STEP_BUTTON, OnPendingPickStepButton)
    ON_CBN_SELCHANGE(IDC_MESSAGEBOX_STYLE_COMBO, OnMessageboxStyleChange)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditPopupMFTMessagePage message handlers

BOOL CPlanStepEditPopupMFTMessagePage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    if (!CPlanStepEditParameterPage::OnInitDialog())
    {
        FUNCTION_EXIT;
        return FALSE;
    }

    try
    {
        const ProfileMap& profileMap = ProfileConfigAccess::instance()->getAllProfile();
        for (ProfileMap::const_iterator iter = profileMap.begin(); iter != profileMap.end(); iter++)
        {
            int appTypeIdx = m_operatorProfileCombo.AddString(iter->second->getName().c_str());
            m_operatorProfileCombo.SetItemData(appTypeIdx, iter->second->getKey());
        }
    }
    catch (...)
    {
        return FALSE;
    }

    if(m_operatorProfileCombo.GetCount() > 0)
    {
        m_operatorProfileCombo.SetCurSel(0);
    }

    // Initialize messagebox style combo
    for ( unsigned long styleIdx = 0; styleIdx < MAX_MESSAGEBOX_STYLE; ++styleIdx)
    {
        int itemIndex = m_messageboxStyleCombo.AddString((LPCTSTR)STYLE_STEP_MAP[styleIdx][0].c_str());
        m_messageboxStyleCombo.SetItemData(itemIndex, styleIdx);
    }

    populateButtonStepParameterMap();

    FUNCTION_EXIT;
    return TRUE;
}

void CPlanStepEditPopupMFTMessagePage::OnPendingPickStepButton()
{
    FUNCTION_ENTRY( "OnPendingPickStepButton" );

    UpdateData(); // TD#1537

    // Select the required step
    m_pendingStepParameter.editParameter();
    m_pendingStep = m_pendingStepParameter.getDestinationStepDescription().c_str();

    UpdateData(FALSE);

    FUNCTION_EXIT;
}


void CPlanStepEditPopupMFTMessagePage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    CEdit* decisionMessage = static_cast<CEdit*>(GetDlgItem(IDC_DECISION_MESSAGE));
    if (decisionMessage)
    {
        decisionMessage->SetReadOnly(!enable);
    }

    CWnd* firstStepButton = GetDlgItem(IDC_FIRST_PICK_STEP_BUTTON);
    if (firstStepButton)
    {
        firstStepButton->EnableWindow(enable);
    }

    CWnd* secondStepButton = GetDlgItem(IDC_SECOND_PICK_STEP_BUTTON);
    if (secondStepButton)
    {
        secondStepButton->EnableWindow(enable);
    }

    CWnd* thirdStepButton = GetDlgItem(IDC_THIRD_PICK_STEP_BUTTON);
    if (thirdStepButton)
    {
        thirdStepButton->EnableWindow(enable);
    }

    CWnd* pendingStepButton = GetDlgItem(IDC_PICK_STEP_BUTTON);
    if (pendingStepButton)
    {
        pendingStepButton->EnableWindow(enable);
    }

    CWnd* operatorProfileCombo = GetDlgItem(IDC_OPERATOR_PROFILE_COMBO);
    if (operatorProfileCombo)
    {
        operatorProfileCombo->EnableWindow(enable);
    }

    CWnd* messageboxStyleCombo = GetDlgItem(IDC_MESSAGEBOX_STYLE_COMBO);
    if (messageboxStyleCombo)
    {
        messageboxStyleCombo->EnableWindow(enable);
    }

    FUNCTION_EXIT;
}


void CPlanStepEditPopupMFTMessagePage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

    UpdateData();

    PopupMFTMessageStep* step = dynamic_cast<PopupMFTMessageStep*>(GetPlanStep());
    TA_ASSERT(step!=NULL,"Incorrect step type for the decision step page");

    unsigned long messageboxStyle = step->getMessageboxStyle()->getNumericValue();
    m_decisionMessage = step->getMessageParameter()->getMessageText().c_str();
    m_pendingStep = m_pendingStepParameter.getDestinationStepDescription().c_str();

    TA_Base_Core::IProfile* iProfile = ProfileConfigAccess::instance()->getProfileByKey( m_operatorProfileParameter.getNumericValue() ) ;
    if (NULL != iProfile)
    {
        int appTypeIdx = m_operatorProfileCombo.FindStringExact(-1, iProfile->getName().c_str());
        if (appTypeIdx != CB_ERR)
        {
            m_operatorProfileCombo.SetCurSel(appTypeIdx);
        }
    }


    int msgboxStyleIdx = m_messageboxStyleCombo.FindStringExact(-1, (LPCSTR)STYLE_STEP_MAP[messageboxStyle][0].c_str());
    if (msgboxStyleIdx != CB_ERR)
    {
        m_messageboxStyleCombo.SetCurSel(msgboxStyleIdx);
    }
    
    OnMessageboxStyleChange();

    m_firstButtonStep = getStepDestinationStepDescriptionByButtonLabel(getWindowsTextAsStdString(IDC_FIRST_BUTTON_STATIC)).c_str();
    m_secondButtonStep = getStepDestinationStepDescriptionByButtonLabel(getWindowsTextAsStdString(IDC_SECOND_BUTTON_STATIC)).c_str();
    m_thirdButtonStep = getStepDestinationStepDescriptionByButtonLabel(getWindowsTextAsStdString(IDC_THIRD_BUTTON_STATIC)).c_str();

    m_timeSpan.SetFormat("HH:mm:ss");

    unsigned long timeSpanSecond = m_timeSpanParameter.getNumericValue();
    COleDateTimeSpan stepTimeSpan(0, 0, 0, (timeSpanSecond == 0)? 1 : timeSpanSecond);
    COleDateTime stepTimeSpanAsTime(COleDateTime::GetCurrentTime());
    stepTimeSpanAsTime.SetTime(stepTimeSpan.GetHours(), stepTimeSpan.GetMinutes(), stepTimeSpan.GetSeconds());
    m_timeSpan.SetTime(stepTimeSpanAsTime);

    UpdateData(FALSE);

    FUNCTION_EXIT;
}


void CPlanStepEditPopupMFTMessagePage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    PopupMFTMessageStep* step = dynamic_cast<PopupMFTMessageStep*>(GetPlanStep());
    TA_ASSERT(step!=NULL,"Incorrect step type for the decision step page");

    UpdateData(TRUE);

    step->getMessageParameter()->setText((LPCSTR) m_decisionMessage);
    step->getYesParameter()->setStepId(m_yesStepParameter.getStepId());
    step->getNoParameter()->setStepId(m_noStepParameter.getStepId());
    step->getPendingParameter()->setStepId(m_pendingStepParameter.getStepId());
    step->getAbortParameter()->setStepId(m_abortStepParameter.getStepId());
    step->getIgnoreParameter()->setStepId(m_ignoreStepParameter.getStepId());
    step->getRetryParameter()->setStepId(m_retryStepParameter.getStepId());
    step->getOkParameter()->setStepId(m_okStepParameter.getStepId());
    step->getCancelParameter()->setStepId(m_cancelStepParameter.getStepId());

    COleDateTime stepTimeSpanAsTime;
    m_timeSpan.GetTime(stepTimeSpanAsTime);
    COleDateTimeSpan stepTimeSpan(0, stepTimeSpanAsTime.GetHour(), stepTimeSpanAsTime.GetMinute(), stepTimeSpanAsTime.GetSecond());
    step->getTimeSpanParameter()->setValue(static_cast<TA_Base_Core::Seconds>(stepTimeSpan.GetTotalSeconds()));

    int appTypeIdx = m_operatorProfileCombo.GetCurSel();
    if (appTypeIdx != CB_ERR)
    {
        step->getOperatorProfile()->setValue(m_operatorProfileCombo.GetItemData(appTypeIdx));
    }

    int msgboxStyleIdx = m_messageboxStyleCombo.GetCurSel();
    if (msgboxStyleIdx != CB_ERR)
    {
        step->getMessageboxStyle()->setValue(m_messageboxStyleCombo.GetItemData(msgboxStyleIdx));
    }

    step->updateParameterList();

    FUNCTION_EXIT;
}

void TA_Base_App::CPlanStepEditPopupMFTMessagePage::OnMessageboxStyleChange()
{
    m_firstButtonStep = "";
    m_secondButtonStep = "";
    m_thirdButtonStep = "";

    int msgboxStyleIdx = m_messageboxStyleCombo.GetCurSel();
    if (msgboxStyleIdx != CB_ERR)
    {
        calibrateControlByMessageboxStyle(m_messageboxStyleCombo.GetItemData(msgboxStyleIdx));
    }
}

void TA_Base_App::CPlanStepEditPopupMFTMessagePage::populateButtonStepParameterMap()
{
    m_buttonStepParameterMap[ABORT_LABEL] = &m_abortStepParameter;
    m_buttonStepParameterMap[RETRY_LABEL] = &m_retryStepParameter;
    m_buttonStepParameterMap[IGNORE_LABEL] = &m_ignoreStepParameter;
    m_buttonStepParameterMap[OK_LABEL] = &m_okStepParameter;
    m_buttonStepParameterMap[CANCEL_LABEL] = &m_cancelStepParameter;
    m_buttonStepParameterMap[YES_LABEL] = &m_yesStepParameter;
    m_buttonStepParameterMap[NO_LABEL] = &m_noStepParameter;
}

void TA_Base_App::CPlanStepEditPopupMFTMessagePage::OnFirstPickStepButton()
{
    FUNCTION_ENTRY( "OnFirstPickStepButton" );

    UpdateData(); // TD#1537

    // Select the required step
    StepNumberStepParameter* stepParameter = m_buttonStepParameterMap[getWindowsTextAsStdString(IDC_FIRST_BUTTON_STATIC)];
    stepParameter->editParameter();
    m_firstButtonStep = stepParameter->getDestinationStepDescription().c_str();

    UpdateData(FALSE);

    FUNCTION_EXIT;
}

void TA_Base_App::CPlanStepEditPopupMFTMessagePage::OnSecondPickStepButton()
{
    FUNCTION_ENTRY( "OnSecondPickStepButton" );

    UpdateData(); // TD#1537

    // Select the required step
    StepNumberStepParameter* stepParameter = m_buttonStepParameterMap[getWindowsTextAsStdString(IDC_SECOND_BUTTON_STATIC)];
    stepParameter->editParameter();
    m_secondButtonStep = stepParameter->getDestinationStepDescription().c_str();

    UpdateData(FALSE);

    FUNCTION_EXIT;
}

void TA_Base_App::CPlanStepEditPopupMFTMessagePage::OnThirdPickStepButton()
{
    FUNCTION_ENTRY( "OnThirdPickStepButton" );

    UpdateData(); // TD#1537

    // Select the required step
    StepNumberStepParameter* stepParameter = m_buttonStepParameterMap[getWindowsTextAsStdString(IDC_THIRD_BUTTON_STATIC)];
    stepParameter->editParameter();
    m_thirdButtonStep = stepParameter->getDestinationStepDescription().c_str();

    UpdateData(FALSE);

    FUNCTION_EXIT;
}

std::string TA_Base_App::CPlanStepEditPopupMFTMessagePage::getWindowsTextAsStdString( UINT controlID )
{
    CWnd* pWnd = GetDlgItem(controlID);
    if (NULL != pWnd)
    {
        CString wndText;
        pWnd->GetWindowText(wndText);
        CT2CA pszConvertedAnsiString (wndText);

        std::string strStd (pszConvertedAnsiString);
        return strStd;
    }
    return "";
}

void TA_Base_App::CPlanStepEditPopupMFTMessagePage::calibrateControlByMessageboxStyle( unsigned long messageboxStyle )
{
    for (int buttonIndex = 1; buttonIndex <= MAX_BUTTON_NUM; ++buttonIndex)
    {
        if(!STYLE_STEP_MAP[messageboxStyle][buttonIndex].empty())
        {
            CWnd* pWnd = GetDlgItem(STEP_CONTROL_MAP[buttonIndex-1][0]);
            if (NULL != pWnd)
            {
                pWnd->SetWindowText((LPCSTR)STYLE_STEP_MAP[messageboxStyle][buttonIndex].c_str());
            }
        }
        setButtonControlVisible(buttonIndex, !STYLE_STEP_MAP[messageboxStyle][buttonIndex].empty());
    }
}

void TA_Base_App::CPlanStepEditPopupMFTMessagePage::setButtonControlVisible( unsigned long controlGroupIdex, bool bVisible )
{
    for (unsigned long controlIdex = 0; controlIdex < MAX_BUTTON_CONTROL_NUM; ++controlIdex)
    {
        CWnd* pWnd = GetDlgItem(STEP_CONTROL_MAP[controlGroupIdex-1][controlIdex]);
        if (NULL != pWnd)
        {
            pWnd->ShowWindow(bVisible?SW_SHOW:SW_HIDE);
        }
    }
}

std::string TA_Base_App::CPlanStepEditPopupMFTMessagePage::getStepDestinationStepDescriptionByButtonLabel( std::string buttonLebal )
{
    std::map<std::string, StepNumberStepParameter*>::iterator stepItr = m_buttonStepParameterMap.find(buttonLebal);
    if (m_buttonStepParameterMap.end() != stepItr)
    {
        return stepItr->second->getDestinationStepDescription();
    }
    return "";
}

#endif // defined (STEPTYPE_POPUP_MFT_MESSAGE)
