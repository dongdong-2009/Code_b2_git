/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/telephone/telephone_manager/src/CTelephoneManagerDlg.cpp $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  */

#include "app/telephone/telephone_manager/src/stdafx.h"
#include "app/telephone/telephone_manager/src/TelephoneApp.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/RightsException.h"
#include "core/message/types/TelephoneManagerAudit_MessageTypes.h"

#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/ResizingProperties.h"
#include "bus/generic_gui/src/TransActiveMessage.h"

#include "app/telephone/telephone_manager/src/CTelephoneManagerDlg.h"
#include "app/telephone/telephone_manager/src/ItaTelephoneManagerSummary.h"
#include "app/telephone/telephone_manager/src/TelephoneManagerUserEvents.h"
#include "app/telephone/telephone_manager/src/TelephoneDirectoryUploadUtility.h"
#include "app/telephone/telephone_manager/src/CtaTelephoneManagerSummary.h"
#include "app/telephone/telephone_manager/src/TelephoneDirectoryUploadUtilityException.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;
using namespace TA_Base_Bus;
using namespace TA_Base_Core;

static UINT indicators[] =
{
    ID_STATUS
};


const string CTelephoneManagerDlg::UPLOADED_MESSAGE       = "Uploaded_Directory";
const string CTelephoneManagerDlg::FILE_UPLOADED_MESSAGE  = "File_Uploaded";

/////////////////////////////////////////////////////////////////////////////
// CTelephoneManagerDlg dialog

CTelephoneManagerDlg::CTelephoneManagerDlg(TA_Base_Bus::IGUIAccess& genericGUICallback,
                                           CWnd* pParent/* = NULL*/)
    : TransActiveDialog(genericGUICallback, CTelephoneManagerDlg::IDD, pParent),
      m_telephoneFacilities(pParent),
      m_toShowStatusBar(true),
      m_toShowToolBar(true),
      m_ptransactiveInterfaces(NULL)
{
    FUNCTION_ENTRY( "CTelephoneManagerDlg" );

    // {{AFX_DATA_INIT(CTelephoneManagerDlg)
    m_Name = _T("");
    m_Directory = _T("");
    m_Number = _T("");
    m_Location = _T("");
    // }}AFX_DATA_INIT

    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    FUNCTION_EXIT;
}


//TD18095, jianghp, to fix the performance of showing manager application
void CTelephoneManagerDlg::setTelephoneManagerSummary(ItaTelephoneManagerSummary *ptransactiveInterfaces)
{
    FUNCTION_ENTRY( "setTelephoneManagerSummary" );

    m_ptransactiveInterfaces = ptransactiveInterfaces;
    ///*//TD18095, jianghp, to fix the performance of showing manager application
    // Loading in the Tab region information - PropertySheet
    CWnd* tabWnd = this->GetDlgItem(IDC_TELEPHONE_FACILITIES);
    ASSERT_VALID(tabWnd);

    CRect tabRect;
    tabWnd->GetWindowRect(tabRect);
    tabWnd->DestroyWindow();
    this->ScreenToClient(tabRect);

    m_telephoneFacilities.setTelephoneManagerSummary(m_ptransactiveInterfaces);
    m_telephoneFacilities.Create(this,WS_CHILD|WS_EX_CONTROLPARENT|WS_VISIBLE|WS_TABSTOP);

    // The minimum allowed dimensions are initial (startup) dimensions
    CRect rect;
    GetWindowRect(rect);
    // tabRect.left -= 7;
    tabRect.left = (rect.Width() - tabRect.Width())/2;
    tabRect.top -= 7;
    m_telephoneFacilities.MoveWindow(tabRect);

    loadToolBar();
    loadStatusBar();

    // Register necessary callbacks
    CtaTelephoneManagerSummary* pCtaSummary = dynamic_cast<CtaTelephoneManagerSummary*>(m_ptransactiveInterfaces);
    ((TelephoneDirectoryMessageSubscriber*)pCtaSummary)->addListener(static_cast<ITelephoneDirectoryListener*>(this));

    FUNCTION_EXIT;
}


//TD18095, jianghp, to fix the performance of showing manager application

CTelephoneManagerDlg::~CTelephoneManagerDlg()
{
    FUNCTION_ENTRY( "~CTelephoneManagerDlg" );
    FUNCTION_EXIT;
}


void CTelephoneManagerDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    TransActiveDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CTelephoneManagerDlg)
    DDX_Text(pDX, IDC_SELECTED_NAME, m_Name);
    DDV_MaxChars(pDX, m_Name, 12);
    DDX_Text(pDX, IDC_SELECTED_DIRECTORY, m_Directory);
    DDV_MaxChars(pDX, m_Directory, 8);
    DDX_Text(pDX, IDC_SELECTED_NUMBER, m_Number);
    DDV_MaxChars(pDX, m_Number, 8);
    DDX_Text(pDX, IDC_SELECTED_LOCATION, m_Location);
    DDV_MaxChars(pDX, m_Location, 8);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CTelephoneManagerDlg, TransActiveDialog)
    // {{AFX_MSG_MAP(CTelephoneManagerDlg)
    ON_COMMAND(ID_VIEW_STATUSBAR, OnViewStatusbar)
    ON_COMMAND(ID_FILE_UPLOADDIRECTORY, OnUploadDirectoryClicked)
    ON_COMMAND(IDCANCEL, OnExit)
    ON_COMMAND(ID_HELP_ABOUTTELEPHONECONTROLLER, OnHelpAbouttelephonecontroller)
    ON_COMMAND(ID_VIEW_TOOLBAR, OnViewToolbar)
    ON_COMMAND(ID_HELP_TELEPHONECONTROLLERHELP, onHelpTelephoneControl)
    ON_WM_MOVE()
    // TD18095, jianghp, to fix the performance of showing manager application
    ON_WM_CLOSE()
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTelephoneManagerDlg message handlers

void CTelephoneManagerDlg::OnMove(int x, int y)
{
    FUNCTION_ENTRY( "OnMove" );

    TransActiveDialog::OnMove(x, y);

    FUNCTION_EXIT;
}


void CTelephoneManagerDlg::OnViewStatusbar()
{
    FUNCTION_ENTRY( "OnViewStatusbar" );

    CMenu* menu = CWnd::GetMenu();
    ASSERT_VALID(menu);

    UINT isPrevChecked = menu->CheckMenuItem(ID_VIEW_STATUSBAR, MF_BYCOMMAND );
    if(isPrevChecked == MF_CHECKED)
    {
        menu->CheckMenuItem(ID_VIEW_STATUSBAR, MF_UNCHECKED|MF_BYCOMMAND );
        m_toShowStatusBar = false;
    }
    else if (isPrevChecked == MF_UNCHECKED)
    {
        menu->CheckMenuItem(ID_VIEW_STATUSBAR, MF_CHECKED|MF_BYCOMMAND );
        m_toShowStatusBar = true;
    }
    toggleStatusBar();

    FUNCTION_EXIT;
}


bool CTelephoneManagerDlg::isFunctionPermitted(TA_Base_Bus::AccessControlledAction action)
{
    FUNCTION_ENTRY( "isFunctionPermitted" );

    ItaRights* ptaRights = dynamic_cast< CtaTelephoneManagerSummary* >( m_ptransactiveInterfaces )->getItaRights();
    ItaRuntime* ptaRuntime = static_cast<ItaRuntime*>(m_ptransactiveInterfaces);

    string sessionId = m_ptransactiveInterfaces->getMySessionId();
    unsigned long entitykey = ptaRuntime->getMyEntityKey();

    string reason;
    bool permitted = false;

    try
    {
        permitted = ptaRights->isActionPermittedOnResource(sessionId, entitykey, action, reason);
    }
    catch(RightsException& e)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Catch RightException: %s", e.what());
    }

    FUNCTION_EXIT;
    return permitted;
}


void CTelephoneManagerDlg::onDegradedMode(bool isOccDown)
{
    FUNCTION_ENTRY( "onDegradedMode" );

    m_telephoneFacilities.enablePage(2, !isOccDown);
    m_telephoneFacilities.enablePage(3, !isOccDown);

    FUNCTION_EXIT;
}


void CTelephoneManagerDlg::onDirectLineModifiedEvent(TelephoneDirectLinesMessageSubscriber* ppublisher, const TelephoneSpeedDialButtonDetails& details)
{
    FUNCTION_ENTRY( "onDirectLineModifiedEvent" );

    SpeedDialButtonModified(details);

    FUNCTION_EXIT;
}


void CTelephoneManagerDlg::onDirectoryUploadEvent()
{
    FUNCTION_ENTRY( "onDirectoryUploadEvent" );

    ReloadDirectory();

    FUNCTION_EXIT;
}


void CTelephoneManagerDlg::onDirectoryEntryEvent(TelephoneDirectoryMessageSubscriber* ppublisher, ITelephoneDirectoryListener::EventType type, const TelephoneDirectoryEntryData& directoryEntryData)
{
    FUNCTION_ENTRY( "onDirectoryEntryEvent" );

    switch (type)
    {
    case ITelephoneDirectoryListener::New:
        DirectoryEntryAdded(directoryEntryData);
        break;

    case ITelephoneDirectoryListener::Update:
        DirectoryEntryModified(directoryEntryData);
        break;

    case ITelephoneDirectoryListener::Delete:
        DirectoryEntryDeleted(directoryEntryData);
        break;

    default:
        TA_ASSERT(false, "Unknown Event Types?");
    }

    FUNCTION_EXIT;
}


void CTelephoneManagerDlg::onDirectoryEvent(TelephoneDirectoryMessageSubscriber* ppublisher, ITelephoneDirectoryListener::EventType type, const TelephoneDirectoryData& directoryData)
{
    FUNCTION_ENTRY( "onDirectoryEvent" );

    switch (type)
    {
    case ITelephoneDirectoryListener::New:
        DirectoryAdded(directoryData);
        break;

    case ITelephoneDirectoryListener::Update:
        DirectoryModified(directoryData);
        break;

    case ITelephoneDirectoryListener::Delete:
        DirectoryDeleted(directoryData);
        break;

    default:
        TA_ASSERT(false, "Unknown Event Types?");
    }

    FUNCTION_EXIT;
}


void CTelephoneManagerDlg::SpeedDialButtonModified(const TelephoneSpeedDialButtonDetails& btnDetails)
{
    FUNCTION_ENTRY( "SpeedDialButtonModified" );

    m_telephoneFacilities.getConfigureDirectLinesPage()->PostMessage(WM_DIRECTLINES_MODIFIED, reinterpret_cast<WPARAM>(new TelephoneSpeedDialButtonDetails(btnDetails)), 0);
    m_telephoneFacilities.getDirectLinePage()->PostMessage(WM_DIRECTLINES_MODIFIED, reinterpret_cast<WPARAM>(new TelephoneSpeedDialButtonDetails(btnDetails)), 0);

    FUNCTION_EXIT;
}


void CTelephoneManagerDlg::ReloadDirectory()
{
    FUNCTION_ENTRY( "ReloadDirectory" );

    m_telephoneFacilities.getDirectorySearchPage()->PostMessage(WM_DIRECTORY_UPLOAD, 0, 0);
    m_telephoneFacilities.getConfigureDirectoryPage()->PostMessage(WM_DIRECTORY_UPLOAD, 0, 0);

    FUNCTION_EXIT;
}


void CTelephoneManagerDlg::DirectoryEntryAdded(const TelephoneDirectoryEntryData& directoryEntryData)
{
    FUNCTION_ENTRY( "DirectoryEntryAdded" );

    m_telephoneFacilities.getDirectorySearchPage()->PostMessage(WM_DIRECTORY_ENTRY_ADDED, reinterpret_cast<WPARAM>(new TelephoneDirectoryEntryData(directoryEntryData)), 0);
    m_telephoneFacilities.getConfigureDirectoryPage()->PostMessage(WM_DIRECTORY_ENTRY_ADDED, reinterpret_cast<WPARAM>(new TelephoneDirectoryEntryData(directoryEntryData)), 0);

    FUNCTION_EXIT;
}


void CTelephoneManagerDlg::DirectoryEntryModified(const TelephoneDirectoryEntryData& directoryEntryData)
{
    FUNCTION_ENTRY( "DirectoryEntryModified" );

    m_telephoneFacilities.getDirectorySearchPage()->PostMessage(WM_DIRECTORY_ENTRY_MODIFIED, reinterpret_cast<WPARAM>(new TelephoneDirectoryEntryData(directoryEntryData)), 0);
    m_telephoneFacilities.getConfigureDirectoryPage()->PostMessage(WM_DIRECTORY_ENTRY_MODIFIED, reinterpret_cast<WPARAM>(new TelephoneDirectoryEntryData(directoryEntryData)), 0);

    FUNCTION_EXIT;
}


void CTelephoneManagerDlg::DirectoryEntryDeleted(const TelephoneDirectoryEntryData& directoryEntryData)
{
    FUNCTION_ENTRY( "DirectoryEntryDeleted" );

    m_telephoneFacilities.getDirectorySearchPage()->PostMessage(WM_DIRECTORY_ENTRY_DELETED, reinterpret_cast<WPARAM>(new TelephoneDirectoryEntryData(directoryEntryData)), 0);
    m_telephoneFacilities.getConfigureDirectoryPage()->PostMessage(WM_DIRECTORY_ENTRY_DELETED, reinterpret_cast<WPARAM>(new TelephoneDirectoryEntryData(directoryEntryData)), 0);

    FUNCTION_EXIT;
}


void CTelephoneManagerDlg::DirectoryAdded(const TelephoneDirectoryData& directoryData)
{
    FUNCTION_ENTRY( "DirectoryAdded" );

    m_telephoneFacilities.getDirectorySearchPage()->PostMessage(WM_DIRECTORY_ADDED, reinterpret_cast<WPARAM>(new TelephoneDirectoryData(directoryData)), 0);
    m_telephoneFacilities.getConfigureDirectoryPage()->PostMessage(WM_DIRECTORY_ADDED, reinterpret_cast<WPARAM>(new TelephoneDirectoryData(directoryData)), 0);

    FUNCTION_EXIT;
}


void CTelephoneManagerDlg::DirectoryModified(const TelephoneDirectoryData& directoryData)
{
    FUNCTION_ENTRY( "DirectoryModified" );

    m_telephoneFacilities.getDirectorySearchPage()->PostMessage(WM_DIRECTORY_MODIFIED, reinterpret_cast<WPARAM>(new TelephoneDirectoryData(directoryData)), 0);
    m_telephoneFacilities.getConfigureDirectoryPage()->PostMessage(WM_DIRECTORY_MODIFIED, reinterpret_cast<WPARAM>(new TelephoneDirectoryData(directoryData)), 0);

    FUNCTION_EXIT;
}


void CTelephoneManagerDlg::DirectoryDeleted(const TelephoneDirectoryData& directoryData)
{
    FUNCTION_ENTRY( "DirectoryDeleted" );

    m_telephoneFacilities.getDirectorySearchPage()->PostMessage(WM_DIRECTORY_DELETED, reinterpret_cast<WPARAM>(new TelephoneDirectoryData(directoryData)), 0);
    m_telephoneFacilities.getConfigureDirectoryPage()->PostMessage(WM_DIRECTORY_DELETED, reinterpret_cast<WPARAM>(new TelephoneDirectoryData(directoryData)), 0);

    FUNCTION_EXIT;
}


void CTelephoneManagerDlg::OnUploadDirectoryClicked()
{
    FUNCTION_ENTRY( "OnUploadDirectoryClicked" );

    CFileDialog dlg(true);
    if(dlg.DoModal() == IDOK)
    {
        CWaitCursor cursor;

        CString selFile = dlg.GetPathName();
    
        string fullPathFileName = selFile.GetBuffer(selFile.GetLength()+1);
		//jinhua++17640
		try
		{
		//++jinhua17640

			TelephoneDirectoryUploadUtility uploadUtil(fullPathFileName,m_ptransactiveInterfaces->getMySessionId());

			if(uploadUtil.processUploadFromFile() == false)
			{
				// TD14164 ++
				/*AfxMessageBox(IDS_UPLOAD_DIR_ERROR);*/
				TA_Base_Bus::TransActiveMessage userMsg;
				
				CString fieldName = "for directory exist"; //jinhua 17640
				userMsg << fieldName;
				userMsg.showMsgBox(IDS_UE_620018);
			}
			else
			{
				ItaAuditing* ptaAudit =  static_cast<ItaAuditing*>(m_ptransactiveInterfaces);
				TA_Base_Core::NameValuePairs params;
				params["UPLOADED_MESSAGE"] = "Success";				
				params["FILE_UPLOADED_MESSAGE"] = "fullPathFileName";				
				ptaAudit->submitAuditMessage(TA_Base_Core::TelephoneManagerAudit::TeleDirectoryUploaded, params);
			}
		//jinhua++17640
		}
		catch (TelephoneDirectoryUploadUtilityException& e )
		{
			MessageBox( e.what(), "TelephoneManager", MB_ICONERROR );
		}
		//++jinhua17640
    }
    FUNCTION_EXIT;
}


BOOL CTelephoneManagerDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    ResizingProperties properties;
    properties.canMaximise = false;


    // The minimum allowed dimensions are initial (startup) dimensions
    CRect rect;
    // GetClientRect(rect);
    GetWindowRect(rect);

    properties.minWidth = rect.right - rect.left; //rect.Width();
    properties.minHeight = rect.bottom - rect.top; //rect.Height();
    properties.maxHeight = rect.bottom - rect.top; //rect.Height();
    properties.maxWidth = rect.right - rect.left; //rect.Width();

    setResizingProperties(properties);

    TransActiveDialog::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    // when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);            // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

/*
    // Loading in the Tab region information - PropertySheet
    CWnd* tabWnd = this->GetDlgItem(IDC_TELEPHONE_FACILITIES);
    ASSERT_VALID(tabWnd);


    CRect tabRect;
    tabWnd->GetWindowRect(tabRect);
    tabWnd->DestroyWindow();
    this->ScreenToClient(tabRect);

    m_telephoneFacilities.Create(this,WS_CHILD|WS_EX_CONTROLPARENT|WS_VISIBLE|WS_TABSTOP);

    // tabRect.left -= 7;
    tabRect.left = (rect.Width() - tabRect.Width())/2;
    tabRect.top -= 7;
    m_telephoneFacilities.MoveWindow(tabRect);

    loadToolBar();
    loadStatusBar();

    // Register necessary callbacks
    CtaTelephoneManagerSummary* pCtaSummary = dynamic_cast<CtaTelephoneManagerSummary*>(m_ptransactiveInterfaces);
    ((TelephoneDirectoryMessageSubscriber*)pCtaSummary)->addListener(static_cast<ITelephoneDirectoryListener*>(this));

    // check right
    bool canUpload = isFunctionPermitted(TA_Base_Bus::aca_TELEPHONE_UPLOAD_DIRECTORY);
    CMenu* menu = CWnd::GetMenu();
    if(menu != NULL)
    {
        menu->EnableMenuItem(ID_FILE_UPLOADDIRECTORY,!canUpload);
    }
*/

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CTelephoneManagerDlg::loadStatusBar()
{
    FUNCTION_ENTRY( "loadStatusBar" );

    if(m_toShowStatusBar)
    {
        m_wndStatusBar.Create(this); //We create the status bar

        m_wndStatusBar.SetIndicators(indicators,1); //Set the number of panes

        CRect rect;
        GetClientRect(&rect);

        // Size the pane
        m_wndStatusBar.SetPaneInfo(0, ID_STATUS, SBPS_NORMAL, rect.Width());

        // This is where we actually draw it on the screen
        RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, ID_STATUS);

        m_wndStatusBar.SetPaneText(0,_T(" Shows all configured direct line"));
    }
    else
    {
        m_wndStatusBar.DestroyWindow();
    }

    FUNCTION_EXIT;
}


void CTelephoneManagerDlg::loadToolBar()
{
    FUNCTION_ENTRY( "loadToolBar" );

    if (m_toShowToolBar)
    {
        m_wndToolBar.Create(this); //We create the status bar
        m_wndToolBar.LoadToolBar(IDR_TOOLBAR);

        // This is where we actually draw it on the screen
        RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, IDR_MAINFRAME);
    }
    else
    {
        m_wndToolBar.ShowWindow(false);
    }

    FUNCTION_EXIT;
}


void CTelephoneManagerDlg::toggleToolBar()
{
    FUNCTION_ENTRY( "toggleToolBar" );

    if(m_toShowToolBar)
    {
        m_wndToolBar.ShowWindow(true);
    }
    else
    {
        m_wndToolBar.ShowWindow(false);
    }

    FUNCTION_EXIT;
}


void CTelephoneManagerDlg::toggleStatusBar()
{
    FUNCTION_ENTRY( "toggleStatusBar" );

    if(m_toShowStatusBar)
    {
        m_wndStatusBar.ShowWindow(true);
    }
    else
    {
        m_wndStatusBar.ShowWindow(false);
    }

    FUNCTION_EXIT;
}


void CTelephoneManagerDlg::onTabChanged()
{
    FUNCTION_ENTRY( "onTabChanged" );

    if(m_toShowStatusBar)
    {
        int curSel = m_telephoneFacilities.GetActiveIndex();
        CPropertyPage* page = m_telephoneFacilities.GetPage(curSel);

        if(page == m_telephoneFacilities.getDirectorySearchPage())
        {
            m_wndStatusBar.SetPaneText(0,_T(" Search for contacts"));
        }
        else if(page == m_telephoneFacilities.getDirectLinePage())
        {
            m_wndStatusBar.SetPaneText(0,_T(" Shows all configured direct line"));
        }
        else if(page == m_telephoneFacilities.getConfigureDirectLinesPage())
        {
            m_wndStatusBar.SetPaneText(0,_T(" Configure telephone lines"));
        }
        else if(page == m_telephoneFacilities.getConfigureDirectoryPage())
        {
            m_wndStatusBar.SetPaneText(0,_T(" Configure directories storing the contacts"));
        }
        else
        {
            m_wndStatusBar.SetPaneText(0,_T(" Ready"));
        }
    }

    FUNCTION_EXIT;
}


BOOL CTelephoneManagerDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnNotify" );

    BOOL result =  TransActiveDialog::OnNotify(wParam, lParam, pResult);

    if (wParam == (WPARAM)(&m_telephoneFacilities))
    {
        NMHDR* pnmh = (LPNMHDR) lParam;

        if (TCN_SELCHANGE == pnmh->code)
        {
            // Selected page just changed - must result in a refresh
            // Must do this after OnNotify (above) to ensure
            // getActivePage returns correct page
            onTabChanged();
            // The execute button state is maintained externally,
            // as windows lose focus they
        }
    }

    FUNCTION_EXIT;
    return result;
}


void CTelephoneManagerDlg::OnExit()
{
    FUNCTION_ENTRY( "OnExit" );

    // Force closure//TD18095, jianghp, to fix the performance of showing manager application
    DestroyWindow();

    FUNCTION_EXIT;
}


void CTelephoneManagerDlg::OnHelpAbouttelephonecontroller()
{
    FUNCTION_ENTRY( "OnHelpAbouttelephonecontroller" );

    TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();

    FUNCTION_EXIT;
}


void CTelephoneManagerDlg::OnViewToolbar()
{
    FUNCTION_ENTRY( "OnViewToolbar" );

    CMenu* menu = CWnd::GetMenu();
    ASSERT_VALID(menu);

    UINT isPrevChecked = menu->CheckMenuItem(ID_VIEW_TOOLBAR, MF_BYCOMMAND );
    if(isPrevChecked == MF_CHECKED)
    {
        menu->CheckMenuItem(ID_VIEW_TOOLBAR, MF_UNCHECKED|MF_BYCOMMAND );
        m_toShowToolBar = false;
    }
    else if (isPrevChecked == MF_UNCHECKED)
    {
        menu->CheckMenuItem(ID_VIEW_TOOLBAR, MF_CHECKED|MF_BYCOMMAND );
        m_toShowToolBar = true;
    }

    toggleToolBar();

    FUNCTION_EXIT;
}


void CTelephoneManagerDlg::onHelpTelephoneControl()
{
    FUNCTION_ENTRY( "onHelpTelephoneControl" );

    // Launch help
    TA_Base_Bus::HelpLauncher::getInstance().displayHelp();

    FUNCTION_EXIT;
}


void CTelephoneManagerDlg::onClose()
{
    FUNCTION_ENTRY( "onClose" );

    // TD18095, jianghp, to fix the performance of showing manager application
    DestroyWindow();

    FUNCTION_EXIT;
}


void CTelephoneManagerDlg::processTelephoneRights( const TelephoneRights& rights )
{
    FUNCTION_ENTRY( "processTelephoneRights" );

    CMenu* menu = CWnd::GetMenu();

    if ( menu != NULL )
    {
        menu->EnableMenuItem( ID_FILE_UPLOADDIRECTORY, !rights.acaTelephoneUploadDirectory );
    }

    m_telephoneFacilities.enablePage( 3, rights.acaTelephoneConfigureDirectory );
    m_telephoneFacilities.enablePage( 1, rights.acaTelephoneSearchDirectory );

    FUNCTION_EXIT;
}


