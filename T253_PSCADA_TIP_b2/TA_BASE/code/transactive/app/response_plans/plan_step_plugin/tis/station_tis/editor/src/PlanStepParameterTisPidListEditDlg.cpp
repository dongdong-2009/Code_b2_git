/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepParameterTisPidListEditDlg.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Dialog for selecting a set of PIDs for the Station TIS step.
  *
  **/

#include "StdAfx.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "resource.h"
#include "PlanStepParameterTisPidListEditDlg.h"
#include "TisPidListStepParameter.h"
#include "TisConfigAccess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterTisPidListEditDlg dialog

CPlanStepParameterTisPidListEditDlg::CPlanStepParameterTisPidListEditDlg(const std::string& stationName, TisPidListStepParameter& parameter, bool canEdit) :
    CPlanStepParameterEditDlg(parameter, CPlanStepParameterTisPidListEditDlg::IDD, canEdit),
    m_stationName(stationName),
    m_pidListParameter(parameter),
    m_parameterName(_T(parameter.name().c_str()))
{
    FUNCTION_ENTRY( "CPlanStepParameterTisPidListEditDlg" );

    // {{AFX_DATA_INIT(CPlanStepParameterTisPidListEditDlg)
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CPlanStepParameterTisPidListEditDlg::~CPlanStepParameterTisPidListEditDlg()
{
    FUNCTION_ENTRY( "~CPlanStepParameterTisPidListEditDlg" );
    FUNCTION_EXIT;
}


std::vector<int> CPlanStepParameterTisPidListEditDlg::GetSelectedListItems(const CListBox& list) const
{
    FUNCTION_ENTRY( "GetSelectedListItems" );

    TA_ASSERT(::IsWindow(GetSafeHwnd()), "Dialog not initialised");

    std::vector<int> selectedItems;
    int selectedCount(list.GetSelCount());

    if (selectedCount > 0)
    {
        int* selectedItemArray = new int[selectedCount];
        selectedItems.reserve(selectedCount);

        list.GetSelItems(selectedCount, selectedItemArray);

        for (int i(0); i < selectedCount; i++)
        {
            selectedItems.push_back(selectedItemArray[i]);
        }

        delete[] selectedItemArray;
    }

    FUNCTION_EXIT;
    return selectedItems;

}


void CPlanStepParameterTisPidListEditDlg::TransferSelectedListItems(CListBox& sourceList, CListBox& destinationList)
{
    FUNCTION_ENTRY( "TransferSelectedListItems" );

    TA_ASSERT(::IsWindow(GetSafeHwnd()), "Dialog not initialised");

    CWaitCursor waitCursor;

    std::vector<int> selectedItems(GetSelectedListItems(sourceList));

    for (std::vector<int>::reverse_iterator itemIt(selectedItems.rbegin()); itemIt != selectedItems.rend(); itemIt++)
    {
        CString text;

        sourceList.GetText(*itemIt, text);
        sourceList.DeleteString(*itemIt);
        destinationList.AddString(text);
    }

    FUNCTION_EXIT;
}


void CPlanStepParameterTisPidListEditDlg::TransferAllListItems(CListBox& sourceList, CListBox& destinationList)
{
    FUNCTION_ENTRY( "TransferAllListItems" );

    TA_ASSERT(::IsWindow(GetSafeHwnd()), "Dialog not initialised");

    CWaitCursor waitCursor;

    for (int item(sourceList.GetCount() - 1); item >= 0; item--)
    {
        CString text;

        sourceList.GetText(item, text);
        sourceList.DeleteString(item);
        destinationList.AddString(text);
    }

    FUNCTION_EXIT;
}


void CPlanStepParameterTisPidListEditDlg::DeselectAllListItems(CListBox& list)
{
    FUNCTION_ENTRY( "DeselectAllListItems" );

    TA_ASSERT(::IsWindow(GetSafeHwnd()), "Dialog not initialised");

    std::vector<int> selectedItems(GetSelectedListItems(list));

    for (std::vector<int>::const_iterator itemIt(selectedItems.begin()); itemIt != selectedItems.end(); itemIt++)
    {
        list.SetSel(*itemIt, FALSE);
    }

    FUNCTION_EXIT;
}


void CPlanStepParameterTisPidListEditDlg::SetPidAddRemoveButtonEnableState()
{
    FUNCTION_ENTRY( "SetPidAddRemoveButtonEnableState" );

    TA_ASSERT(::IsWindow(GetSafeHwnd()), "Dialog not initialised");

    m_addSelectionBtn.EnableWindow(m_availablePidList.GetSelCount() > 0);
    m_removeSelectionBtn.EnableWindow(m_selectedPidList.GetSelCount() > 0);
    m_addAllBtn.EnableWindow(m_availablePidList.GetCount() > 0);
    m_removeAllBtn.EnableWindow(m_selectedPidList.GetCount() > 0);

    FUNCTION_EXIT;
}


void CPlanStepParameterTisPidListEditDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepParameterEditDlg::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepParameterTisPidListEditDlg)
    DDX_Control(pDX, IDC_STATIONTIS_SELECTED_PID_LIST, m_selectedPidList);
    DDX_Control(pDX, IDC_STATIONTIS_REMOVE_SELECTION_BUTTON, m_removeSelectionBtn);
    DDX_Control(pDX, IDC_STATIONTIS_REMOVE_ALL_BUTTON, m_removeAllBtn);
    DDX_Control(pDX, IDC_STATIONTIS_AVAILABLE_PID_LIST, m_availablePidList);
    DDX_Control(pDX, IDC_STATIONTIS_ADD_SELECTION_BUTTON, m_addSelectionBtn);
    DDX_Control(pDX, IDC_STATIONTIS_ADD_ALL_BUTTON, m_addAllBtn);
    DDX_Text(pDX, IDC_STATIONTIS_PARAMETER_NAME, m_parameterName);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterTisPidListEditDlg message handlers

BEGIN_MESSAGE_MAP(CPlanStepParameterTisPidListEditDlg, CPlanStepParameterEditDlg)
    // {{AFX_MSG_MAP(CPlanStepParameterTisPidListEditDlg)
    ON_BN_CLICKED(IDC_STATIONTIS_REMOVE_ALL_BUTTON, OnRemoveAllButton)
    ON_BN_CLICKED(IDC_STATIONTIS_REMOVE_SELECTION_BUTTON, OnRemoveSelectionButton)
    ON_BN_CLICKED(IDC_STATIONTIS_ADD_ALL_BUTTON, OnAddAllButton)
    ON_BN_CLICKED(IDC_STATIONTIS_ADD_SELECTION_BUTTON, OnAddSelectionButton)
    ON_LBN_SETFOCUS(IDC_STATIONTIS_AVAILABLE_PID_LIST, OnAvailablePidListSetFocus)
    ON_LBN_SETFOCUS(IDC_STATIONTIS_SELECTED_PID_LIST, OnSelectedPidListSetfocus)
    ON_LBN_SELCHANGE(IDC_STATIONTIS_AVAILABLE_PID_LIST, OnPidListSelectionChanged)
    ON_LBN_SELCHANGE(IDC_STATIONTIS_SELECTED_PID_LIST, OnPidListSelectionChanged)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CPlanStepParameterTisPidListEditDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CPlanStepParameterEditDlg::OnInitDialog();

    CWaitCursor waitCursor;

    // Retrieve from the database all station TIS PIDs.
    TisConfigAccess::TisPidSet availablePids(TisConfigAccess::getSTisPids(m_stationName));

    if (availablePids.empty())
    {
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg.showMsgBox(IDS_UE_210108);

        EndDialog(IDABORT);

        FUNCTION_EXIT;
        return TRUE;
    }

    // Preallocate storage for the PID lists.
    const int AVERAGE_PID_LENGTH(40);
    m_availablePidList.InitStorage(availablePids.size(), AVERAGE_PID_LENGTH);
    m_selectedPidList.InitStorage(availablePids.size(), AVERAGE_PID_LENGTH);

    // Take out from the available PIDs those that are already selected.
    // While we're iterating over the selected PIDs, we can populate the relevant list box.
	std::vector<std::string> selectedPids(m_pidListParameter.getPids());
	for (std::vector<std::string>::iterator itSelectedPid = selectedPids.begin(); itSelectedPid != selectedPids.end(); ++itSelectedPid)
    {
        TisConfigAccess::TisPidSet::iterator pidIt(availablePids.find(*itSelectedPid));

        if (pidIt != availablePids.end())
        {
            m_selectedPidList.AddString(pidIt->c_str()); // valid PID, ok to add to Selected PIDs box
            availablePids.erase(pidIt);
        }
    }

    // Populate the available PID list.
    for (TisConfigAccess::TisPidSet::const_iterator pidIt(availablePids.begin()); pidIt != availablePids.end(); pidIt++)
    {
        m_availablePidList.AddString(pidIt->c_str());
    }

    // Enable/disable PID transfer buttons as appropriate.
    SetPidAddRemoveButtonEnableState();

    // Check whether all of the currently selected PIDs are valid.
    if (static_cast<unsigned long>(m_selectedPidList.GetCount()) < selectedPids.size())
    {
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg.showMsgBox(IDS_UW_210023);
    }

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CPlanStepParameterTisPidListEditDlg::OnAvailablePidListSetFocus()
{
    FUNCTION_ENTRY( "OnAvailablePidListSetFocus" );

    // Deselect items in the Selected PIDs list
    DeselectAllListItems(m_selectedPidList);

    FUNCTION_EXIT;
}


void CPlanStepParameterTisPidListEditDlg::OnSelectedPidListSetfocus()
{
    FUNCTION_ENTRY( "OnSelectedPidListSetfocus" );

    // Deselect items in the Available PIDs list
    DeselectAllListItems(m_availablePidList);

    FUNCTION_EXIT;
}


void CPlanStepParameterTisPidListEditDlg::OnPidListSelectionChanged()
{
    FUNCTION_ENTRY( "OnPidListSelectionChanged" );

    // Enable/disable PID transfer buttons as appropriate.
    SetPidAddRemoveButtonEnableState();

    FUNCTION_EXIT;
}


void CPlanStepParameterTisPidListEditDlg::OnAddSelectionButton()
{
    FUNCTION_ENTRY( "OnAddSelectionButton" );

    // Move the highlighted PIDs from the Available PID list to the Selected PID list.
    TransferSelectedListItems(m_availablePidList, m_selectedPidList);

    // Enable/disable PID transfer buttons as appropriate.
    SetPidAddRemoveButtonEnableState();

    FUNCTION_EXIT;
}


void CPlanStepParameterTisPidListEditDlg::OnRemoveSelectionButton()
{
    FUNCTION_ENTRY( "OnRemoveSelectionButton" );

    // Move the highlighted PIDs from the Selected PID list back to the Available PID list.
    TransferSelectedListItems(m_selectedPidList, m_availablePidList);

    // Enable/disable PID transfer buttons as appropriate.
    SetPidAddRemoveButtonEnableState();

    FUNCTION_EXIT;
}


void CPlanStepParameterTisPidListEditDlg::OnAddAllButton()
{
    FUNCTION_ENTRY( "OnAddAllButton" );

    // Move all PIDs from the Available PID list to the Selected PID list.
    TransferAllListItems(m_availablePidList, m_selectedPidList);

    // Enable/disable PID transfer buttons as appropriate.
    SetPidAddRemoveButtonEnableState();

    FUNCTION_EXIT;
}


void CPlanStepParameterTisPidListEditDlg::OnRemoveAllButton()
{
    FUNCTION_ENTRY( "OnRemoveAllButton" );

    // Move all PIDs from the Selected PID list back to the Available PID list.
    TransferAllListItems(m_selectedPidList, m_availablePidList);

    // Enable/disable PID transfer buttons as appropriate.
    SetPidAddRemoveButtonEnableState();

    FUNCTION_EXIT;
}


void CPlanStepParameterTisPidListEditDlg::OnOK()
{
    FUNCTION_ENTRY( "OnOK" );

    m_selectedPids.clear();

    for (int item(0); item < m_selectedPidList.GetCount(); item++)
    {
        CString pid;

        m_selectedPidList.GetText(item, pid);
        m_selectedPids.push_back(static_cast<LPCTSTR>(pid));
    }

    CPlanStepParameterEditDlg::OnOK();

    FUNCTION_EXIT;
}
