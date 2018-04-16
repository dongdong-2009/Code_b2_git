/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditStationTisPage.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Facilitates the editing of Station TIS step parameters.
  *
  */

#include "StdAfx.h"

#if defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM)

#ifdef _MSC_VER
#pragma warning(disable: 4503 4786)
#endif

#include <map>
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "PlanManager.h"
#include "PlanStepEditStationTisPage.h"
#include "StationTisStep.h"
#include "StationStepParameter.h"
#include "TimeSpanStepParameter.h"
#include "TextualStepParameter.h"
#include "TisConfigAccess.h"
#include "PlanNode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
    static const int PID_NAME_COLUMN_INDEX(0);

    static const unsigned short UNDEFINED_LIBRARY_VERSION(0xFFFF);

    static const int CB_GLOBAL_SEARCH(-1);
    static const int CB_NO_SELECTION(-1);
}

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditStationTisPage dialog


CPlanStepEditStationTisPage::CPlanStepEditStationTisPage(StationTisStep* step) :
    CPlanStepEditParameterPage(CPlanStepEditStationTisPage::IDD, step),
    m_stationLibraryVersion(UNDEFINED_LIBRARY_VERSION),
    m_pidListParameter(*step->getPidListParameter()),
    m_messageParameter(*step->getMessageParameter()),
    m_priorityParameter(*step->getPriorityParameter())
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

    m_stationLevelBtn.EnableWindow(enable);
    m_stationLevelCombo.EnableWindow(enable);

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

    TA_ASSERT(::IsWindow(GetSafeHwnd()), "Dialog not initialised");

    StationTisStep* step = dynamic_cast<StationTisStep*>(GetPlanStep());
    TA_ASSERT(step != NULL, "Incorrect step type for the Station TIS step page");

    CWaitCursor waitCursor;

    // Initialise the destination station section.
    TA_Base_Core::StationDestination station(step->getStationParameter()->getDestination());

    if (station.allStations)
    {
        SetButtonState(m_allStationsBtn, BST_CHECKED);
    }
    else
    {
        SetButtonState(m_stationBtn, BST_CHECKED);

        std::string stationName(station.station.in());

        if (!stationName.empty())
        {
            // Try to locate the station in the station combo.
            int item(m_stationCombo.FindStringExact(CB_GLOBAL_SEARCH, stationName.c_str()));

            if (item == CB_ERR)
            {
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << stationName;
                userMsg.showMsgBox(IDS_UE_210103);
            }
            else
            {
                // This will call OnStationComboSelectionChanged() to determine the current
                // library version number at this station.
                SetComboSelection(m_stationCombo, item);
            }
        }
    }

    // Initialise the sign filter section.
	TextualStepParameter* stationLevelParameter = step->getStationLevelParameter();
	TisPidListStepParameter* pidListParameter = step->getPidListParameter();
/*	if(!stationLevelParameter->getMessageText().empty())
	{
		pidListParameter->resetPids();
	}
	else if(pidListParameter->getPids().length() > 0)
	{
		stationLevelParameter->setText("");
	}
*/
    std::string stationLevel(stationLevelParameter->getMessageText());
    CString strStationLevel = stationLevel.c_str();
    strStationLevel.Trim(" ");
    stationLevel = strStationLevel;
    TA_Base_Core::TISPidList pids(pidListParameter->getPids());

    if (stationLevel.empty())
    {
        SetButtonState(m_stationLevelBtn, BST_UNCHECKED);
    }
    else
    {
        SetButtonState(m_stationLevelBtn, BST_CHECKED);

        // Try to locate the station level in the station level combo.
        int item(m_stationLevelCombo.FindStringExact(CB_GLOBAL_SEARCH, stationLevel.c_str()));

        if (item == CB_ERR)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << stationLevel;
            userMsg.showMsgBox(IDS_UE_210104);
        }
        else
        {
            SetComboSelection(m_stationLevelCombo, item);
        }
    }

    if (pids.length() == 0)
    {
        SetButtonState(m_pidListBtn, BST_UNCHECKED);
    }
    else
    {
        SetButtonState(m_pidListBtn, BST_CHECKED);

        // Station combo selection change resets m_pidListParameter so must reassign.
        m_pidListParameter.setPids(pids);
        UpdatePidList();
    }

    // Initialise message and priority.
    m_messageEdit.SetWindowText(m_messageParameter.getMessageText().c_str());
    UpdatePriorityCombo();

    // Initialise duration. Use any valid date with the picker control, it will be suppressed anyway.
     // as per TD 12382
     // for new steps set default duration to 1 minute
     unsigned long initialDuration = step->getDurationParameter()->getNumericValue();
     if (initialDuration == 0)
     {
         initialDuration = 1;
     }


    COleDateTimeSpan duration(0, 0, 0, initialDuration);
    TA_ASSERT(duration.GetDays() == 0, "Invalid message duration");
    COleDateTime durationAsTime(COleDateTime::GetCurrentTime());
    durationAsTime.SetTime(duration.GetHours(), duration.GetMinutes(), duration.GetSeconds());
    m_durationPicker.SetTime(durationAsTime);

    FUNCTION_EXIT;
}


void CPlanStepEditStationTisPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    TA_ASSERT(::IsWindow(GetSafeHwnd()), "Dialog not initialised");

    StationTisStep* step = dynamic_cast<StationTisStep*>(GetPlanStep());
    TA_ASSERT(step != NULL, "Incorrect step type for the Station TIS step page");

    StationStepParameter* stationParameter = step->getStationParameter();
    TA_ASSERT(stationParameter != NULL, "Station parameter not set");

    TextualStepParameter* stationLevelParameter = step->getStationLevelParameter();
    TA_ASSERT(stationLevelParameter != NULL, "Station level parameter not set");

    TisPidListStepParameter* pidListParameter = step->getPidListParameter();
    TA_ASSERT(pidListParameter != NULL, "PID list parameter not set");

    TisMessageStepParameter* messageParameter = step->getMessageParameter();
    TA_ASSERT(messageParameter != NULL, "Message parameter not set");

    TisPriorityStepParameter* priorityParameter = step->getPriorityParameter();
    TA_ASSERT(priorityParameter != NULL, "Priority parameter not set");

    TimeSpanStepParameter* durationParameter = step->getDurationParameter();
    TA_ASSERT(durationParameter != NULL, "Duration parameter not set");

    // Update the station destination parameter
    if (m_allStationsBtn.GetCheck() == BST_CHECKED)
    {
        stationParameter->setAllStations();
    }
    else
    {
        CString stationName("");
        m_stationCombo.GetWindowText(stationName);
        stationParameter->setStation(static_cast<LPCTSTR>(stationName), 0u);
    }

    // Update the station level parameter. If the station level option is unchecked,
    // the parameter is set to an empty string.
    CString stationLevel("");

    if (m_stationLevelBtn.GetCheck() == BST_CHECKED)
    {
        m_stationLevelCombo.GetWindowText(stationLevel);
    }

    stationLevelParameter->setText(static_cast<LPCTSTR>(stationLevel));

    // Set the PID list parameter. If the PID option is unchecked, the parameter
    // is set to an empty list.
    pidListParameter->setPids(m_pidListParameter.getPids());

    // Update the message parameter.
    messageParameter->setMessage(m_messageParameter.getMessage());

    // Update the priority parameter.
    priorityParameter->setValue(m_priorityParameter.getNumericValue());
    priorityParameter->hideValue(m_priorityParameter.isHiddenValue());

    // Update the duration parameter.
    COleDateTime durationAsTime;
    m_durationPicker.GetTime(durationAsTime);

    COleDateTimeSpan duration(0, durationAsTime.GetHour(), durationAsTime.GetMinute(), durationAsTime.GetSecond());
    durationParameter->setValue(static_cast<unsigned long>(duration.GetTotalSeconds()));
    durationParameter->hideValue(durationParameter->getNumericValue() == 0);

    FUNCTION_EXIT;
}


void CPlanStepEditStationTisPage::UpdatePidList()
{
    FUNCTION_ENTRY( "UpdatePidList" );

    TA_ASSERT(::IsWindow(GetSafeHwnd()), "Dialog not initialised");

    // Populate the PID list control and adjust the width of the PID column to accomodate
    // the longest PID string.
    TA_Base_Core::TISPidList pids(m_pidListParameter.getPids());
    int pidColumnWidth(0);

    m_pidList.DeleteAllItems();

    for (unsigned long i(0); i < pids.length(); i++)
    {
        m_pidList.InsertItem(i, pids[i].in());

        pidColumnWidth = __max(pidColumnWidth, m_pidList.GetStringWidth(pids[i].in()));
    }

    m_pidList.SetColumnWidth(PID_NAME_COLUMN_INDEX, pidColumnWidth + 10);

    FUNCTION_EXIT;
}


void CPlanStepEditStationTisPage::UpdatePriorityCombo()
{
    FUNCTION_ENTRY( "UpdatePriorityCombo" );

    TA_ASSERT(::IsWindow(GetSafeHwnd()), "Dialog not initialised");

    m_priorityCombo.ResetContent();
    m_priorityCombo.EnableWindow(m_messageParameter.getMessage().messageId != TA_Base_Core::TIS_UNDEFINED_MESSAGE_ID);

    if (m_priorityCombo.IsWindowEnabled())
    {
        // Populate the priority combo with priorities corresponding to the library section
        // associated with the current message.
        typedef std::vector<unsigned short> PriorityList;
        PriorityList priorities(m_priorityParameter.getTisPriorities(
            static_cast<TA_Base_Core::ELibrarySection>(m_messageParameter.getMessage().librarySection)));

        if (priorities.empty())
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_210114);
        }
        else
        {
            int currentPriorityIndex(CB_ERR);

            for (PriorityList::const_iterator pIt(priorities.begin()); pIt != priorities.end(); pIt++)
            {
                int item(m_priorityCombo.AddString(m_priorityParameter.getPriorityLabel(*pIt).c_str()));
                m_priorityCombo.SetItemData(item, *pIt);

                // See if the given priority matches the one currently configured
                // for this step. We'll want to select this item.
                if (*pIt == m_priorityParameter.getPriority())
                {
                    currentPriorityIndex = item;
                }
            }

            if (currentPriorityIndex == CB_ERR)
            {
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << m_priorityParameter.getPriority();
                userMsg.showMsgBox(IDS_UE_210106);
            }
            else
            {
                SetComboSelection(m_priorityCombo, currentPriorityIndex);
            }
        }
    }

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


void CPlanStepEditStationTisPage::SetStationLibraryVersion()
{
    FUNCTION_ENTRY( "SetStationLibraryVersion" );

    TA_ASSERT(::IsWindow(GetSafeHwnd()), "Dialog not initialised");

    m_stationLibraryVersion = UNDEFINED_LIBRARY_VERSION;
    m_pickMessageBtn.EnableWindow(FALSE);

    // Compile a list of target station names. If the station button is checked, this is limited
    // to the single selected station. If the ALL stations button is checked, the list will contain
    // all TIS-enabled stations.
    typedef std::vector<std::string> StationList;
    StationList targetStations;

    if (m_stationBtn.GetCheck() == BST_CHECKED)
    {
        CString station;
        m_stationCombo.GetWindowText(station);

        if (station.IsEmpty())
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "No station selected from the list");

            FUNCTION_EXIT;
            return;
        }

        targetStations.push_back(static_cast<LPCTSTR>(station));
    }
    else if (m_allStationsBtn.GetCheck() == BST_CHECKED)
    {
        for (int i(0); i < m_stationCombo.GetCount(); i++)
        {
            CString station;
            m_stationCombo.GetLBText(i, station);

            targetStations.push_back(static_cast<LPCTSTR>(station));
        }
    }

    // Iterate over the target stations and ensure they are all using the same version of the
    // message library. Otherwise, we can't guarantee that a given message ID will denote the
    // same message across different libraries.
    CWaitCursor waitCursor;
    unsigned short newLibraryVersion(UNDEFINED_LIBRARY_VERSION);

    for (StationList::const_iterator sIt(targetStations.begin()); sIt != targetStations.end(); sIt++)
    {
        try
        {
            TA_Base_Bus::TISAgentAccessFactory::StisNamedObj& stisAgent =
                TA_Base_Bus::TISAgentAccessFactory::getInstance().getSTISAgentAtLocation(*sIt);

            unsigned short stationLibraryVersion;
            CORBA_CALL_RETURN( stationLibraryVersion, stisAgent, getCurrentSTISMessageLibraryVersion, () );//limin

            if (newLibraryVersion == UNDEFINED_LIBRARY_VERSION)
            {
                newLibraryVersion = stationLibraryVersion;
            }

            // If one of the stations has a different message library, message selection
            // will not be allowed.
            if (newLibraryVersion != stationLibraryVersion)
            {
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UW_210027);

                FUNCTION_EXIT;
                return;
            }
        }
        catch (const TA_Base_Core::TransactiveException& ex)
        {
            std::ostringstream error;
            error << "Failed to retrieve STIS message library version from " << *sIt
                  << " (" << ex.what() << ")";

            LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", error.str().c_str());

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << *sIt;
            userMsg.showMsgBox(IDS_UE_210110);

            FUNCTION_EXIT;
            return;
        }
        catch (...)
        {
            std::ostringstream error;
            error << "Failed to retrieve STIS message library version from " << *sIt;

            LOG_EXCEPTION_CATCH(SourceInfo, " Unknown Exception", error.str().c_str());

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << *sIt;
            userMsg.showMsgBox(IDS_UE_210110);

            FUNCTION_EXIT;
            return;
        }
    }

    m_stationLibraryVersion = newLibraryVersion;

    // Must have a valid library to be able to pick messages.
    if (m_stationLibraryVersion != UNDEFINED_LIBRARY_VERSION)
    {
        m_pickMessageBtn.EnableWindow(TRUE);

        // Make sure we're displaying a valid message for the given station library version.
        // If the current message is sourced from a different library, then advise the
        // operator to pick one from the correct message library.
        TA_Base_Core::TISMessage currentMessage(m_messageParameter.getMessage());

        if (currentMessage.messageId != TA_Base_Core::TIS_UNDEFINED_MESSAGE_ID &&
            currentMessage.libraryVersion != m_stationLibraryVersion)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UW_210029);
        }
    }

    FUNCTION_EXIT;
}


void CPlanStepEditStationTisPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditStationTisPage)
    DDX_Control(pDX, IDC_PID_LIST, m_pidList);
    DDX_Control(pDX, IDC_DURATION_PICKER, m_durationPicker);
    DDX_Control(pDX, IDC_MESSAGE_EDIT, m_messageEdit);
    DDX_Control(pDX, IDC_PRIORITY_COMBO, m_priorityCombo);
    DDX_Control(pDX, IDC_STATION_COMBO, m_stationCombo);
    DDX_Control(pDX, IDC_STATION_LEVEL_COMBO, m_stationLevelCombo);
    DDX_Control(pDX, IDC_STATION_RADIO, m_stationBtn);
    DDX_Control(pDX, IDC_ALL_STATIONS_RADIO, m_allStationsBtn);
    DDX_Control(pDX, IDC_STATION_LEVEL_CHECK, m_stationLevelBtn);
    DDX_Control(pDX, IDC_PID_LIST_CHECK, m_pidListBtn);
    DDX_Control(pDX, IDC_PICK_PID_BUTTON, m_pickPidBtn);
    DDX_Control(pDX, IDC_PICK_MESSAGE_BUTTON, m_pickMessageBtn);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditStationTisPage message handlers

BEGIN_MESSAGE_MAP(CPlanStepEditStationTisPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditStationTisPage)
    ON_CBN_SELCHANGE(IDC_STATION_COMBO, OnStationComboSelectionChanged)
    ON_CBN_SELCHANGE(IDC_PRIORITY_COMBO, OnPriorityComboSelectionChanged)
    ON_BN_CLICKED(IDC_STATION_RADIO, OnStationRadio)
    ON_BN_CLICKED(IDC_ALL_STATIONS_RADIO, OnAllStationsRadio)
    ON_BN_CLICKED(IDC_STATION_LEVEL_CHECK, OnStationLevelCheck)
    ON_BN_CLICKED(IDC_PID_LIST_CHECK, OnPidListCheck)
    ON_BN_CLICKED(IDC_PICK_PID_BUTTON, OnPickPidButton)
    ON_BN_CLICKED(IDC_PICK_MESSAGE_BUTTON, OnPickMessageButton)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CPlanStepEditStationTisPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CPlanStepEditParameterPage::OnInitDialog();

    // Populate the station list. The contents will remain unchanged.
    TisConfigAccess::TisStationNameSet stations(TisConfigAccess::getSTisStations());

    if (stations.empty())
    {
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg.showMsgBox(IDS_UE_210107);
    }
    else
    {
        for (TisConfigAccess::TisStationNameSet::const_iterator sIt(stations.begin()); sIt != stations.end(); sIt++)
        {
            m_stationCombo.AddString(sIt->c_str());
        }
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
        m_stationLevelBtn.EnableWindow(FALSE);
        m_pidListBtn.EnableWindow(FALSE);

        SetButtonState(m_stationLevelBtn, BST_UNCHECKED);
        SetButtonState(m_pidListBtn, BST_UNCHECKED);
    }
    else
    {
        m_stationLevelBtn.EnableWindow(TRUE);
        m_pidListBtn.EnableWindow(TRUE);

        // Re-set the buttons to their current check state. This effectively invokes the respective
        // BN_CLICKED handlers and updates the station level combo and the PID list controls.
        SetButtonState(m_stationLevelBtn, m_stationLevelBtn.GetCheck());
        SetButtonState(m_pidListBtn, m_pidListBtn.GetCheck());
    }

    // If a valid station is selected (or All Stations is active), we need to determine the version
    // of the message library in use and immediately inform the operator of any problems.
    SetStationLibraryVersion();

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


void CPlanStepEditStationTisPage::OnStationLevelCheck()
{
    FUNCTION_ENTRY( "OnStationLevelCheck" );

    if (m_stationLevelBtn.GetCheck() == BST_CHECKED)
    {
        SetButtonState(m_pidListBtn, BST_UNCHECKED);

         m_stationLevelCombo.EnableWindow(TRUE);
        m_stationLevelCombo.ResetContent();

        // Populate the station level combo with station levels corresponding to the
        // selected station. If no station is selected, the combo is populated with
        // the levels for ALL TIS-enabled stations.
        CString selectedStation;
        m_stationCombo.GetWindowText(selectedStation); // may be an empty string

        TisConfigAccess::TisStationLevelSet levels(TisConfigAccess::getSTisLevels(static_cast<LPCTSTR>(selectedStation)));

        for (TisConfigAccess::TisStationLevelSet::const_iterator lIt(levels.begin()); lIt != levels.end(); lIt++)
        {
            m_stationLevelCombo.AddString(lIt->c_str());
        }
    }
    else
    {
        m_stationLevelCombo.EnableWindow(FALSE);
        m_stationLevelCombo.ResetContent();
    }

    FUNCTION_EXIT;
}


void CPlanStepEditStationTisPage::OnPidListCheck()
{
    FUNCTION_ENTRY( "OnPidListCheck" );

    if (m_pidListBtn.GetCheck() == BST_CHECKED)
    {
        SetButtonState(m_stationLevelBtn, BST_UNCHECKED);

        m_pickPidBtn.EnableWindow(TRUE);
        SetListEnableState(m_pidList, TRUE);
    }
    else
    {
        m_pickPidBtn.EnableWindow(FALSE);
        SetListEnableState(m_pidList, FALSE);
    }

    m_pidListParameter.resetPids();
    UpdatePidList();

    FUNCTION_EXIT;
}


void CPlanStepEditStationTisPage::OnPickPidButton()
{
    FUNCTION_ENTRY( "OnPickPidButton" );

    CString stationName;
    m_stationCombo.GetWindowText(stationName);

    if (m_pidListParameter.editParameter(static_cast<LPCTSTR>(stationName)))
    {
        UpdatePidList();
    }

    FUNCTION_EXIT;
}


void CPlanStepEditStationTisPage::OnPickMessageButton()
{
    FUNCTION_ENTRY( "OnPickMessageButton" );

    // The destination station library version should be valid for message selection button to become enabled.
    TA_ASSERT(m_stationLibraryVersion != UNDEFINED_LIBRARY_VERSION, "Invalid destination library version");

    TisConfigAccess::PredefinedTisMessage selectedMessage;

    if (m_messageParameter.editParameter(m_stationLibraryVersion, selectedMessage))
    {
        TA_ASSERT(selectedMessage.messageTag != TA_Base_Core::TIS_UNDEFINED_MESSAGE_ID, "Invalid TIS message");

        TA_Base_Core::TISMessage newMessage;
        newMessage.libraryVersion = m_stationLibraryVersion;
        newMessage.librarySection = selectedMessage.librarySection;
        newMessage.messageId = selectedMessage.messageTag;

        m_messageParameter.setMessage(newMessage);
        m_priorityParameter.setValue(selectedMessage.priority);

        m_messageEdit.SetWindowText(m_messageParameter.getMessageText().c_str());
        UpdatePriorityCombo();
    }

    FUNCTION_EXIT;
}


void CPlanStepEditStationTisPage::OnPriorityComboSelectionChanged()
{
    FUNCTION_ENTRY( "OnPriorityComboSelectionChanged" );

    int selectedPriority(m_priorityCombo.GetCurSel());

    if (selectedPriority == CB_ERR)
    {
        m_priorityParameter.hideValue(true);
    }
    else
    {
        if (GetPlanStep()->getParentPlan()->isCopiedPlan())
        {
            m_priorityParameter.setValue(m_priorityCombo.GetItemData(selectedPriority));
        }
        m_priorityParameter.hideValue(false);
    }

    FUNCTION_EXIT;
}


#endif // defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM)

