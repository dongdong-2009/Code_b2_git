/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditTrainTisPage.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Facilitates the editing of Train TIS step parameters.
  *
  */

#include "StdAfx.h"

#ifdef _MSC_VER
#pragma warning(disable: 4503 4786)
#endif

#include <map>
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/tis_agent/IDL/src/TISMessageDataDefinitions.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/signs/tis_agent_access/src/TISAgentAccessFactory.h"
#include "resource.h"
#include "PlanStepEditTrainTisPage.h"
#include "TrainTisStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/TrainStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/TimeSpanStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/TextualStepParameter.h"
#include "TisConfigAccess.h"

namespace
{
	static const unsigned short TIS_UNDEFINED_TRAIN_ID = 0xFFFF;
	static const unsigned short TIS_UNDEFINED_MESSAGE_ID = 0xFFFF;
    static const unsigned short UNDEFINED_LIBRARY_VERSION(0xFFFF);

    static const int CB_GLOBAL_SEARCH(-1);
    static const int CB_NO_SELECTION(-1);

	const std::string ALL_TRAINS =  "All trains";
	const std::string INBOUND_TRAINS =  "Inbound trains";
	const std::string OUTBOUND_TRAINS =  "Outbound trains";
	const std::string TRAIN =  "Single train";
}

namespace TA_Base_App
{
/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditTrainTisPage dialog

CPlanStepEditTrainTisPage::CPlanStepEditTrainTisPage(TrainTisStep* step) :
    CPlanStepEditParameterPage(IDD_PLAN_STEP_EDIT_TRAIN_TIS_DLG, step)
{
	//{{AFX_DATA_INIT(CPlanStepEditTrainTisPage)
	//}}AFX_DATA_INIT
}

CPlanStepEditTrainTisPage::~CPlanStepEditTrainTisPage()
{
}

void CPlanStepEditTrainTisPage::EnableParameterControls(bool enable)
{
    FUNCTION_ENTRY("EnableParameterControls(bool enable)");

    TA_ASSERT(::IsWindow(GetSafeHwnd()), "Dialog not initialised");

    m_trainIdBtn.EnableWindow(enable);
    m_trainIdCombo.EnableWindow(enable);
    m_inboundTrainsBtn.EnableWindow(enable);
    m_outboundTrainsBtn.EnableWindow(enable);
    m_allTrainsBtn.EnableWindow(enable);

    m_messageEdit.EnableWindow(enable);
    m_pickMessageBtn.EnableWindow(enable);

    m_priorityCombo.EnableWindow(enable);
    m_durationPicker.EnableWindow(enable);

    FUNCTION_EXIT;
}

void CPlanStepEditTrainTisPage::DisplayParameters()
{
    FUNCTION_ENTRY("DisplayParameters()");

	std::vector<ItemPair> vecItems;
	GetPlanStep()->getActivityParameter().getNameValuePairs( vecItems );

	for ( std::vector<ItemPair>::iterator itLoop = vecItems.begin(); vecItems.end() != itLoop; ++itLoop )
	{
		if ( itLoop->Name.compare( ParamTrainId ) == 0 )
		{
			int trianId = atoi(itLoop->Item.c_str());
			int itemWidth = 2;
			std::stringstream ss;
			ss.clear();
			ss.str("");
			ss << std::setw(itemWidth) << std::setfill('0') << trianId;

			if (trianId > 0)
			{
				int item(m_trainIdCombo.FindStringExact(CB_GLOBAL_SEARCH, ss.str().c_str()));
				SetComboSelection(m_trainIdCombo, item);
			}
		}
		else if ( itLoop->Name.compare( ParamTrainGroup) == 0 )
		{
			std::string trainGroup = itLoop->Item;
			SetButtonState(m_allTrainsBtn, (trainGroup == ALL_TRAINS) ? BST_CHECKED : BST_UNCHECKED);
			SetButtonState(m_inboundTrainsBtn, (trainGroup == INBOUND_TRAINS) ? BST_CHECKED : BST_UNCHECKED);
			SetButtonState(m_outboundTrainsBtn, (trainGroup == OUTBOUND_TRAINS) ? BST_CHECKED : BST_UNCHECKED);
			SetButtonState(m_trainIdBtn, (trainGroup == TRAIN) ? BST_CHECKED : BST_UNCHECKED);
		}
		else if ( itLoop->Name.compare( ParamMessage ) == 0 )
		{
			m_messageEdit.SetWindowText(itLoop->Item.c_str());
		}
		else if ( itLoop->Name.compare( ParamPriority ) == 0 )
		{
			UpdatePriorityCombo(itLoop->Item);
		}
	}

	fillTimePicker( ParamDuration, m_durationPicker );

    FUNCTION_EXIT;
}

void CPlanStepEditTrainTisPage::UpdateParameters()
{
    FUNCTION_ENTRY("UpdateParameters()");

	std::vector<unsigned int> vecSelectedKeys;
	if (m_trainIdBtn.GetCheck() == BST_CHECKED)
	{
		int curSel = m_trainIdCombo.GetCurSel ();
		unsigned int key = m_trainIdCombo.GetItemData (curSel);
		vecSelectedKeys.push_back(key);
	}
	std::string strDisplayValue = "";
	GetPlanStep()->getActivityParameter().getListPicker(ParamTrainId)->pickFromList(ParamTrainId,vecSelectedKeys,strDisplayValue);

	vecSelectedKeys.clear();
	if (m_trainIdBtn.GetCheck() == BST_CHECKED)
	{
		vecSelectedKeys.push_back(TG_UNDEFINED);
	}
	else if (m_inboundTrainsBtn.GetCheck() == BST_CHECKED)
	{
		vecSelectedKeys.push_back(TG_INBOUND_TRAINS);
	}
	else if (m_outboundTrainsBtn.GetCheck() == BST_CHECKED)
	{
		vecSelectedKeys.push_back(TG_OUTBOUND_TRAINS);
	}
	else if (m_allTrainsBtn.GetCheck() == BST_CHECKED)
	{
		vecSelectedKeys.push_back(TG_ALL_TRAINS);
	}
 	GetPlanStep()->getActivityParameter().getListPicker(ParamTrainGroup)->pickFromList(ParamTrainGroup,vecSelectedKeys,strDisplayValue);

	vecSelectedKeys.clear();
	int curSel = m_priorityCombo.GetCurSel();
	if (curSel >= 0)
	{
		unsigned int key = m_priorityCombo.GetItemData(curSel);
		vecSelectedKeys.push_back(key);
	}
	GetPlanStep()->getActivityParameter().getListPicker(ParamPriority)->pickFromList(ParamPriority,vecSelectedKeys,strDisplayValue);

	updateTimeValue( ParamDuration, m_durationPicker);

    FUNCTION_EXIT;
}

void CPlanStepEditTrainTisPage::UpdatePriorityCombo(const std::string& strPriority)
{
    FUNCTION_ENTRY("UpdatePriorityCombo()");

	m_priorityCombo.ResetContent();
	std::vector<ListItem> vecKeyItems;
	std::vector<unsigned int> vecSelected;
	unsigned int nMaxSelection = 0u;
	int currentPriorityIndex = -1;
	GetPlanStep()->getActivityParameter().getListPicker( ParamPriority)->getInputListItems( ParamPriority, vecKeyItems, vecSelected, nMaxSelection );
	for ( std::vector<ListItem>::iterator itLoop = vecKeyItems.begin(); vecKeyItems.end() != itLoop; ++itLoop )
	{
		int nIdx = m_priorityCombo.AddString( itLoop->Item.c_str() );
		m_priorityCombo.SetItemData(nIdx, itLoop->ID );
		if (itLoop->ID == atoi(strPriority.c_str()))
		{
			currentPriorityIndex = nIdx;
		}
	}

	SetComboSelection(m_priorityCombo, currentPriorityIndex);

    FUNCTION_EXIT;
}

void CPlanStepEditTrainTisPage::SetButtonState(CButton& button, int checkState)
{
    FUNCTION_ENTRY("SetButtonState(CButton& button, int checkState)");

    TA_ASSERT(::IsWindow(GetSafeHwnd()), "Dialog not initialised");

    // Update the specified button's check state and ensure that the "button clicked" message
    // handler for that button is invoked.
    button.SetCheck(checkState);
    SendMessage(WM_COMMAND, MAKEWPARAM(button.GetDlgCtrlID(), BN_CLICKED), reinterpret_cast<LPARAM>(GetSafeHwnd()));

    FUNCTION_EXIT;
}

void CPlanStepEditTrainTisPage::SetComboSelection(CComboBox& combo, int item)
{
    FUNCTION_ENTRY("SetComboSelection(CComboBox& combo, int checkState)");

    TA_ASSERT(::IsWindow(GetSafeHwnd()), "Dialog not initialised");

    // Select the specified item in the combo box and ensure that the "selection changed" message
    // handler for that combo is invoked.
    combo.SetCurSel(item);
    SendMessage(WM_COMMAND, MAKEWPARAM(combo.GetDlgCtrlID(), CBN_SELCHANGE), reinterpret_cast<LPARAM>(GetSafeHwnd()));

    FUNCTION_EXIT;
}

void CPlanStepEditTrainTisPage::DoDataExchange(CDataExchange* pDX)
{
	CPlanStepEditParameterPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlanStepEditTrainTisPage)
	DDX_Control(pDX, IDC_TRAINTIS_DURATION_PICKER, m_durationPicker);
	DDX_Control(pDX, IDC_TRAINTIS_MESSAGE_EDIT, m_messageEdit);
	DDX_Control(pDX, IDC_TRAINTIS_PRIORITY_COMBO, m_priorityCombo);
	DDX_Control(pDX, IDC_TRAINTIS_TRAIN_ID_COMBO, m_trainIdCombo);
	DDX_Control(pDX, IDC_TRAINTIS_TRAIN_ID_RADIO, m_trainIdBtn);
    DDX_Control(pDX, IDC_TRAINTIS_INBOUND_TRAINS_RADIO, m_inboundTrainsBtn);
    DDX_Control(pDX, IDC_TRAINTIS_OUTBOUND_TRAINS_RADIO, m_outboundTrainsBtn);
    DDX_Control(pDX, IDC_TRAINTIS_ALL_TRAINS_RADIO, m_allTrainsBtn);
	DDX_Control(pDX, IDC_TRAINTIS_PICK_MESSAGE_BUTTON, m_pickMessageBtn);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditTrainTisPage message handlers

BEGIN_MESSAGE_MAP(CPlanStepEditTrainTisPage, CPlanStepEditParameterPage)
	//{{AFX_MSG_MAP(CPlanStepEditTrainTisPage)
	ON_CBN_SELCHANGE(IDC_TRAINTIS_TRAIN_ID_COMBO, OnTrainComboSelectionChanged)
	ON_CBN_SELCHANGE(IDC_TRAINTIS_PRIORITY_COMBO, OnPriorityComboSelectionChanged)
	ON_BN_CLICKED(IDC_TRAINTIS_TRAIN_ID_RADIO, OnTrainIdRadio)
	ON_BN_CLICKED(IDC_TRAINTIS_INBOUND_TRAINS_RADIO, OnTrainGroupRadio)
	ON_BN_CLICKED(IDC_TRAINTIS_OUTBOUND_TRAINS_RADIO, OnTrainGroupRadio)
	ON_BN_CLICKED(IDC_TRAINTIS_ALL_TRAINS_RADIO, OnTrainGroupRadio)
	ON_BN_CLICKED(IDC_TRAINTIS_PICK_MESSAGE_BUTTON, OnPickMessageButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CPlanStepEditTrainTisPage::OnInitDialog()
{
    FUNCTION_ENTRY("OnInitDialog()");

	CPlanStepEditParameterPage::OnInitDialog();

    // Populate the train list. The contents will remain unchanged.
	m_trainIdCombo.ResetContent();
	std::vector<ListItem> vecKeyItems;
	std::vector<unsigned int> vecSelected;
	unsigned int nMaxSelection = 0u;
	GetPlanStep()->getActivityParameter().getListPicker( ParamTrainId)->getInputListItems( ParamTrainId, vecKeyItems, vecSelected, nMaxSelection );
	for ( std::vector<ListItem>::iterator itLoop = vecKeyItems.begin(); vecKeyItems.end() != itLoop; ++itLoop )
	{
		int nIdx = m_trainIdCombo.AddString( itLoop->Item.c_str() );
		m_trainIdCombo.SetItemData(nIdx, itLoop->ID );
	}

    // The duration date/time picker control will only be used for hours and minutes,
    // so set the format such that the date and the seconds are hidden.
    m_durationPicker.SetFormat("HH:mm");

    FUNCTION_EXIT;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPlanStepEditTrainTisPage::OnTrainComboSelectionChanged()
{
    FUNCTION_ENTRY("OnTrainComboSelectionChanged()");

    // If a valid train is selected (or a train group button is active), we need to determine
    //  the version of the message library in use and immediately inform the operator of any problems.
    //SetTrainLibraryVersion();

    FUNCTION_EXIT;
}

void CPlanStepEditTrainTisPage::OnTrainIdRadio()
{
    FUNCTION_ENTRY("OnTrainRadio()");

    // Tabbing through the controls a BN_CLICKED message is generated when m_trainIdBtn
    // receives focus. If this happens while any of the other train buttons is selected,
    // simply ignore the notification.
    if (m_inboundTrainsBtn.GetCheck() == BST_CHECKED ||
        m_outboundTrainsBtn.GetCheck() == BST_CHECKED ||
        m_allTrainsBtn.GetCheck() == BST_CHECKED)
    {
        FUNCTION_EXIT;
        return;
    }

    m_trainIdCombo.EnableWindow(TRUE);

    // Clear train selection. OnTrainComboSelectionChanged() will set the library version
    // for the target destination.
    SetComboSelection(m_trainIdCombo, CB_NO_SELECTION);

    FUNCTION_EXIT;
}

void CPlanStepEditTrainTisPage::OnTrainGroupRadio()
{
    FUNCTION_ENTRY("OnTrainGroupRadio()");

    // Tabbing through the controls a BN_CLICKED message is generated when a train group
    // radio button receives focus. If this happens while m_trainIdBtn is selected, simply
    // ignore the notification.
    if (m_trainIdBtn.GetCheck() == BST_CHECKED)
    {
        FUNCTION_EXIT;
        return;
    }

    m_trainIdCombo.EnableWindow(FALSE);

    // Clear train selection. OnTrainComboSelectionChanged() will set the library version
    // for the target destinations.
    SetComboSelection(m_trainIdCombo, CB_NO_SELECTION);

    FUNCTION_EXIT;
}

void CPlanStepEditTrainTisPage::OnPickMessageButton()
{
    FUNCTION_ENTRY("OnPickMessageButton()");

	popupList(ParamMessage,ParamMessage,m_messageEdit);
	UpdatePriorityCombo(GetPlanStep()->getActivityParameter().displayValue(ParamPriority));

    FUNCTION_EXIT;
}

void CPlanStepEditTrainTisPage::OnPriorityComboSelectionChanged()
{
    int selectedPriority(m_priorityCombo.GetCurSel());
	m_priorityCombo.GetItemData(selectedPriority);
}
}