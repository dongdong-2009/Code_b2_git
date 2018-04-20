/**
  * The source code in this file is the property of
  * MI Consulting Group (Australia) and is not for
  * redistribution in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_display_manager/src/SequenceControllerDlg.cpp $
  * @author Matthew Atkinson
  * @version $Revision: #1 $
  * Last Modification: $DateTime: 2008/11/28 16:26:01 $
  * Last Modified by:
  *
  * The SequenceControllerDlg class allows the user to halt, play, pause and navigate to
  * and fro along any give sequence of cameras. This class is not suitable for sub-classing.
  */


#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif

#include "stdafx.h"
#include "SequenceControllerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SequenceControllerDlg dialog
SequenceControllerDlg::SequenceControllerDlg(CWnd* pParent /*=NULL*/ )
    : CDialog(SequenceControllerDlg::IDD, pParent)
{
    FUNCTION_ENTRY( "SequenceControllerDlg" );

    // {{AFX_DATA_INIT(SequenceControllerDlg)
    m_sequenceName = _T("");
    m_monitorName = _T("");
    // }}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDI_SEQUENCE);

    FUNCTION_EXIT;
}


/**
  * LoadButtonIcons.
  *
  * Load all the controller button icons.
  */

void SequenceControllerDlg::LoadButtonIcons()
{
    FUNCTION_ENTRY( "LoadButtonIcons" );

    // Load check box icons.
    m_previousElementIcon = AfxGetApp()->LoadIcon(IDI_PREVIOUSELEMENT_RW_ICON);
    m_nextElementIcon = AfxGetApp()->LoadIcon(IDI_NEXTELEMENT_FF_ICON);
    m_playIcon = AfxGetApp()->LoadIcon(IDI_PLAY_ICON);
    m_pauseIcon = AfxGetApp()->LoadIcon(IDI_PAUSE_ICON);

    // Set the check box icons.
    m_previousElement.SetIcon(m_previousElementIcon);
    m_nextElement.SetIcon(m_nextElementIcon);
    m_pause.SetIcon(m_pauseIcon);
    m_play.SetIcon(m_playIcon);

    FUNCTION_EXIT;
}


/**
  * SetToolTips.
  *
  * Set the tool tips for the dialog controller buttons.
  */

void SequenceControllerDlg::SetToolTips()
{
    FUNCTION_ENTRY( "SetToolTips" );

    m_ctlToolTip.Create(this);
    m_ctlToolTip.SetFont(GetFont());
    m_ctlToolTip.AddWindowTool(GetDlgItem(IDC_SC_PREVIOUSELEMENT_BUTTON ), "Previous element");
    m_ctlToolTip.AddWindowTool(GetDlgItem(IDC_SC_NEXTELEMENT_BUTTON ), "Next element");
    m_ctlToolTip.AddWindowTool(GetDlgItem(IDC_SC_PLAY_CHECK ), "Play");
    m_ctlToolTip.AddWindowTool(GetDlgItem(IDC_SC_PAUSE_CHECK ), "Pause");

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
void SequenceControllerDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(SequenceControllerDlg)
    DDX_Text(pDX, IDC_SC_SEQNAMEDESCRIPTION_EDIT, m_sequenceName);
    DDX_Text(pDX, IDC_SC_MONITOR_EDIT, m_monitorName);
    DDX_Control(pDX, IDC_SC_PREVIOUSELEMENT_BUTTON, m_previousElement);
    DDX_Control(pDX, IDC_SC_PLAY_CHECK, m_play);
    DDX_Control(pDX, IDC_SC_NEXTELEMENT_BUTTON, m_nextElement);
    DDX_Control(pDX, IDC_SC_PAUSE_CHECK, m_pause);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(SequenceControllerDlg, CDialog)
    // {{AFX_MSG_MAP(SequenceControllerDlg)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_SC_PREVIOUSELEMENT_BUTTON, OnPreviousElementButton)
    ON_BN_CLICKED(IDC_SC_PLAY_CHECK, OnPlayCheck)
    ON_BN_CLICKED(IDC_SC_NEXTELEMENT_BUTTON, OnNextElementButton)
    ON_BN_CLICKED(IDC_SC_PAUSE_CHECK, OnPauseCheck)
    ON_COMMAND(IDOK, OnOK)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// SequenceControllerDlg message handlers
BOOL SequenceControllerDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CDialog::OnInitDialog();

    // Set the icon for this dialog.
    SetIcon(m_hIcon, TRUE);            // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

    // Set up button icons and tooltips.
    LoadButtonIcons();
    SetToolTips();

    // Initialise the edit boxes.
    UpdateData( FALSE );

    m_play.SetFocus();

    FUNCTION_EXIT;
    return FALSE;  // return FALSE because the focus is set to a control.
}


/////////////////////////////////////////////////////////////////////////////
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void SequenceControllerDlg::OnPaint()
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
HCURSOR SequenceControllerDlg::OnQueryDragIcon()
{
    FUNCTION_ENTRY( "OnQueryDragIcon" );
    FUNCTION_EXIT;
    return (HCURSOR) m_hIcon;
}


/////////////////////////////////////////////////////////////////////////////
// Prevents application closing if user presses enter.
void SequenceControllerDlg::OnOK()
{
    FUNCTION_ENTRY( "OnOK" );
    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// Called when the close button is pressed.
void SequenceControllerDlg::OnCancel()
{
    FUNCTION_ENTRY( "OnCancel" );

    // Hide the window for future use.
    ShowWindow( SW_HIDE );

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
//                            Playback Functions                               //
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// View the previous element in the sequence.
// Only available when the sequence is paused.
void SequenceControllerDlg::OnPreviousElementButton()
{
    FUNCTION_ENTRY( "OnPreviousElementButton" );

    m_previousElement.SetCheck( 0 );

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// Play the sequence.
void SequenceControllerDlg::OnPlayCheck()
{
    FUNCTION_ENTRY( "OnPlayCheck" );

    m_play.SetCheck( 0 );

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// View the next element in the sequence.
// Only available when the sequence is paused.
void SequenceControllerDlg::OnNextElementButton()
{
    FUNCTION_ENTRY( "OnNextElementButton" );

    m_nextElement.SetCheck( 0 );

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// Pause/Resume the sequence.
void SequenceControllerDlg::OnPauseCheck()
{
    FUNCTION_ENTRY( "OnPauseCheck" );

    m_pause.SetCheck( 0 );

    FUNCTION_EXIT;
}


/**
  * setMonitor.
  *
  * Set the monitor name and ID.
  */
void SequenceControllerDlg::setMonitor( CString& monitor)
{
    FUNCTION_ENTRY( "setMonitor" );

    m_monitorName = monitor;
    UpdateData( FALSE );

    FUNCTION_EXIT;
}


/**
  * setMonitor.
  *
  * Set the monitor name and ID.
  */
void SequenceControllerDlg::setSequence( CString& sequence)
{
    FUNCTION_ENTRY( "setSequence" );

    m_sequenceName = sequence;
    UpdateData( FALSE );

    FUNCTION_EXIT;
}


