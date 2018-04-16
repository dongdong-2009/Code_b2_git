//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepParameterMonitorEditDlg.cpp $
// @author:  Katherine Thomson
// @version: $Revision: #3 $
//
// Last modification: $DateTime: 2009/02/10 15:59:52 $
// Last modified by:  $Author: builder $
//
// Dialog for selecting a monitor name for the Assign Camera To Monitor step.

#include "stdafx.h"

#if defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR)

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/cctv/video_switch_agent/agent_access/src/VideoSwitchAgentFactory.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoMonitorCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoOutputGroupCorbaDef.h"
#include "bus/cctv/video_switch_agent/src/EntityTypeConstants.h"
#include "PlanManager.h"
#include "PlanStepParameterMonitorEditDlg.h"
#include "MonitorStepParameter.h"
#include "PlanStep.h"
#include "PlanNode.h"
#include "MessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;

namespace
{
    const std::string ERROR_MSG = "Failed to get the monitors from the VideoSwitchAgent.";
}


/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterMonitorEditDlg dialog

CPlanStepParameterMonitorEditDlg::CPlanStepParameterMonitorEditDlg(MonitorStepParameter& parameter, bool canEdit)
    : CPlanStepParameterEditDlg(parameter, CPlanStepParameterMonitorEditDlg::IDD, canEdit),
      m_monitorParameter( parameter )
{
    FUNCTION_ENTRY( "CPlanStepParameterMonitorEditDlg" );

    // {{AFX_DATA_INIT(CPlanStepParameterMonitorEditDlg)
    m_monitorName = _T("");
    m_parameterName = _T("");
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void CPlanStepParameterMonitorEditDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepParameterEditDlg::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepParameterMonitorEditDlg)
    DDX_Control(pDX, IDOK, m_okButton);
    DDX_Control(pDX, IDC_MONITOR_TREE, m_monitorTree);
    DDX_Text(pDX, IDC_MONITOR_NAME, m_monitorName);
    DDX_Text(pDX, IDC_PARAMETER_NAME, m_parameterName);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepParameterMonitorEditDlg, CPlanStepParameterEditDlg)
    // {{AFX_MSG_MAP(CPlanStepParameterMonitorEditDlg)
    ON_NOTIFY(TVN_SELCHANGED, IDC_MONITOR_TREE, onSelectionChangedMonitorTree)
    ON_BN_CLICKED(IDOK, onOK)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterMonitorEditDlg message handlers

BOOL CPlanStepParameterMonitorEditDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CPlanStepParameterEditDlg::OnInitDialog();

    CWaitCursor waitCursor;

    m_monitorName = m_monitorParameter.getMonitorName().c_str();
    m_parameterName = m_monitorParameter.getName().c_str();
    UpdateData( FALSE );
    m_monitorKey = m_monitorParameter.getMonitorKey();

    // Disable the OK button until a monitor has been selected.

    if ( m_monitorName.IsEmpty() )
    {
        m_okButton.EnableWindow( FALSE );
    }

       loadMonitorTree();

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CPlanStepParameterMonitorEditDlg::loadMonitorTree()
{
    FUNCTION_ENTRY( "loadMonitorTree" );

    TA_Base_Bus::VideoSwitchAgentFactory::VideoOutputGroupVector groups;
    try
    {
        groups = TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getVideoOutputGroups();
    }
    catch( const TA_Base_Core::ObjectResolutionException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "ObjectResolutionException", e.what() );
        CString actionName, agent;
        actionName.LoadString(IDS_RETRIEVE_MONITORS);
        agent.LoadString(IDS_SCHEDULING_AGENT);
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << actionName << agent;
        userMsg.showMsgBox(IDS_UE_030064);
    }
    catch( const TA_Base_Bus::VideoSwitchAgentException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", static_cast< const char* >( e.what ) );
        LOGMORE( SourceInfo, ERROR_MSG.c_str() );
        CString actionName, agent;
        actionName.LoadString(IDS_RETRIEVE_MONITORS);
        agent.LoadString(IDS_SCHEDULING_AGENT);
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << actionName << agent;
        userMsg.showMsgBox(IDS_UE_030064);
    }
    catch( const CORBA::Exception& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
        MessageBox::error(IDS_RETRIEVE_MONITORS);
    }
    catch( const TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        MessageBox::error(IDS_RETRIEVE_MONITORS, e);
    }
    catch( ... )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Caught unknown exception" );
        MessageBox::error(IDS_RETRIEVE_MONITORS);
    }

    HTREEITEM rootItem;
    std::string groupName = "";
    std::string monitorName = "";
    unsigned long monitorKey = 0;
    HTREEITEM monitorItem;
    int groupFailures = 0;
    int monitorFailures = 0;

    // This will be set to the item for the monitor that is currently stored in the MonitorStepParameter.
    // It will be identified using m_initialMonitorKey and it will be automatically selected.

    HTREEITEM* initialMonitorItem = 0;

    TA_Base_Bus::VideoSwitchAgentFactory::VideoOutputGroupVector::iterator it;
    for( it = groups.begin(); it != groups.end(); it++ )
    {
        // Check that the group contains VideoMonitors. At the moment
        // a group can only consist of all the same type of output as specified
        // by the constants in app/cctv/video_switch_agent/src/EntityTypeConstants.h.

        try
        {
            std::string groupType;
            CORBA_CALL_RETURN( groupType, ( *( *it ) ), getGroupType, () );//limin
            if ( TA_Base_App::TA_Visual::VIDEO_MONITOR != groupType )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Group does not contain VideoMonitors." );
                continue;   // Wrong type, not interested.
            }
            CORBA_CALL_RETURN( groupName, ( *( *it ) ), getName, () );//limin
            if ( groupName.empty() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Group name is empty." );
                groupFailures++;
                continue;   // Invalid group name, not interested.
            }
        }
        catch( TA_Base_Bus::VideoSwitchAgentException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", static_cast< const char* >( e.what ) );
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Failed to determine group type or name." );
            groupFailures++;
            continue;    // Failed to get group type or name.
        }

        // Check that we can get the VideoOutputs in this group.

        TA_Base_Bus::VideoOutputGroupCorbaDef::VideoOutputSequence* videoOutputs;
        try
        {
            CORBA_CALL_RETURN( videoOutputs, ( *( *it ) ), getVideoOutputList, () );//limin
            if ( 0 == videoOutputs )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Failed to get the monitors for group: %s.", groupName.c_str() );
                groupFailures++;
                continue;  // Failed to get vos.
            }
        }
        catch( TA_Base_Bus::VideoSwitchAgentException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", static_cast< const char* >( e.what ) );
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Failed to get the monitors for group: %s.", groupName.c_str() );
            groupFailures++;
            continue;  // Failed to get vos.
        }

        // Insert a root item for this group, but only if it contains some VideoOutputs.

        if ( 0 >= videoOutputs->length() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "Not inserting group name because it has no children: %s", groupName.c_str() );
            continue;
        }

        rootItem = m_monitorTree.InsertItem( groupName.c_str(), 0, 0, TVI_ROOT, TVI_LAST );
        if ( NULL == rootItem )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Failed to insert group: %s.", groupName.c_str() );
            groupFailures++;
            continue; // Failed to insert group.
        }

        // For each monitor in this group - insert an item.

        for( unsigned int i = 0; i < videoOutputs->length(); i++ )
        {
            try
            {
                monitorName = (*videoOutputs)[i]->getName();
                monitorKey = (*videoOutputs)[i]->getKey();
            }
            catch( TA_Base_Bus::VideoSwitchAgentException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", static_cast< const char* >( e.what ) );
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Failed to get monitor details.");
                monitorFailures++;
                continue;
            }
            if ( monitorName.empty() || 0 == monitorKey )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Failed to get monitor details.");
                monitorFailures++;
                continue;
            }

            // Insert the monitor's name into the tree and put the entity key in as the item data.

            monitorItem = m_monitorTree.InsertItem( monitorName.c_str(), 2 ,2, rootItem, TVI_LAST );
            if ( NULL == monitorItem )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Failed to insert monitor: %s.", monitorName.c_str());
                monitorFailures++;
            }
            m_monitorTree.SetItemData( monitorItem, monitorKey );

            // Do this in here because it is faster than trying to find the item again.

            if ( monitorKey == m_monitorKey )
            {
                initialMonitorItem = &monitorItem;
            }
        }

        // Remove the group name if none of the children could be inserted.

        if ( !m_monitorTree.ItemHasChildren( rootItem ) )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "Removing group name because it has no children: %s", groupName.c_str() );
            m_monitorTree.DeleteItem( rootItem );
        }
    }

    // Sort the whole tree.

    m_monitorTree.SortChildren( NULL );

    // Display an error message if there were errors loading the list.

    if ( 0 != groupFailures || 0 != monitorFailures )
    {
        TA_Base_Bus::TransActiveMessage userMsg;
        unsigned int number = (0 != groupFailures) ? groupFailures : monitorFailures;
        CString itemName;
        itemName.LoadString( (0 != groupFailures) ? IDS_GROUPS : IDS_MONITORS);
        userMsg << number << itemName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_210083);
    }

    // When the bit below this works, we can take this bit out...

    if ( m_monitorName.IsEmpty() )
    {
        m_okButton.EnableWindow( FALSE );
    }
    else
    {
        m_okButton.EnableWindow( TRUE );
    }

/* KTTODO THIS DOESN'T WORK AT THE MOMENT.

    // Select the current monitor (if there is one and we found it).
    // It's faster to do it here while we already know the item.

    if ( 0 != m_monitorKey )
    {
        if ( ( 0 == initialMonitorItem ) || ( 0 == m_monitorTree.Select( *initialMonitorItem, TVGN_FIRSTVISIBLE ) ) )
        {
            std::stringstream error;
            if ( m_monitorName.IsEmpty() )
            {
                error << "Failed to select current Monitor (name unknown).";
            }
            else
            {
                error << "Failed to select current Monitor " << m_monitorName << ".";
            }
            MessageBox::error( error.str().c_str() );
        }
        else
        {
            // Now we've selected a monitor, so enable the OK button.

            m_okButton.EnableWindow( TRUE );
        }
    }
*/
    // Finally delete all the named obj refs, because we don't need them anymore.

    for ( it = groups.begin(); it != groups.end(); it++ )
    {
        delete *it;
        *it = 0;
    }

    FUNCTION_EXIT;
}


void CPlanStepParameterMonitorEditDlg::onSelectionChangedMonitorTree(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "onSelectionChangedMonitorTree" );

    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

    // Get currently selected item.

    HTREEITEM item = m_monitorTree.GetSelectedItem();
    if ( NULL == item )
    {
//        MessageBox::error( "Failed to identify selected monitor." );
        m_monitorName = "";
    }
    else
    {
        // If the key is not zero, then we have a monitor, otherwise it is a group name.

        unsigned long itemKey =  m_monitorTree.GetItemData( item );
        if ( 0 != itemKey )
        {
            m_monitorName = m_monitorTree.GetItemText( item );
            m_monitorKey = itemKey;

            // Now we've selected a monitor, so enable the OK button.

            m_okButton.EnableWindow( TRUE );
        }
    }
    UpdateData( FALSE );

    *pResult = 0;

    FUNCTION_EXIT;
}


void CPlanStepParameterMonitorEditDlg::onOK()
{
    FUNCTION_ENTRY( "onOK" );

    m_monitorParameter.setMonitorName( std::string( m_monitorName ) );
    m_monitorParameter.setMonitorKey( m_monitorKey );

    CPlanStepParameterEditDlg::OnOK();

    FUNCTION_EXIT;
}


#endif // defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR)
