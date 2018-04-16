/**
  * The source code in this file is the property of
  * MI Consulting Group (Australia) and is not for
  * redistribution in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/RecordingUnitDlg.cpp $
  * @author Matthew Atkinson
  * @version $Revision: #1 $
  * Last Modification: $DateTime: 2012/06/12 13:35:44 $
  * Last Modified by:
  *
  * The RecordingUnitDlg class allows the user to record, play, rewind, fast forward,
  * and pause camera or sequence images. This class is not suitable for sub-classing.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif

// System includes.
#include "stdafx.h"

// Application includes.
#include "RecordingUnitDlg.h"
#include "bus/generic_gui/src/TransActiveMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// RecordingUnitDlg dialog

RecordingUnitDlg::RecordingUnitDlg( CString& recUnitName, short recordingUnitID, CString& monitorName,
                                      short monitorID, CWnd* pParent /*=NULL*/ )
    : CDialog( RecordingUnitDlg::IDD, pParent )
{
    FUNCTION_ENTRY( "RecordingUnitDlg" );

    // {{AFX_DATA_INIT(RecordingUnitDlg)
    m_recordingUnitName = _T("");
    m_monitorName = _T("");
    // }}AFX_DATA_INIT

    // Initialise private data.
    m_recordingUnitName = recUnitName;
    m_recordingUnitID = recordingUnitID;
    m_monitorName = monitorName;
    m_monitorID = monitorID;

    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32.
    m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );

    FUNCTION_EXIT;
}


/**
  * LoadButtonIcons.
  *
  * Load all the controller button icons.
  */

void RecordingUnitDlg::LoadButtonIcons()
{
    FUNCTION_ENTRY( "LoadButtonIcons" );

    // Load check box icons.
    m_stopIcon = AfxGetApp()->LoadIcon( IDI_HALT_ICON );
    m_recordIcon = AfxGetApp()->LoadIcon( IDI_RECORD_ICON );
    m_rewindIcon = AfxGetApp()->LoadIcon( IDI_PREVIOUSELEMENT_RW_ICON );
    m_playIcon = AfxGetApp()->LoadIcon( IDI_PLAY_ICON );
    m_fastForwardIcon = AfxGetApp()->LoadIcon( IDI_NEXTELEMENT_FF_ICON );
    m_pauseIcon = AfxGetApp()->LoadIcon( IDI_PAUSE_ICON );

    // Set the check box icons
    m_stop.SetIcon( m_stopIcon );
    m_record.SetIcon( m_recordIcon );
    m_rewind.SetIcon( m_rewindIcon );
    m_play.SetIcon( m_playIcon );
    m_fastForward.SetIcon( m_fastForwardIcon );
    m_pause.SetIcon( m_pauseIcon );

    FUNCTION_EXIT;
}


/**
  * SetToolTips.
  *
  * Load all the tool tips for the dialog controller buttons.
  */

void RecordingUnitDlg::SetToolTips()
{
    FUNCTION_ENTRY( "SetToolTips" );

    m_ctlToolTip.Create( this );
    m_ctlToolTip.SetFont(GetFont());
    m_ctlToolTip.AddWindowTool(GetDlgItem( IDC_RU_STOP_CHECK ), "Stop" );
    m_ctlToolTip.AddWindowTool(GetDlgItem( IDC_RU_RECORD_CHECK ), "Record" );
    m_ctlToolTip.AddWindowTool(GetDlgItem( IDC_RU_PLAY_CHECK ), "Play" );

    // Set the tool tip depending on the recording unit type.
    if ( m_recordingUnitName.Find( "DVD" ) >= 1 )
    {
        m_ctlToolTip.AddWindowTool(GetDlgItem( IDC_RU_REWIND_CHECK ), "Review" );
        m_ctlToolTip.AddWindowTool(GetDlgItem( IDC_RU_FAST_FORWARD_CHECK ), "Cue" );
    }
    else
    {
        m_ctlToolTip.AddWindowTool(GetDlgItem( IDC_RU_REWIND_CHECK ), "Rewind" );
        m_ctlToolTip.AddWindowTool(GetDlgItem( IDC_RU_FAST_FORWARD_CHECK ), "Fast Forward" );
    }

    m_ctlToolTip.AddWindowTool( GetDlgItem( IDC_RU_PAUSE_CHECK ), "Pause" );

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
void RecordingUnitDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(RecordingUnitDlg)
    DDX_Text(pDX, IDC_RU_RECORDINGUNIT_EDIT, m_recordingUnitName);
    DDX_Text(pDX, IDC_RU_MONITOR_EDIT, m_monitorName);
    DDX_Control(pDX, IDC_RU_STOP_CHECK, m_stop);
    DDX_Control(pDX, IDC_RU_RECORD_CHECK, m_record);
    DDX_Control(pDX, IDC_RU_REWIND_CHECK, m_rewind);
    DDX_Control(pDX, IDC_RU_PLAY_CHECK, m_play);
    DDX_Control(pDX, IDC_RU_FAST_FORWARD_CHECK, m_fastForward);
    DDX_Control(pDX, IDC_RU_PAUSE_CHECK, m_pause);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(RecordingUnitDlg, CDialog)
    // {{AFX_MSG_MAP(RecordingUnitDlg)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_RU_STOP_CHECK, OnStop)
    ON_BN_CLICKED(IDC_RU_RECORD_CHECK, OnRecord)
    ON_BN_CLICKED(IDC_RU_REWIND_CHECK, OnRewind)
    ON_BN_CLICKED(IDC_RU_PLAY_CHECK, OnPlay)
    ON_BN_CLICKED(IDC_RU_FAST_FORWARD_CHECK, OnFastForward)
    ON_BN_CLICKED(IDC_RU_PAUSE_CHECK, OnPause)
    ON_COMMAND(IDOK, OnOK)
    ON_COMMAND(IDCANCEL, OnCancel)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// RecordingUnitDlg message handlers

BOOL RecordingUnitDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CDialog::OnInitDialog();

    // Set the icon for this dialog.
    SetIcon(m_hIcon, TRUE);            // Set big icon
//    SetIcon(m_hIcon, FALSE);        // Set small icon

    // Set up dialog display
    LoadButtonIcons();
    SetToolTips();

    // Initialise the edit boxes.
    UpdateData( FALSE );

    m_stop.SetFocus();

    FUNCTION_EXIT;
    return FALSE;  // return FALSE because the focus is set to a control.
}


/////////////////////////////////////////////////////////////////////////////
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void RecordingUnitDlg::OnPaint()
{
    FUNCTION_ENTRY( "OnPaint" );

    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR RecordingUnitDlg::OnQueryDragIcon()
{
    FUNCTION_ENTRY( "OnQueryDragIcon" );
    FUNCTION_EXIT;
    return (HCURSOR) m_hIcon;
}


/////////////////////////////////////////////////////////////////////////////
// Prevents dialog closing if user presses enter.
void RecordingUnitDlg::OnOK()
{
    FUNCTION_ENTRY( "OnOK" );
    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// Called when the close button is pressed.
void RecordingUnitDlg::OnCancel()
{
    FUNCTION_ENTRY( "OnCancel" );

    // Hide the window for future use.
    ShowWindow( SW_HIDE );

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
//                        Record & Playback Functions                             //
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Stop the recording unit.
void RecordingUnitDlg::OnStop()
{
    FUNCTION_ENTRY( "OnStop" );

    m_stop.SetCheck( 0 );

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// Record the recording unit.
void RecordingUnitDlg::OnRecord()
{
    FUNCTION_ENTRY( "OnRecord" );

    m_record.SetCheck( 0 );

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// Rewind the recording unit.
void RecordingUnitDlg::OnRewind()
{
    FUNCTION_ENTRY( "OnRewind" );

    m_rewind.SetCheck( 0 );

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// Play the recording unit.
void RecordingUnitDlg::OnPlay()
{
    FUNCTION_ENTRY( "OnPlay" );

    if ( m_monitorID == -1 )
    {
        TA_Base_Bus::TransActiveMessage userMsg;
        UINT selectedButton = userMsg.showMsgBox(IDS_UI_090071);
        // AfxMessageBox( _T( "No monitor is associated with the recording unit." ) );
        m_play.SetCheck( 0 );

        FUNCTION_EXIT;
        return;
    }

    // Clear any camera/sequence associations the monitor may have.
//    AfxGetApp()->GetMainWnd()->InvalidateRect( NULL, TRUE );

    // Start playing.
    m_play.SetCheck( 0 );

    // Force a repaint on the parent dialog to update the labels.
//    GetParent()->PostMessage( WM_PAINT, NULL, NULL );

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// Fast forward the recording unit.
void RecordingUnitDlg::OnFastForward()
{
    FUNCTION_ENTRY( "OnFastForward" );

    m_fastForward.SetCheck( 0 );

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// Pause the recording unit.
void RecordingUnitDlg::OnPause()
{
    FUNCTION_ENTRY( "OnPause" );

    m_pause.SetCheck( 0 );

    FUNCTION_EXIT;
}


/**
  * SetMonitor.
  *
  * Set the monitor name and ID.
  */

void RecordingUnitDlg::SetMonitor( CString& monitorName, short monitorID )
{
    FUNCTION_ENTRY( "SetMonitor" );

    m_monitorName = monitorName;
    m_monitorID = monitorID;
    UpdateData( FALSE );

    FUNCTION_EXIT;
}


/**
  * SetRecordingUnit.
  *
  * Set the recording unit name and ID.
  */

void RecordingUnitDlg::SetRecordingUnit( CString& recordingUnitName, short recordingUnitID )
{
    FUNCTION_ENTRY( "SetRecordingUnit" );

    m_recordingUnitName = recordingUnitName;
    m_recordingUnitID = recordingUnitID;
    UpdateData( FALSE );

    FUNCTION_EXIT;
}


