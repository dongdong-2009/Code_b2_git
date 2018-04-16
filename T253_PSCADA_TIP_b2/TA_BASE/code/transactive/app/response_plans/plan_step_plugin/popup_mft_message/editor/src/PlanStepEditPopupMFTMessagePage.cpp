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
#include "resource.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/src/IProfile.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepCommonDef.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/ProfileConfigAccess.h"
#include "PlanStepEditPopupMFTMessagePage.h"
#include "PopupMFTMessageStep.h"

namespace TA_Base_App
{

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
CPlanStepEditParameterPage(IDD_PLAN_STEP_EDIT_POPUP_MFT_MESSAGE_DLG, step)
{
    FUNCTION_ENTRY( "CPlanStepEditPopupMFTMessagePage" );
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
    DDX_Control(pDX, IDC_DECISION_MESSAGE, m_decisionMessage);
    DDX_Control(pDX, IDC_FIRST_OPTION_STEP, m_firstButtonStep);
    DDX_Control(pDX, IDC_SECOND_OPTION_STEP, m_secondButtonStep);
    DDX_Control(pDX, IDC_THIRD_OPTION_STEP, m_thirdButtonStep);
    DDX_Control(pDX, IDC_POPUP_SUCCESS_STEP, m_pendingStep);
    DDX_Control(pDX, IDC_OPERATOR_PROFILE_COMBO, m_operatorProfileCombo);
    DDX_Control(pDX, IDC_MESSAGEBOX_STYLE_COMBO, m_messageboxStyleCombo);
    DDX_Control(pDX, IDC_POPUP_TIMESPAN_PICKER, m_timeSpan);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditPopupMFTMessagePage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditPopupMFTMessagePage)
    ON_BN_CLICKED(IDC_FIRST_PICK_STEP_BUTTON, OnFirstPickStepButton)
    ON_BN_CLICKED(IDC_SECOND_PICK_STEP_BUTTON, OnSecondPickStepButton)
    ON_BN_CLICKED(IDC_THIRD_PICK_STEP_BUTTON, OnThirdPickStepButton)
    ON_BN_CLICKED(IDC_POPUP_PICK_STEP_BUTTON, OnPendingPickStepButton)
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

	m_operatorProfileCombo.ResetContent();
	std::vector<ListItem> vecKeyItems;
	std::vector<unsigned int> vecSelected;
	unsigned int nMaxSelection = 0u;
	GetPlanStep()->getActivityParameter().getListPicker( ParamOperatorProfileNumber)->getInputListItems( ParamOperatorProfileNumber, vecKeyItems, vecSelected, nMaxSelection );
	for ( std::vector<ListItem>::iterator itLoop = vecKeyItems.begin(); vecKeyItems.end() != itLoop; ++itLoop )
	{
		int nIdx = m_operatorProfileCombo.AddString( itLoop->Item.c_str() );
		m_operatorProfileCombo.SetItemData(nIdx, itLoop->ID );
	}

	m_messageboxStyleCombo.ResetContent();
	vecKeyItems.clear();
	vecSelected.clear();
	nMaxSelection = 0u;
	GetPlanStep()->getActivityParameter().getListPicker( ParamMessageBoxType)->getInputListItems( ParamMessageBoxType, vecKeyItems, vecSelected, nMaxSelection );
	for ( std::vector<ListItem>::iterator itLoop = vecKeyItems.begin(); vecKeyItems.end() != itLoop; ++itLoop )
	{
		int nIdx = m_messageboxStyleCombo.AddString( itLoop->Item.c_str() );
		m_messageboxStyleCombo.SetItemData(nIdx, itLoop->ID );
	}

    FUNCTION_EXIT;
    return TRUE;
}

void CPlanStepEditPopupMFTMessagePage::OnPendingPickStepButton()
{
    FUNCTION_ENTRY( "OnPendingPickStepButton" );

    popupList(ParamPendingStep, ParamPendingStep, m_pendingStep);

    FUNCTION_EXIT;
}


void CPlanStepEditPopupMFTMessagePage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    m_decisionMessage.SetReadOnly(!enable);
	m_operatorProfileCombo.EnableWindow(enable);
	m_messageboxStyleCombo.EnableWindow(enable);

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

    CWnd* pendingStepButton = GetDlgItem(IDC_POPUP_PICK_STEP_BUTTON);
    if (pendingStepButton)
    {
        pendingStepButton->EnableWindow(enable);
    }

    FUNCTION_EXIT;
}


void CPlanStepEditPopupMFTMessagePage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );
    
	std::string yesStep = "";
	std::string noStep = "";
	std::string abortStep = "";
	std::string ignoreStep = "";
	std::string retryStep = "";
	std::string okStep = "";
	std::string cancelStep = "";
	unsigned long messageboxStyle = 0;
 
	std::vector<ItemPair> vecItems;
	GetPlanStep()->getActivityParameter().getNameValuePairs( vecItems );

	for ( std::vector<ItemPair>::iterator itLoop = vecItems.begin(); vecItems.end() != itLoop; ++itLoop )
	{
		if ( itLoop->Name.compare( ParamDecisionMessage ) == 0 )
		{
			m_decisionMessage.SetWindowText( itLoop->Item.c_str() );
		}
		else if ( itLoop->Name.compare( ParamOperatorProfileNumber ) == 0 )
		{
			TA_Base_Core::IProfile* iProfile = ProfileConfigAccess::instance()->getProfileByKey( atol(itLoop->Item.c_str()) );
			if (NULL != iProfile)
			{
				int appTypeIdx = m_operatorProfileCombo.FindStringExact(-1, iProfile->getName().c_str());
				if (appTypeIdx != CB_ERR)
				{
					m_operatorProfileCombo.SetCurSel(appTypeIdx);
				}
			}
		}
		else if ( itLoop->Name.compare( ParamPendingStep ) == 0 )
		{
			m_pendingStep.SetWindowText( itLoop->Item.c_str() );
		}
		else if ( itLoop->Name.compare( ParamYesStep ) == 0 )
		{
			yesStep = itLoop->Item;
		}
		else if ( itLoop->Name.compare( ParamNoStep ) == 0 )
		{
			noStep = itLoop->Item;
		}
		else if ( itLoop->Name.compare( ParamAbortStep ) == 0 )
		{
			abortStep = itLoop->Item;
		}
		else if ( itLoop->Name.compare( ParamIgnoreStep ) == 0 )
		{
			ignoreStep = itLoop->Item;
		}
		else if ( itLoop->Name.compare( ParamRetryStep) == 0 )
		{
			retryStep = itLoop->Item;
		}
		else if ( itLoop->Name.compare( ParamOkStep ) == 0 )
		{
			okStep = itLoop->Item;
		}
		else if ( itLoop->Name.compare( ParamCancelStep ) == 0 )
		{
			cancelStep = itLoop->Item;
		}
		else if ( itLoop->Name.compare( ParamMessageBoxType ) == 0 )
		{
			messageboxStyle = atol(itLoop->Item.c_str());
		}
	}


    int msgboxStyleIdx = m_messageboxStyleCombo.FindStringExact(-1, (LPCSTR)STYLE_STEP_MAP[messageboxStyle][0].c_str());
    if (msgboxStyleIdx != CB_ERR)
    {
        m_messageboxStyleCombo.SetCurSel(msgboxStyleIdx);
    }
    
    OnMessageboxStyleChange();

	SetFirstToThirdStep(messageboxStyle, yesStep, noStep,abortStep,ignoreStep,retryStep,okStep,cancelStep);
    //m_firstButtonStep = getStepDestinationStepDescriptionByButtonLabel(getWindowsTextAsStdString(IDC_FIRST_BUTTON_STATIC)).c_str();
    //m_secondButtonStep = getStepDestinationStepDescriptionByButtonLabel(getWindowsTextAsStdString(IDC_SECOND_BUTTON_STATIC)).c_str();
    //m_thirdButtonStep = getStepDestinationStepDescriptionByButtonLabel(getWindowsTextAsStdString(IDC_THIRD_BUTTON_STATIC)).c_str();

    m_timeSpan.SetFormat("HH:mm:ss");
	fillTimePicker( ParamTimeSpan, m_timeSpan);

    FUNCTION_EXIT;
}

void CPlanStepEditPopupMFTMessagePage::SetFirstToThirdStep(unsigned long messageboxStyle,const std::string& yesStep,const std::string& noStep,const std::string& abortStep,const std::string& ignoreStep,const std::string& retryStep,const std::string& okStep,const std::string& cancelStep)
{
	const unsigned long MessageBoxStyle_OK = 0;
	const unsigned long MessageBoxStyle_OK_CANCEL = 1;
	const unsigned long MessageBoxStyle_ABORT_RETRY_IGNORE = 2;
	const unsigned long MessageBoxStyle_YES_NO_CANCEL = 3;
	const unsigned long MessageBoxStyle_YES_NO = 4;
	const unsigned long MessageBoxStyle_RETRY_CANCEL = 5;

    if (messageboxStyle == MessageBoxStyle_OK)
    {
        m_firstButtonStep.SetWindowText(okStep.c_str());
    }
	else if (messageboxStyle == MessageBoxStyle_OK_CANCEL)
	{
		m_firstButtonStep.SetWindowText(okStep.c_str());
		m_secondButtonStep.SetWindowText(cancelStep.c_str());
	}
	else if (messageboxStyle == MessageBoxStyle_ABORT_RETRY_IGNORE)
	{
		m_firstButtonStep.SetWindowText(abortStep.c_str());
		m_secondButtonStep.SetWindowText(retryStep.c_str());
		m_thirdButtonStep.SetWindowText(ignoreStep.c_str());
	}
	else if (messageboxStyle == MessageBoxStyle_YES_NO_CANCEL)
	{
		m_firstButtonStep.SetWindowText(yesStep.c_str());
		m_secondButtonStep.SetWindowText(noStep.c_str());
		m_thirdButtonStep.SetWindowText(cancelStep.c_str());
	}
	else if (messageboxStyle == MessageBoxStyle_YES_NO)
	{
		m_firstButtonStep.SetWindowText(yesStep.c_str());
		m_secondButtonStep.SetWindowText(noStep.c_str());
	}
	else if (messageboxStyle == MessageBoxStyle_RETRY_CANCEL)
	{
		m_firstButtonStep.SetWindowText(retryStep.c_str());
		m_secondButtonStep.SetWindowText(cancelStep.c_str());
	}
}

void CPlanStepEditPopupMFTMessagePage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

	updateValue( ParamDecisionMessage, m_decisionMessage );
	updateTimeValue( ParamTimeSpan, m_timeSpan );

    FUNCTION_EXIT;
}

void TA_Base_App::CPlanStepEditPopupMFTMessagePage::OnMessageboxStyleChange()
{
    int msgboxStyleIdx = m_messageboxStyleCombo.GetCurSel();
    if (msgboxStyleIdx != CB_ERR)
    {
        calibrateControlByMessageboxStyle(m_messageboxStyleCombo.GetItemData(msgboxStyleIdx));
    }
}

void TA_Base_App::CPlanStepEditPopupMFTMessagePage::OnFirstPickStepButton()
{
    FUNCTION_ENTRY( "OnFirstPickStepButton" );

	std::string messageBoxStyle = getWindowsTextAsStdString(IDC_FIRST_BUTTON_STATIC);
	if (OK_LABEL == messageBoxStyle)
	{
		popupList(ParamOkStep,ParamOkStep,m_firstButtonStep);
	}
	if (ABORT_LABEL == messageBoxStyle)
	{
		popupList(ParamAbortStep,ParamAbortStep,m_firstButtonStep);
	}
	if (YES_LABEL == messageBoxStyle)
	{
		popupList(ParamYesStep,ParamYesStep,m_firstButtonStep);
	}
	if (RETRY_LABEL == messageBoxStyle)
	{
		popupList(ParamRetryStep,ParamRetryStep,m_firstButtonStep);
	}
    FUNCTION_EXIT;
}

void TA_Base_App::CPlanStepEditPopupMFTMessagePage::OnSecondPickStepButton()
{
    FUNCTION_ENTRY( "OnSecondPickStepButton" );

	std::string messageBoxStyle = getWindowsTextAsStdString(IDC_SECOND_BUTTON_STATIC);
	if (CANCEL_LABEL == messageBoxStyle)
	{
		popupList(ParamCancelStep,ParamCancelStep,m_secondButtonStep);
	}
	if (RETRY_LABEL == messageBoxStyle)
	{
		popupList(ParamRetryStep,ParamRetryStep,m_secondButtonStep);
	}
	if (NO_LABEL == messageBoxStyle)
	{
		popupList(ParamNoStep,ParamNoStep,m_secondButtonStep);
	}

    FUNCTION_EXIT;
}

void TA_Base_App::CPlanStepEditPopupMFTMessagePage::OnThirdPickStepButton()
{
    FUNCTION_ENTRY( "OnThirdPickStepButton" );

	std::string messageBoxStyle = getWindowsTextAsStdString(IDC_THIRD_BUTTON_STATIC);
	if (IGNORE_LABEL == messageBoxStyle)
	{
		popupList(ParamIgnoreStep,ParamIgnoreStep,m_thirdButtonStep);
	}
	if (CANCEL_LABEL == messageBoxStyle)
	{
		popupList(ParamCancelStep,ParamCancelStep,m_thirdButtonStep);
	}
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

}
