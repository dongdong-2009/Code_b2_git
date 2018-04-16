/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditStationTisPage.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2014/05/19 15:59:20 $
  * Last modified by:  $Author: qi.huang $
  *
  * Facilitates the editing of Station TIS step parameters.
  *
  */

#include "StdAfx.h"

#ifdef _MSC_VER
#pragma warning(disable: 4503 4786)
#endif

#include <map>
#include "resource.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/tis_agent/IDL/src/TISMessageDataDefinitions.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/signs/tis_agent_access/src/TISAgentAccessFactory.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "PlanStepEditStationTisPage.h"
#include "StationTisStep.h"
#include "TisConfigAccess.h"

namespace
{
    static const int PID_NAME_COLUMN_INDEX(0);

    static const unsigned short UNDEFINED_LIBRARY_VERSION(0xFFFF);

    static const int CB_GLOBAL_SEARCH(-1);
    static const int CB_NO_SELECTION(-1);
}

namespace TA_Base_App
{
/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditStationTisPage dialog

CPlanStepEditStationTisPage::CPlanStepEditStationTisPage(StationTisStep* step) :
    CPlanStepEditParameterPage(IDD_PLAN_STEP_EDIT_STATION_TIS_DLG, step)
{
    FUNCTION_ENTRY( "CPlanStepEditStationTisPage" );

    // {{AFX_DATA_INIT(CPlanStepEditStationTisPage)
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}

CPlanStepEditStationTisPage::~CPlanStepEditStationTisPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditStationTisPage" );
    FUNCTION_EXIT;
}

void CPlanStepEditStationTisPage::EnableParameterControls(bool enable)
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    TA_ASSERT(::IsWindow(GetSafeHwnd()), "Dialog not initialised");

    m_stationBtn.EnableWindow(enable);
    m_stationCombo.EnableWindow(enable);
    m_allStationsBtn.EnableWindow(enable);

    m_pidListBtn.EnableWindow(enable);
    m_pickPidBtn.EnableWindow(enable);
    SetListEnableState(m_pidList, enable);

    m_messageEdit.EnableWindow(enable);
    m_pickMessageBtn.EnableWindow(enable);

    m_priorityCombo.EnableWindow(enable);
    m_durationPicker.EnableWindow(enable);

    FUNCTION_EXIT;
}

void CPlanStepEditStationTisPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

	std::vector<ItemPair> vecItems;
	GetPlanStep()->getActivityParameter().getNameValuePairs( vecItems );

	for ( std::vector<ItemPair>::iterator itLoop = vecItems.begin(); vecItems.end() != itLoop; ++itLoop )
	{
		if ( itLoop->Name.compare( ParamAllStations ) == 0 )
		{
			m_allStationsBtn.SetCheck (itLoop->Item == "Yes" ? TRUE : FALSE);
		}
		else if ( itLoop->Name.compare( ParamStation ) == 0 )
		{
			if (itLoop->Item.empty())
			{
				m_stationBtn.SetCheck(FALSE);
				m_stationCombo.EnableWindow(FALSE);
			}
			else
			{
				m_stationBtn.SetCheck(TRUE);
				m_stationCombo.EnableWindow(TRUE);
				m_stationCombo.SetCurSel(m_stationCombo.FindStringExact( 0, itLoop->Item.c_str()));
			}
		}
		else if ( itLoop->Name.compare( ParamStationPids ) == 0 )
		{
			m_pidListBtn.SetCheck(itLoop->Item.empty() ? BST_UNCHECKED : BST_CHECKED);
			std::vector<std::string> pids;
			char *tokenPtr=strtok(const_cast<char*>(itLoop->Item.c_str()),",");
			while(tokenPtr!=NULL){
				pids.push_back(tokenPtr);
				tokenPtr=strtok(NULL,",");
			}
			UpdatePidList(pids);
		}
		else if ( itLoop->Name.compare( ParamMessage ) == 0 )
		{
			m_messageEdit.SetWindowText(itLoop->Item.c_str());
		}
		else if ( itLoop->Name.compare( ParamPriority ) == 0 )
		{
			int item(m_priorityCombo.AddString(itLoop->Item.c_str()));
			UpdatePriorityCombo(itLoop->Item);
		}
	}

	fillTimePicker( ParamDuration, m_durationPicker );

    FUNCTION_EXIT;
}

void CPlanStepEditStationTisPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );
	std::vector<unsigned int> vecSelectedKeys;
	if (m_stationBtn.GetCheck() == BST_CHECKED)
	{
		int curSel = m_stationCombo.GetCurSel();
		unsigned int key = m_stationCombo.GetItemData(curSel);
		vecSelectedKeys.push_back(key);
	}
	std::string strDisplayValue = "";
	GetPlanStep()->getActivityParameter().getListPicker(ParamStation)->pickFromList(ParamStation,vecSelectedKeys,strDisplayValue);

	vecSelectedKeys.clear();
	int curSel = m_priorityCombo.GetCurSel();
	unsigned int key = m_priorityCombo.GetItemData(curSel);
	vecSelectedKeys.push_back(key);
	GetPlanStep()->getActivityParameter().getListPicker(ParamPriority)->pickFromList(ParamPriority,vecSelectedKeys,strDisplayValue);

	vecSelectedKeys.clear();
	if (m_pidListBtn.GetCheck() == BST_UNCHECKED)
	{
		GetPlanStep()->getActivityParameter().getListPicker(ParamStationPids)->pickFromList(ParamStationPids,vecSelectedKeys,strDisplayValue);
	}

	updateBooleanValue( ParamAllStations, m_allStationsBtn );
	updateTimeValue( ParamDuration, m_durationPicker);

    FUNCTION_EXIT;
}

void CPlanStepEditStationTisPage::UpdatePidList(const std::vector<std::string>& pids)
{
    FUNCTION_ENTRY( "UpdatePidList" );

    TA_ASSERT(::IsWindow(GetSafeHwnd()), "Dialog not initialised");

    int pidColumnWidth(0);

    m_pidList.DeleteAllItems();
	int index = 0;

	for (std::vector<std::string>::const_iterator itPid = pids.begin(); itPid != pids.end(); ++itPid)
    {
        m_pidList.InsertItem(index, (*itPid).c_str());

        pidColumnWidth = __max(pidColumnWidth, m_pidList.GetStringWidth((*itPid).c_str()));

		index++;
    }

    m_pidList.SetColumnWidth(PID_NAME_COLUMN_INDEX, pidColumnWidth + 10);

    FUNCTION_EXIT;
}

void CPlanStepEditStationTisPage::UpdatePriorityCombo(const std::string& strPriority)
{
    FUNCTION_ENTRY( "UpdatePriorityCombo" );

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
		if (itLoop->Item == strPriority)
		{
           currentPriorityIndex = nIdx;
		}
	}

	SetComboSelection(m_priorityCombo, currentPriorityIndex);

    FUNCTION_EXIT;
}

void CPlanStepEditStationTisPage::SetButtonState(CButton& button, int checkState)
{
    FUNCTION_ENTRY( "SetButtonState" );

    TA_ASSERT(::IsWindow(GetSafeHwnd()), "Dialog not initialised");

    // Update the specified button's check state and ensure that the "button clicked" message
    // handler for that button is invoked.
    button.SetCheck(checkState);
    SendMessage(WM_COMMAND, MAKEWPARAM(button.GetDlgCtrlID(), BN_CLICKED), reinterpret_cast<LPARAM>(GetSafeHwnd()));

    FUNCTION_EXIT;
}

void CPlanStepEditStationTisPage::SetListEnableState(CListCtrl& list, BOOL enabled)
{
    FUNCTION_ENTRY( "SetListEnableState" );

    TA_ASSERT(::IsWindow(GetSafeHwnd()), "Dialog not initialised");

    // Grey out the list background when diabled.
    DWORD colorRef = ::GetSysColor(enabled? COLOR_WINDOW : COLOR_3DFACE);

    list.EnableWindow(enabled);
    list.SetBkColor(colorRef);
    list.SetTextBkColor(colorRef);

    FUNCTION_EXIT;
}

void CPlanStepEditStationTisPage::SetComboSelection(CComboBox& combo, int item)
{
    FUNCTION_ENTRY( "SetComboSelection" );

    TA_ASSERT(::IsWindow(GetSafeHwnd()), "Dialog not initialised");

    // Select the specified item in the combo box and ensure that the "selection changed" message
    // handler for that combo is invoked.
    combo.SetCurSel(item);
    SendMessage(WM_COMMAND, MAKEWPARAM(combo.GetDlgCtrlID(), CBN_SELCHANGE), reinterpret_cast<LPARAM>(GetSafeHwnd()));

    FUNCTION_EXIT;
}

void CPlanStepEditStationTisPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditStationTisPage)
    DDX_Control(pDX, IDC_STATIONTIS_PID_LIST, m_pidList);
    DDX_Control(pDX, IDC_STATIONTIS_DURATION_PICKER, m_durationPicker);
    DDX_Control(pDX, IDC_STATIONTIS_MESSAGE_EDIT, m_messageEdit);
    DDX_Control(pDX, IDC_STATIONTIS_PRIORITY_COMBO, m_priorityCombo);
    DDX_Control(pDX, IDC_STATIONTIS_STATION_COMBO, m_stationCombo);
    DDX_Control(pDX, IDC_STATIONTIS_STATION_RADIO, m_stationBtn);
    DDX_Control(pDX, IDC_STATIONTIS_ALL_STATIONS_RADIO, m_allStationsBtn);
    DDX_Control(pDX, IDC_STATIONTIS_PID_LIST_CHECK, m_pidListBtn);
    DDX_Control(pDX, IDC_STATIONTIS_PICK_PID_BUTTON, m_pickPidBtn);
    DDX_Control(pDX, IDC_STATIONTIS_PICK_MESSAGE_BUTTON, m_pickMessageBtn);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditStationTisPage message handlers

BEGIN_MESSAGE_MAP(CPlanStepEditStationTisPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditStationTisPage)
    ON_CBN_SELCHANGE(IDC_STATIONTIS_STATION_COMBO, OnStationComboSelectionChanged)
    ON_CBN_SELCHANGE(IDC_STATIONTIS_PRIORITY_COMBO, OnPriorityComboSelectionChanged)
    ON_BN_CLICKED(IDC_STATIONTIS_STATION_RADIO, OnStationRadio)
    ON_BN_CLICKED(IDC_STATIONTIS_ALL_STATIONS_RADIO, OnAllStationsRadio)
    ON_BN_CLICKED(IDC_STATIONTIS_PID_LIST_CHECK, OnPidListCheck)
    ON_BN_CLICKED(IDC_STATIONTIS_PICK_PID_BUTTON, OnPickPidButton)
    ON_BN_CLICKED(IDC_STATIONTIS_PICK_MESSAGE_BUTTON, OnPickMessageButton)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CPlanStepEditStationTisPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CPlanStepEditParameterPage::OnInitDialog();

    // Populate the station list. The contents will remain unchanged.
	m_stationCombo.ResetContent();
	std::vector<ListItem> vecKeyItems;
	std::vector<unsigned int> vecSelected;
	unsigned int nMaxSelection = 0u;
	GetPlanStep()->getActivityParameter().getListPicker( ParamStation)->getInputListItems( ParamStation, vecKeyItems, vecSelected, nMaxSelection );
	for ( std::vector<ListItem>::iterator itLoop = vecKeyItems.begin(); vecKeyItems.end() != itLoop; ++itLoop )
	{
		int nIdx = m_stationCombo.AddString( itLoop->Item.c_str() );
		m_stationCombo.SetItemData(nIdx, itLoop->ID );
	}

    // Initialise the PID list control.
    m_pidList.SetExtendedStyle(m_pidList.GetExtendedStyle() | LVS_EX_INFOTIP);
    m_pidList.InsertColumn(PID_NAME_COLUMN_INDEX, "", LVCFMT_LEFT, 0);

    // The duration date/time picker control will only be used for time,
    // so set the format such that the date is hidden.
    m_durationPicker.SetFormat("HH:mm:ss");

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void CPlanStepEditStationTisPage::OnStationComboSelectionChanged()
{
    FUNCTION_ENTRY( "OnStationComboSelectionChanged" );

    // If no station is currently selected in the station combo and All Stations is not active,
    // it makes no sense to let the operator pick station levels or PIDs, so these options must
    // be disabled.
    CString selectedStation;
    m_stationCombo.GetWindowText(selectedStation);
    bool disablePidFilters(selectedStation.IsEmpty() && m_allStationsBtn.GetCheck() == BST_UNCHECKED);

    if (disablePidFilters)
    {
        m_pidListBtn.EnableWindow(FALSE);

        SetButtonState(m_pidListBtn, BST_UNCHECKED);
    }
    else
    {
        m_pidListBtn.EnableWindow(TRUE);

        // Re-set the buttons to their current check state. This effectively invokes the respective
        // BN_CLICKED handlers and updates the station level combo and the PID list controls.
        SetButtonState(m_pidListBtn, m_pidListBtn.GetCheck());
    }

    // If a valid station is selected (or All Stations is active), we need to determine the version
    // of the message library in use and immediately inform the operator of any problems.
    //SetStationLibraryVersion();

    FUNCTION_EXIT;
}

void CPlanStepEditStationTisPage::OnStationRadio()
{
    FUNCTION_ENTRY( "OnStationRadio" );

    // Tabbing through the controls a BN_CLICKED message is generated when m_stationBtn
    // receives focus. If this happens while the m_allStationsBtn is selected, simply
    // ignore the notification.
    if (m_allStationsBtn.GetCheck() == BST_CHECKED)
    {
        FUNCTION_EXIT;
        return;
    }

    m_stationCombo.EnableWindow(m_stationBtn.GetCheck() == BST_CHECKED);

    // Clear station selection. OnStationComboSelectionChanged() will reset the station
    // level combo and set the library version for the target destination.
    SetComboSelection(m_stationCombo, CB_NO_SELECTION);

    FUNCTION_EXIT;
}

void CPlanStepEditStationTisPage::OnAllStationsRadio()
{
    FUNCTION_ENTRY( "OnAllStationsRadio" );

    // Tabbing through the controls a BN_CLICKED message is generated when m_allStationsBtn
    // receives focus. If this happens while the m_stationBtn is selected, simply ignore
    // the notification.
    if (m_stationBtn.GetCheck() == BST_CHECKED)
    {
        FUNCTION_EXIT;
        return;
    }

    m_stationCombo.EnableWindow(FALSE);

    // Clear station selection. OnStationComboSelectionChanged() will populate the station
    // level combo for all stations and set the library version for the target destinations.
    SetComboSelection(m_stationCombo, CB_NO_SELECTION);

    FUNCTION_EXIT;
}

void CPlanStepEditStationTisPage::OnPidListCheck()
{
    FUNCTION_ENTRY( "OnPidListCheck" );

    if (m_pidListBtn.GetCheck() == BST_CHECKED)
    {
        m_pickPidBtn.EnableWindow(TRUE);
        SetListEnableState(m_pidList, TRUE);
    }
    else
    {
        m_pickPidBtn.EnableWindow(FALSE);
        SetListEnableState(m_pidList, FALSE);
    }

	std::vector<std::string> pids;
    UpdatePidList(pids);

    FUNCTION_EXIT;
}

void CPlanStepEditStationTisPage::OnPickPidButton()
{
    FUNCTION_ENTRY( "OnPickPidButton" );

	popupList(ParamStationPids, ParamStationPids, m_pidList);

	std::string pid = GetPlanStep()->getActivityParameter().displayValue(ParamStationPids);
	std::vector<std::string> pids;
	char *tokenPtr=strtok(const_cast<char*>(pid.c_str()),",");
	while(tokenPtr!=NULL){
		pids.push_back(tokenPtr);
		tokenPtr=strtok(NULL,",");
	}
	UpdatePidList(pids);
    FUNCTION_EXIT;
}

void CPlanStepEditStationTisPage::OnPickMessageButton()
{
    FUNCTION_ENTRY( "OnPickMessageButton" );

	popupList(ParamMessage,ParamMessage,m_messageEdit);
	UpdatePriorityCombo(GetPlanStep()->getActivityParameter().displayValue(ParamPriority));

    FUNCTION_EXIT;
}

void CPlanStepEditStationTisPage::OnPriorityComboSelectionChanged()
{
    FUNCTION_ENTRY( "OnPriorityComboSelectionChanged" );

    int selectedPriority(m_priorityCombo.GetCurSel());
	m_priorityCombo.GetItemData(selectedPriority);

    FUNCTION_EXIT;
}
}