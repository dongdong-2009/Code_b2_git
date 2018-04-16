/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/EquipmentStatusViewerDlg.cpp $
 * @author:  Rod Rolirad
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2018/01/23 17:49:34 $
 * Last modified by:  $Author: builder $
 * 
 * Dialog class of the main dialog responsible for displaying the equipment status list,
 * location, report names, toolbar, status bar etc
 *
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#pragma warning(disable:4355)  // warning C4355: 'this' : used in base member initializer list'
#endif

#include "app/scada/EquipmentStatusViewer/src/stdafx.h"
#include "app/scada/EquipmentStatusViewer/src/EquipmentStatusViewer.h"
#include "app/scada/EquipmentStatusViewer/src/EquipmentStatusViewerDlg.h"
#include "app/scada/EquipmentStatusViewer/src/FilterDialog.h"
#include "app/scada/EquipmentStatusViewer/src/NewReport.h"
#include "app/scada/EquipmentStatusViewer/src/EditReport.h"
#include "app/scada/EquipmentStatusViewer/src/ReportCommentDialog.h"
#include "app/scada/EquipmentStatusViewer/src/FileUtility.h"
#include "app/scada/EquipmentStatusViewer/src/AuditMessageUtility.h"
#include "app/scada/EquipmentStatusViewer/src/ExportFileDialog.h"
#include "app/scada/EquipmentStatusViewer/src/DataNodeListItem.h"
#include "app/scada/EquipmentStatusViewer/src/DataPointListItem.h"
#include "app/scada/EquipmentStatusViewer/src/DataNodeSummaryListItem.h"
#include "bus/mfc_extensions/src/redraw_control_lock/LockControlRedraw.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/data_access_interface/src/IProfile.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "app/scada/EquipmentStatusViewer/src/DataNodeAgentProxy.h"

//TD13200 Mintao++
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/utilities/src/DebugUtil.h"

#include <afxpriv.h>
#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

// NOTE : for auditing
#include "core/message/types/ESVAudit_MessageTypes.h"

// displays the Transactive About Box
#include "bus\generic_gui\src\HelpLauncher.h"

// for displaying message boxes
#include "bus/generic_gui/src/TransActiveMessage.h"

// for printing the list control
#include "bus/mfc_extensions/src/print_list_ctrl/ListCtrlPrint.h"

//TD17888 marc_bugfix for save as dialog
#include "bus/mfc_extensions/src/file_dialog_no_folder_change/FileDialogArialDisableFolders.h"

// NOTE : Application launcher
#include "bus/generic_gui/src/AppLauncher.h"
#include "core/exceptions/src/ApplicationException.h"

// NOTE : Location related
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

// NOTE : Entity related
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"

// NOTE : Report related
#include "core/data_access_interface/src/IEquipReport.h"
#include "core/data_access_interface/src/EquipReportAccessFactory.h"
#include "core/data_access_interface/src/EquipReportDataAccessFactory.h"
#include "core/data_access_interface/src/EquipReportData.h"

// NOTE : Scada Common Factory
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/common_library/src/ScadaUtilities.h"

// NOTE : Exception related
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/ScadaProxyException.h"

// NOTE : Debug related
#include "core/utilities/src/DebugUtil.h"

#include "core/data_access_interface/src/Session.h"
#include "bus/scada/common_library/IDL/src/DataNodeCorbaTypes.h"
#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"

#include "resource.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TOOLBAR_TOOLTIP_TIMER	1000

using TA_Base_Bus::TransActiveDialog;
using namespace TA_Base_Core;
using namespace TA_Base_App;

namespace TA_Base_App
{
	class AuditMessageSender;
	class DataNodeEntityData;

    const unsigned long CEquipmentStatusViewerDlg::OCC_LOCATION_KEY_IN_DATABASE = 1;
	const std::string CEquipmentStatusViewerDlg::PARENT_ENTITY_NAME = "Parent Entity";

	CEquipmentStatusViewerDlg::CEquipmentStatusViewerDlg(TA_Base_Bus::IGUIAccess& genericGUICallback)
	:
	TransActiveDialog(genericGUICallback, CEquipmentStatusViewerDlg::IDD),
		m_isAllowedToPrintOrExport(false),
		m_isToolTip(false),
		m_isAllowedToConfigure(false),
		m_isAllowedToViewAllReports(false),
		m_needSortListCtrl(false),
		m_hasCleanUp(false),
		m_guiEntityKey ( 0 ),
		m_isFiltered(false),
		m_isActiveReport(false),
		m_selectedLocationKey ( -1 ),
		m_selectedReportKey ( ""),
        m_populatorTimer( 0),
        m_EquipmentStatusList( m_reportItems),
		m_scadaProxyFactory(TA_Base_Bus::ScadaProxyFactory::getInstance()),
		m_profileAccessFactory ( TA_Base_Core::ProfileAccessFactory::getInstance() ),
		m_genericGUICallback(genericGUICallback),
		m_DnUpdateProcessor( NULL ),
		m_curLoginLocation(ULONG_MAX),
		m_previousSessionId("")
	{
		m_activeProfileNameConstants[ACTIVE_PROFILE_ALL_DATA] = ACTIVE_PROFILE_ALL;
		m_activeProfileNameConstants[ACTIVE_PROFILE_MMS_DATA] = ACTIVE_PROFILE_MMS;

		// If necessary this code can go in OnInitDialog before the call to TransActiveDialog::OnInitDialog()
		TA_Base_Bus::ResizingProperties properties;
		properties.canMaximise	= true;
		properties.maxHeight	= -1;
		properties.maxWidth		= -1;
		properties.minWidth		= 670;
		properties.minHeight	= 480;
		setResizingProperties(properties);

		m_authenticationLibrary = new TA_Base_Bus::AuthenticationLibrary(true);

		m_dnEntityDataPtrMap.clear();

		m_DnUpdateProcessor = new DnUpdateProcessor(*this);
		m_DnUpdateProcessor->start();
	}

	CEquipmentStatusViewerDlg::~CEquipmentStatusViewerDlg()
	{ 
		CleanUpResource();
	}

	void CEquipmentStatusViewerDlg::init()
	{
		m_previousSessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

		TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);
		
		TA_Base_Core::IEntityData * guiEntity = 0;
		guiEntity = m_genericGUICallback.getGUIEntity();
		m_guiEntityKey = guiEntity->getKey();
		
        checkPermissions();
		m_reportKeyStringToIdMap.clear();
        m_DatanodeAgentProxy.clear();

		PopulateLocations();
		
		//default set report ACTIVE_PROFILE_ALL_REPORT
		m_selectedReportKey = ACTIVE_PROFILE_ALL_REPORT;
		ClearCurrentFiltersForActive();

		AdjustGUI();
		
		EnableToolTips(TRUE);
		
        //m_progressBar.Create( "", 100, 100, false, 0, this);
		//m_progressBar.SetBarColour(PROGRESS_BAR_COLOR);
        //m_progressBar.Clear();
	

        //TD16889 Mintao 
        //at the starting set up the default sorting as sort by asset
        m_EquipmentStatusList.SortColumn(ASSET_COLUMN);
		for (int i= 0; i < m_Location.GetCount(); i++)
		{
			if (m_Location.GetItemData(i) == m_curLoginLocation)
			{
				m_Location.SetCurSel(i);
				break;
			}
		}
		OnSelchangeLocationCombo();
        //check whether we need to launch the ESV with the Tag/Inhibit/Override List
        /*bool isshowlist = RunParams::getInstance().isSet("ShowActiveProfile");
        if (isshowlist)
        {
            //prepare the report combo to be at ALL
            PopulateReports(0);
            //set the location combo to be 'ALL'
			m_Location.SetCurSel(0);
            //set the report name to be ACTIVE_PROFILE_ALL_REPORT
            m_ReportName.SetCurSel(0);
            //OnSelchangeReportCombo();
        }*/		
	}

	void CEquipmentStatusViewerDlg::DoDataExchange(CDataExchange* pDX)
	{
		TransActiveDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CEquipmentStatusViewerDlg)
	    DDX_Control(pDX, IDC_STOP, m_StopButton);
		DDX_Control(pDX, IDC_LOCATION_COMBO, m_Location);
		DDX_Control(pDX, IDC_EQUIPMENT_LIST, m_EquipmentStatusList);
		DDX_Control(pDX, IDC_REPORT_COMBO, m_ReportName);
	    //}}AFX_DATA_MAP
	}

	BEGIN_MESSAGE_MAP(CEquipmentStatusViewerDlg, TransActiveDialog)
		//{{AFX_MSG_MAP(CEquipmentStatusViewerDlg)
		ON_WM_CREATE()
		ON_COMMAND(ID_VIEW_STATUSBAR, OnViewStatusbar)
		ON_COMMAND(ID_VIEW_TOOLBAR, OnViewToolbar)
		//ON_COMMAND(ID_EDIT_CONFIGUREREPORT, OnEditConfigurereport)
		ON_COMMAND(ID_FILE_EXPORT, OnExport)
		//ON_COMMAND(ID_VIEW_FILTER, OnFilter)
		ON_COMMAND(ID_FILE_PRINT, OnPrint)
		//ON_COMMAND(ID_FILE_NEWREPORT, OnNewreport)
		//ON_COMMAND(ID_FILE_EDITREPORT, OnEditReport)
		ON_NOTIFY(LVN_COLUMNCLICK, IDC_EQUIPMENT_LIST, OnColumnClickEquipmentList)
		ON_WM_SHOWWINDOW()
		ON_WM_MENUSELECT()
		//ON_COMMAND(ID_FILE_DELETEREPORT, OnFileDeletereport)
		ON_COMMAND(ID_HELP_EQUIPMENTSTATUSVIEWERHELP, OnHelpEquipmentstatusviewerhelp)
		//ON_COMMAND(ID_TOOLBAR_HELP, OnToolbarHelp)
		ON_COMMAND(ID_TOOLBAR_HELP1, OnToolbarHelp)
		ON_CBN_SELCHANGE(IDC_LOCATION_COMBO, OnSelchangeLocationCombo)
		//ON_CBN_SELCHANGE(IDC_REPORT_COMBO, OnSelchangeReportCombo)
		//ON_COMMAND(ID_EDIT_CONFIGUREFILTER, OnEditConfigurefilter)
		ON_COMMAND(IDEXIT, OnExit)
		ON_COMMAND(ID_HELP_ABOUTEQUIPMENTSTATUSVIEWER, OnHelpAboutequipmentstatusviewer)
		//ON_COMMAND(ID_EDIT_INSPECT, OnEditInspect)
		//ON_COMMAND(ID_TOOLBAR_INSPECT, OnToolbarInspect)
		ON_NOTIFY(NM_CLICK, IDC_EQUIPMENT_LIST, OnClickEquipmentList)
		ON_NOTIFY(NM_DBLCLK, IDC_EQUIPMENT_LIST, OnDblclkEquipmentList)
		ON_MESSAGE(WM_LAUNCH_ACTIVE, onLaunchActive)
		ON_MESSAGE(WM_LIST_ITEM_UPDATE, OnListItemUpdate)
		ON_MESSAGE(WM_SESSION_CHANGE, OnSessionIdChange) 
		ON_MESSAGE(WM_SORT_LIST_CTRL, OnSortListCtrl)
		ON_WM_CLOSE()
		//ON_BN_CLICKED(IDC_STOP, OnStop)
		ON_WM_SYSCOMMAND()
		ON_WM_SIZE()
	    ON_WM_TIMER()
		ON_COMMAND(ID_TOOLBAR_EXPORT, OnExport)
		//ON_COMMAND(ID_TOOLBAR_FILTER, OnFilter)
		//ON_COMMAND(ID_TOOLBAR_PRINT, OnPrint)
		ON_COMMAND(ID_TOOLBAR_PRINT1, OnPrint)
	    ON_NOTIFY(NM_RDBLCLK, IDC_EQUIPMENT_LIST, OnRdblclkEquipmentList)
	    //}}AFX_MSG_MAP
		ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnToolTipText)
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CEquipmentStatusViewerDlg message handlers

	static UINT indicators[] =
	{
		ID_PANE_0,          // status line indicator
	    ID_PANE_1,
		ID_PANE_2
	};


	void CEquipmentStatusViewerDlg::onRunParamChange(const std::string& name, const std::string& value)
	{
		std::string sessionID = value;

		if (name.compare(RPARAM_SESSIONID) == 0 && sessionID != m_previousSessionId)
        {
			m_previousSessionId = sessionID;
            PostMessage( WM_SESSION_CHANGE, 0, 0);
        }
	}


	int CEquipmentStatusViewerDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
	{
		if (TransActiveDialog::OnCreate(lpCreateStruct) == -1)
			return -1;
				
		CreateStatusBar();
		CreateToolbar();
		CalculateToolBarStatusBarHeights();		
		
		return 0;
	}

	BOOL CEquipmentStatusViewerDlg::OnToolTipText(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
	{			
		UINT nID = pNMHDR->idFrom;

		if ((nID == ID_TOOLBAR_EXPORT) ||
			(nID == ID_TOOLBAR_PRINT) ||
			(nID == ID_TOOLBAR_PRINT1) || 
			(nID == ID_TOOLBAR_INSPECT) ||
			(nID == ID_TOOLBAR_FILTER) ||
			(nID == ID_TOOLBAR_HELP) ||
			(nID == ID_TOOLBAR_HELP1))

		{	
			m_isToolTip = true;
			
			char szFullText[255];

			AfxLoadString(nID, szFullText);        
			//SetStatusBarText(0, szFullText);
			
			return TRUE;

		}
		else
		{
			//SetStatusBarText(0, "Ready");
		}
				
		return TRUE;
	}

	BOOL CEquipmentStatusViewerDlg::OnInitDialog()
	{
		TransActiveDialog::OnInitDialog();

		HICON appIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
		SetIcon(appIcon, TRUE);
		HICON buttonIcon = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_STOP), IMAGE_ICON, 0, 0, LR_LOADMAP3DCOLORS);
		if (buttonIcon)
			m_StopButton.SetIcon(buttonIcon);

		m_WndToolBar.ShowWindow(SW_SHOW);
		m_WndStatusBar.ShowWindow(SW_SHOW);
		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

		//TD18095, jianghp, to fix the performance of showing manager application

		return TRUE; 
	}


	//******************************
	// Tool bar
	//******************************

	void CEquipmentStatusViewerDlg::CreateToolbar()
	{
		// Create toolbar at the top of the dialog window
		if (!m_WndToolBar.CreateEx(this, TBSTYLE_FLAT , WS_CHILD | WS_VISIBLE | CBRS_TOP | 
										CBRS_TOOLTIPS | CBRS_FLYBY ) ||
			!m_WndToolBar.LoadToolBar(/*IDR_MAINTOOLBAR*/IDR_MAINTOOLBAR2))
		{
			//m_toolBar.LoadToolBar(IDR_MAINTOOLBAR);
			TRACE0("Failed to create toolbar\n");
		}

		// Add text to the buttons
		// Any exception from these functions will fall through and be caught by
		// the calling member
		//AddTextToButton(ID_TOOLBAR_EXPORT, "Export");
		//AddTextToButton(ID_TOOLBAR_PRINT,	"Print");
		//AddTextToButton(ID_TOOLBAR_INSPECT,"Inspect");
		//AddTextToButton(ID_TOOLBAR_FILTER, "Filters");
		//AddTextToButton(ID_TOOLBAR_HELP,	"Help");
		AddTextToButton(ID_TOOLBAR_PRINT1,	"Print");
		AddTextToButton(ID_TOOLBAR_HELP1,	"Help");
		//TD18095, jianghp, after fixed performance issue, change buttonsize to 54.
		//ESV gui is incorrect.There is 2 filter button when it is loading and it will change to 2 help button after it is loaded.
		//m_WndToolBar.GetToolBarCtrl().SetButtonSize(CSize(54,40));	
	}

	LRESULT CEquipmentStatusViewerDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
	{		
		switch(message)
		{		
			case WM_NCMOUSEMOVE: case WM_MOUSEMOVE: case WM_COMMAND:
			{		
				if (m_isToolTip)
				{
					POINTS point = MAKEPOINTS(lParam);
					
					CRect rect;
					m_WndToolBar.GetClientRect(&rect);

					if (!((point.x > rect.left) && (point.x < rect.right) &&
						(point.y > rect.top) && (point.y < rect.bottom)))
					{
						//SetStatusBarText(0, "Ready");
						m_isToolTip = false;
					}
				};		
				break;
			}	
			//case WM_PAINT:
				//Resize();
		}
		return CDialog::WindowProc(message, wParam, lParam);
	}

	void CEquipmentStatusViewerDlg::AddTextToButton(UINT buttonId, CString buttonLabel )
	{
		int index = m_WndToolBar.CommandToIndex(buttonId);
		if( index >=0 )
		{
			m_WndToolBar.SetButtonText(index, buttonLabel);
		}
		else
		{
			TRACE1("Unable to find %s button\n", buttonLabel);
		}
	}

	void CEquipmentStatusViewerDlg::OnViewToolbar() 
	{
		bool isVisible = (m_WndToolBar.IsWindowVisible()==TRUE);
		m_WndToolBar.ShowWindow(isVisible ? SW_HIDE : SW_SHOWNA);
		
 		GetMenu()->CheckMenuItem(ID_VIEW_TOOLBAR,isVisible ? MF_UNCHECKED : MF_CHECKED);
		RedrawWindow(NULL,NULL,RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_FRAME);

		Resize();
	}

	void CEquipmentStatusViewerDlg::OnViewStatusbar() 
	{
		bool isVisible = (m_WndStatusBar.IsWindowVisible() == TRUE);
		m_WndStatusBar.ShowWindow(isVisible ? SW_HIDE : SW_SHOWNA);
 		
		GetMenu()->CheckMenuItem(ID_VIEW_STATUSBAR,isVisible ? MF_UNCHECKED : MF_CHECKED);
		RedrawWindow(NULL,NULL,RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_FRAME);

		Resize();
	}


	/*void CEquipmentStatusViewerDlg::OnEditConfigurereport() 
	{
        CWaitCursor waitCursor;
		CString locationName, reportName;
		unsigned long locationID = 0;
        std::string reportID = "0";

		m_Location.GetWindowText(locationName);
		m_ReportName.GetWindowText(reportName);
		locationID = m_Location.GetItemData ( m_Location.GetCurSel() );

		//look for the actual string id in the map		
		std::map<int, std::string>::iterator itr;
		itr = m_reportKeyStringToIdMap.find(m_ReportName.GetItemData(m_ReportName.GetCurSel()));
		if (itr != m_reportKeyStringToIdMap.end())
		{
			reportID = itr->second;
		}
		else
		{
			return;
		}

        if (locationID != OCC_LOCATION_KEY_IN_DATABASE)
        {
            CConfigurationDialog dlg( locationID, reportID, 
                (LPCTSTR) locationName, (LPCTSTR) reportName, m_progressBar, this );
		    if (dlg.DoModal() == IDOK)
		    {
			    // report must have changed
			    PopulateEquipmentStatusList(reportID);
		    }
        }
        else
        {
            CConfigurationDialog dlg( locationID, (LPCTSTR) locationName, reportID, 
                (LPCTSTR) reportName, m_progressBar, this , NULL);
		    if (dlg.DoModal() == IDOK)
		    {
			    // report must have changed
			    PopulateEquipmentStatusList(reportID);
		    }
        }

	}*/


	void CEquipmentStatusViewerDlg::OnExport() 
	{
        CWaitCursor waitCursor;
/*
        // TD12051: Make sure the configured export directory is valid.
        CExportFileDialog exportFileDlg;

        if (exportFileDlg.getExportDirectory().IsEmpty())
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << TA_Base_Core::RunParams::getInstance().get(RPARAM_EXPORTDIR);
            userMsg.showMsgBox(IDS_UE_083009);
        }
*/
	// Allocate storage for szSelections
		//TD17888 marc_bugfix
		CString fileExtension = ".csv";
		CString fileName = "Untitled";


		CString m_defaultExportPath(TA_Base_Core::RunParams::getInstance().get(RPARAM_EXPORTDIR).c_str());

	    if (m_defaultExportPath.Right(1) == "/" || m_defaultExportPath.Right(1) == "\\")
		{
        
			m_defaultExportPath = m_defaultExportPath.Left( m_defaultExportPath.GetLength()-1);
		}

		// Show print dialog for user to choose a file name
		TA_Base_Bus::FileDialogArialDisableFolders exportFileDlg(FALSE,            // We want a Save dialog not an Open one
					fileExtension,           // The default file extension
					fileName,       // This is the initial path and file name
					OFN_HIDEREADONLY | // Hide the readonly check box
					OFN_OVERWRITEPROMPT | // Prompt to ask the user if they want to overwrite existing files
					OFN_PATHMUSTEXIST | // Check that the path entered is correct and warn if it is not
					OFN_NOCHANGEDIR,
					"CSV (Comma delimited) (*.csv)|*.csv||",	// Only allow them to save in csv format
					AfxGetMainWnd());			  // Parent window is the actual alarm viewer

		exportFileDlg.m_ofn.lpstrTitle = "Save Report As";

		// Open the file read-only. Create and set size to 0. Will overwrite existing files.
		if (m_defaultExportPath.IsEmpty()) // Path set in configuration is empty
		{
			TCHAR szMaxpath[MAX_PATH];

		    if (SHGetSpecialFolderPath(NULL, szMaxpath, CSIDL_PERSONAL, FALSE))
			{
				m_defaultExportPath = szMaxpath;
				exportFileDlg.m_ofn.lpstrInitialDir = m_defaultExportPath;
			}

		}

		CString pathName =  m_defaultExportPath + fileName;
		
		CFile outFile;
		// Check the path and file name given for availability
		if ( 0 == outFile.Open(pathName, CFile::modeCreate | CFile::modeReadWrite) )
		{

			TCHAR szMaxpath[MAX_PATH];

			if (SHGetSpecialFolderPath(NULL, szMaxpath, CSIDL_PERSONAL, FALSE))
			{
				m_defaultExportPath = szMaxpath;
				exportFileDlg.m_ofn.lpstrInitialDir = m_defaultExportPath;

			}
		}
		else
		{
			exportFileDlg.m_ofn.lpstrInitialDir = m_defaultExportPath;
			// remove the file if it is created
			if (outFile != NULL) 
			{	
				outFile.Close();
				CFile::Remove(pathName);
			}
		}


		CString locationName, reportName;

		m_Location.GetWindowText(locationName);
		m_ReportName.GetWindowText(reportName);

		// show the comment dialog
		CReportCommentDialog commentDlg;

		if (commentDlg.DoModal() == IDOK)
		{
            // Get the export file path.
            if (IDOK == exportFileDlg.DoModal())
			{
                CString fileNameFullPath = exportFileDlg.GetPathName();
				CFileUtility fileUtil;

				fileUtil.SetFileName(fileNameFullPath);
				fileUtil.SetLocationName(locationName);
				fileUtil.SetReportName(reportName);
				fileUtil.SetReportComment(commentDlg.GetComment());

				if (!fileUtil.ExportToCSV((CListCtrl *)&m_EquipmentStatusList))
				{
                    TA_Base_Bus::TransActiveMessage userMsg;
                    userMsg.showMsgBox(IDS_UE_083006);
                }
                else
				{
                    TA_Base_Bus::TransActiveMessage userMsg;
                    userMsg << fileNameFullPath;
                    userMsg.showMsgBox(IDS_UI_083007);
				}
   			}
		}

	}


	/*void CEquipmentStatusViewerDlg::OnFilter() 
	{
		// view existing filters and add additional ones for the current session if necessary
		// but DO NOT remove configured filters

        CWaitCursor waitCursor;
		CString locationName;
		CString reportName;

		m_Location.GetWindowText(locationName);
		m_ReportName.GetWindowText(reportName);
		//look for the actual string id in the map	
		std::string reportID;
		std::map<int, std::string>::iterator itr;
		itr = m_reportKeyStringToIdMap.find(m_ReportName.GetItemData(m_ReportName.GetCurSel()));
		if (itr != m_reportKeyStringToIdMap.end())
		{
			reportID = itr->second;
		}
		else
		{
			return;
		}

		CFilterDialog dlg( false,  // false means do not save filter to db
            m_currentFilters,
            reportID,
    		reportName,
            locationName,
            m_reportItems);

        if (dlg.DoModal() == IDOK)
		{
            m_isFiltered = IsFiltered();
			m_EquipmentStatusList.SetSortOrder(ASSET_COLUMN,true);//CL-17485--lkm
			m_EquipmentStatusList.SortColumn(ASSET_COLUMN);
            EquipmentStatusListItemMap::iterator iter = m_reportItems.begin();
            for ( ; iter != m_reportItems.end(); iter++)
            {
                iter->second->CheckFilters( m_currentFilters);
                iter->second->UpdateInList( m_EquipmentStatusList);
            }
		}
        UpdateStatusBar();
	}*/


	void CEquipmentStatusViewerDlg::OnPrint() 
	{
        CWaitCursor waitCursor;
		CReportCommentDialog dlg;

		if (dlg.DoModal() != IDOK)
		{
			return;
		}

		CString locDisplayName, repName;

		m_Location.GetWindowText(locDisplayName);
		m_ReportName.GetWindowText(repName);

        if ( ( ACTIVE_PROFILE_ALL_REPORT == m_selectedReportKey ) ||
             ( ACTIVE_PROFILE_MMS_REPORT == m_selectedReportKey ) )
        {
			TA_Base_Core::Session session(TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID));
            TA_Base_Core::ILocation* currentloc = NULL;
            try
		    {
                currentloc= TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey( session.getLocationKey());
                locDisplayName = currentloc->getDisplayName().c_str();
            }
            catch (...)
            {

            }

            if(currentloc != NULL)
			{
				delete currentloc;
			}
        }

		//we need set default sort(sort by Assert column and )
		bool bPreAssertSortOrder = m_EquipmentStatusList.GetSortOrder(ASSET_COLUMN);
		int  bPreSortColumn = m_EquipmentStatusList.getCurrentSortCol();
		bool bPreSortColumnSortOrder = m_EquipmentStatusList.GetSortOrder(bPreSortColumn);
		bool bReSorted = false;
		if(bPreSortColumn != ASSET_COLUMN || bPreAssertSortOrder != true)
		{
			bReSorted = true;
			m_EquipmentStatusList.SetSortOrder(ASSET_COLUMN, true);
			m_EquipmentStatusList.SortColumn(ASSET_COLUMN, false);
		}

		CListCtrlPrint cJob;
		cJob.pList = &m_EquipmentStatusList;

		// Set the header information.
		// ListCtrlPrint uses DT_WORDBREAK in CDC::DrawText() to enable multiple lines
		CString header = CString("Equipment Datapoint Status Report\r\n") + CTime::GetCurrentTime().Format("%a %#d/%#m/%Y %#H:%M:%S") + "\r\n";
		header+= (repName + " at " + locDisplayName + "\r\n");
		header+= "Title: ";
		header+= dlg.GetComment();
		cJob.csPageHead = header;
		cJob.csPage = "Page %d of %d";

		//	At present, the ES Report is monochrome, so the CellColourMap needs to be
        //  the same size as the list control but all cells are just black on white.
		TA_Base_Bus::CellColourMap colourMap( ESL_COLUMN_MAX, m_EquipmentStatusList.GetItemCount(),
            0,  //  the foreground colour: black
            m_EquipmentStatusList.GetBkColor() );
		cJob.pColourMap = &colourMap;

		// create a new col width array for print job to process the width
       	int * colWidths = new int [ ESL_COLUMN_MAX+1];
	    for (int i=0 ; i< ESL_COLUMN_MAX; i++ )
        {
            int wd = m_EquipmentStatusList.GetColumnWidth( i);

            if (wd <= 0)
            {   //  Hidden column
                if ( QUALITY_COLUMN == i)
                {
                    wd = m_EquipmentStatusList.GetColumnWidth( VALUE_COLUMN);
                }
                else
                {
                    if ( ACTIVE_PROFILE_MMS_REPORT == m_selectedReportKey )
                    {
                        //  Keep the other hidden columns hidden by making them very narrow
                        wd = 1;
                    }
                }
            }
			colWidths[i+1] = wd;
        }
		cJob.pColWidthArray = colWidths;

		cJob.OnFilePrint();
		if(bReSorted)
		{
			m_EquipmentStatusList.SetSortOrder(ASSET_COLUMN, bPreAssertSortOrder);
			m_EquipmentStatusList.SetSortOrder(bPreSortColumn, bPreSortColumnSortOrder);
			m_EquipmentStatusList.SortColumn(bPreSortColumn, false);
		}

		// write audit message
		CAuditMessageUtility auditLogWriter;
		std::string reportName(repName.GetBuffer(repName.GetLength()));
		std::string locationName(locDisplayName.GetBuffer(locDisplayName.GetLength()));
		auditLogWriter.LogAuditMessage(reportName, locationName, ESVAudit::PrintReportLog);
	}


	/*void CEquipmentStatusViewerDlg::OnNewreport() 
	{
        CWaitCursor waitCursor;
		CNewReport dlg;
        dlg.m_isAllowedToViewAllReports = m_isAllowedToViewAllReports;

		if (dlg.DoModal() == IDOK)
		{
			int indexOfNewLocation = -1;
			int indexOfNewReport = -1;

			// find the index of the new location
			for (int i= 0; i < m_Location.GetCount(); i++)
			{
				if (m_Location.GetItemData(i) == dlg.m_newLocationKey)
				{
					indexOfNewLocation = i;
					break;
				}
			}

			if (indexOfNewLocation == -1)
				return;

			m_Location.SetCurSel(indexOfNewLocation);
            m_selectedLocationKey = dlg.m_newLocationKey;
			PopulateReports(m_selectedLocationKey);

            SelectReportByKey( dlg.m_newReportKey);
		}
	}*/

	/*void CEquipmentStatusViewerDlg::OnEditReport() 
	{
        CWaitCursor waitCursor;
		EditReport dlg( m_selectedLocationKey, m_selectedReportKey );

		if (dlg.DoModal() == IDOK)
		{
			PopulateReports(m_Location.GetItemData(m_Location.GetCurSel()));
            std::string editedReportKey = m_selectedReportKey;
            m_selectedReportKey = "";   //  force a refresh of the equipment list
            SelectReportByKey( editedReportKey);
		}
	}*/

	void CEquipmentStatusViewerDlg::OnColumnClickEquipmentList(NMHDR* pNMHDR, LRESULT* pResult) 
	{
        CWaitCursor waitCursor;
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

		if (m_EquipmentStatusList.GetItemCount() > 0)
		{
			int nColumnClickedOn = pNMListView->iSubItem;

			m_EquipmentStatusList.SetSortOrder(nColumnClickedOn, !m_EquipmentStatusList.GetSortOrder(nColumnClickedOn));
			m_EquipmentStatusList.SortColumn(nColumnClickedOn);
		}

		*pResult = 0;
	}

	void CEquipmentStatusViewerDlg::PopulateLocations()
	{
		FUNCTION_ENTRY("CEquipmentStatusViewerDlg::PopulateLocations()");

		using TA_Base_Core::ILocation;
		using TA_Base_Core::DataException;
		using TA_Base_Core::DatabaseException;

		m_Location.ResetContent();
		std::vector<ILocation*> aLocations;	
		std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
		unsigned long locationKey = ULONG_MAX;
		int stepPosition = 0;
		bool isCentralProfile = false;
		try
        {
			TA_Base_Bus::SessionInfo sessionInfo = m_authenticationLibrary->getSessionInfo(sessionId, sessionId);
			stepPosition = 1;
			locationKey = sessionInfo.LocationId;
			m_curLoginLocation = locationKey;
			if(locationKey != 0)
			{
				std::vector<unsigned long > profileKeys = m_authenticationLibrary->getProfiles(sessionId);
				std::vector<unsigned long>::iterator profileIt;
				for (profileIt = profileKeys.begin(); profileIt != profileKeys.end(); profileIt++)
				{
					TA_Base_Core::IProfile* profile = m_profileAccessFactory.getProfile(*profileIt);

					if (!profile->requiresLocationToBeSelected())
					{
						isCentralProfile = true;
						break;
					}

					delete profile;
				}
			}
			stepPosition = 2;
			if (locationKey == 0 || isCentralProfile )																	
			{
			    // set to false by default for read only access
			    // set it to true if you want to edit the table and cast it to ConfigEquipReport
			    aLocations = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations(false);
			}
			else
			{
				ILocation* currentloc = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(locationKey);
				aLocations.push_back(currentloc);
			}
			
			stepPosition = 3;

			CDC* locationDc = m_Location.GetDC();
			int index = 0;
			int maxTextLen = 0;
			if (locationKey == 0 || isCentralProfile)
			{
				index = m_Location.AddString("ALL");
				m_Location.SetItemData(index, 0L);
				if (locationDc != NULL)
				{
					maxTextLen = locationDc->GetTextExtent("ALL").cx;
				}
			}

			for (std::vector<ILocation*>::iterator iter = aLocations.begin(); iter != aLocations.end(); ++iter)
			{
				try
				{
					std::string locDisplayName = (*iter)->getDisplayName();
					std::string locName = (*iter)->getName();
					// OCC need get rid of TDS location
					bool isTDSLocAndInOCC = (locName == "TDS" && (isCentralProfile || locationKey == 0));
					// TES #759 - 'Unallocated' location should be filtered out
					if ( (*iter)->getKey() > 0 && !isTDSLocAndInOCC)
					{
						index = m_Location.AddString(locDisplayName.c_str());
						m_Location.SetItemData(index,(*iter)->getKey());
						if (locationDc != NULL)
						{
							int textLen = locationDc->GetTextExtent(locDisplayName.c_str()).cx;
							if (textLen > maxTextLen)
							{
								maxTextLen = textLen;
							}
						}
					}
				}
				catch (...)
				{

				}

				delete *iter;
				*iter = NULL;
			}
			
			if (locationDc != NULL)
			{
				int spaceTextLen = locationDc->GetTextExtent(_T(" ")).cx * 2;
				m_Location.SetDroppedWidth(maxTextLen + spaceTextLen);
				m_Location.ReleaseDC(locationDc);
				locationDc = NULL;
			}

        }
        catch (...)
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"PopulationLocation catch exception, position:%d", stepPosition);
        }
		FUNCTION_EXIT;
	}

	/*void CEquipmentStatusViewerDlg::PopulateReports(const unsigned long aLocationKey)
    {
        CWaitCursor waitCursor;
        //SetStatusBarText(1,"Filters : OFF");
        //SetStatusBarText(2,"");

        // a new location has been chosen so a list of reports has been loaded
        // this means we need to clear datanodes/datapoints vector
        OnStop(); // stops and removes any further queued updates still to be processed

        m_EquipmentStatusList.DeleteAllItems();

        m_ReportName.ResetContent();
        //clear the current string to id map first
		m_reportKeyStringToIdMap.clear();

        int maxTextLen = 0;

        CDC* reportNameDc = m_ReportName.GetDC();
        if ( NULL == reportNameDc)
        {
            return;
        }

        int spaceTextLen = reportNameDc->GetTextExtent(_T(" ")).cx * 2;

        if (aLocationKey > 0L)
        {
            using TA_Base_Core::IEquipReport;
            using TA_Base_Core::DataException;
            using TA_Base_Core::DatabaseException;

            unsigned long profileLocation = 0;
            std::vector<unsigned long> profileKeys;
            bool central = false;

            // This bit is expensive - don't do it if m_isAllowedToViewAllReports is true
            if (false == m_isAllowedToViewAllReports)
            {
                TA_Base_Core::Session session(TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID));
                profileLocation = session.getLocationKey();

                // Need to get all the profiles for this session
                try
                {
                    profileKeys =
                        m_authenticationLibrary->getProfiles(
                        TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID));

                    // Still not enough, because we need to know if there is a central profile
                    std::vector<unsigned long>::iterator profileIt;
                    for (profileIt = profileKeys.begin(); profileIt != profileKeys.end(); profileIt++)
                    {
                        TA_Base_Core::IProfile* profile = m_profileAccessFactory.getProfile(*profileIt);

                        if (!profile->requiresLocationToBeSelected())
                        {
                            central = true;
                        }

                        delete profile;
                    }
                }
                catch (...)
                {
                }
            }

            // OK to populate if we have a central profile, local profile with matching location or
            // we have the right to view all reports
            if (central || profileLocation == aLocationKey || m_isAllowedToViewAllReports)
            {
                std::vector<IEquipReport*> aEquipReports;

                try
                {
                    aEquipReports = TA_Base_Core::EquipReportAccessFactory::getInstance(). getAllEquipReportsByLocationKey(aLocationKey);
                }
                catch (...)
                {
                    m_ReportName.SetDroppedWidth(spaceTextLen);
                    AdjustGUIWhileLoading(true);
                    return;
                }

                for (std::vector<IEquipReport *>::iterator iter = aEquipReports.begin(); iter != aEquipReports.end(); ++iter)
                {
                    try
                    {
                        // Must have same profile to access reports.
                        CString repName((*iter)->getName().c_str());
                        // Only put report in list if profiles match or has right to view all reports
                        unsigned long tempProfile = (*iter)->getProfile();
                        bool showReport = m_isAllowedToViewAllReports;

                        std::vector<unsigned long>::iterator profileIt;
                        for (profileIt = profileKeys.begin();
                            profileIt != profileKeys.end() && showReport == false; 
                            profileIt++)
                        {
                            if ((*profileIt) == tempProfile)
                            {
                                showReport = true;
                                break;
                            }
                        }

                        if(showReport)
                        {
                            repName.Replace("''","'");
                            int extent = reportNameDc->GetTextExtent(repName).cx;
                            if (maxTextLen < extent)
                                maxTextLen = extent;

                            int index = m_ReportName.AddString(repName);
							m_reportKeyStringToIdMap.insert(std::pair<int, std::string>(index, (*iter)->getId()));
                            m_ReportName.SetItemData(index,index);
                        }
                    }
                    catch (...)
                    {
                    }

                    delete *iter;
                    *iter = NULL;
                }
            }
        } 
        else if (aLocationKey == 0L)
        {
            for( int loopIndex = ACTIVE_PROFILES_FIRST; loopIndex < MAX_ACTIVE_PROFILES; ++loopIndex )
            {
                std::string loopIndexId = "";
                if (loopIndex == ACTIVE_PROFILE_ALL_DATA)
                {
                    loopIndexId = ACTIVE_PROFILE_ALL_REPORT;
                }
                else if (loopIndex == ACTIVE_PROFILE_MMS_DATA) 
                {
                    loopIndexId = ACTIVE_PROFILE_MMS_REPORT;
                }
                int index = m_ReportName.AddString(m_activeProfileNameConstants[(Active_Profile)loopIndex]);
				m_reportKeyStringToIdMap.insert(std::pair<int, std::string>(index, loopIndexId));
                m_ReportName.SetItemData(index, index);

                int extent = reportNameDc->GetTextExtent(m_activeProfileNameConstants[(Active_Profile)loopIndex]).cx;

                if (maxTextLen < extent)
                    maxTextLen = extent ;
            }
        }

        maxTextLen += spaceTextLen;
        m_ReportName.SetDroppedWidth(maxTextLen);

        m_ReportName.ReleaseDC(reportNameDc);
        reportNameDc = NULL;

        AdjustGUIWhileLoading(true);
    }*/


    /*void CEquipmentStatusViewerDlg::PopulateEquipmentStatusList(std::string& aReportKey, bool aInitProxies)
    {
        CWaitCursor waitCursor;
        //SetStatusBarText(2, "");
		AdjustGUIWhileLoading(false);

		//CL-20798 zhangjunsheng
        ///std::vector<DataNodeAgentProxy*>::iterator  dnagentitr;
        //for (dnagentitr = m_DatanodeAgentProxy.begin(); dnagentitr != m_DatanodeAgentProxy.end() ; dnagentitr++)
        //{
            //delete (*dnagentitr);
        //}
        //m_DatanodeAgentProxy.clear();

        // Empty the vector of entity keys; these keys will be used to fill the reportItems map
  		m_unprocessedEntities.clear();

        // Empty the map of report items (these encapsulate the datanode/point proxies)
        m_reportItems.clear();
        m_EquipmentStatusList.DeleteAllItems();


        //m_progressBar.SetMessage( "Reading database...");
        //m_progressBar.SetRange( 0, 20, 1); //  set the progress bar so it increments a little while loading the EquipReport items
        //m_progressBar.SetPos( 0);
        CString statusMessage;

        m_EquipmentStatusList.prepareColumns( aReportKey);

        if ( ( ACTIVE_PROFILE_ALL_REPORT == aReportKey ) || ( ACTIVE_PROFILE_MMS_REPORT == aReportKey ) )
        {
            //  Prepare to display ALL equipment at the current profile's location

    		IEntityDataList entityDatas;
			TA_Base_Core::Session session(TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID));
            unsigned long locationKey = session.getLocationKey();

			 //zhangjunsheng TD CL_20798
			unsigned long nProfileKey = session.getProfileKey();
			TA_Base_Core::IProfile* profile = TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(nProfileKey);

            //TD13200 Mintao++
            std::string locationName;
			 //zhangjunsheng TD CL_20798
			if((0 == locationKey) || (! profile->requiresLocationToBeSelected()))
            //if (locationKey == 0 || ) //locationkey 0 means at all location
            {
                //statusMessage.Format( "Loading all equipment at All location");
                //m_progressBar.SetMessage( statusMessage );
                try
                {
                    //m_progressBar.StepIt();

					std::string newAgentName="";
					std::string oldAgentName="";
					TA_Base_Core::CorbaNameList corbaNames = EntityAccessFactory::getInstance().getCorbaNamesOfType("DataNode",true);
					TA_Base_Core::CorbaNameList::iterator corbaitr;
					for (corbaitr = corbaNames.begin(); corbaitr != corbaNames.end(); ++corbaitr)
					{
						newAgentName = (*corbaitr).getAgentName();
						if ( newAgentName != oldAgentName )
						{
							oldAgentName = newAgentName;
							if ( newAgentName != "" )
							{
								std::map< std::string, DataNodeAgentProxy* >::iterator itr;
								itr = m_DatanodeAgentProxy.find( newAgentName );
								if ( itr != m_DatanodeAgentProxy.end() )
								{
									(*itr).second->setToBadNotConnectedQuality();
								}
								else
								{
									TA_Base_Core::CorbaNameList toinsert;
									toinsert.push_back((*corbaitr));
									m_DatanodeAgentProxy.insert( std::pair< std::string, DataNodeAgentProxy* >(newAgentName, new DataNodeAgentProxy(toinsert, this)) );
									
									itr = m_DatanodeAgentProxy.find( newAgentName );
									(*itr).second->start();
								}
							}
						} 
					}

                    //m_progressBar.StepIt();
                }
                catch (...)
                {
                    //m_progressBar.Clear();
                    TA_Base_Bus::TransActiveMessage userMsg;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_083004);
		            AdjustGUIWhileLoading(true);    //  enable the comboboxes to let the user try again
                    return;
                }
                //TD13200 Mintao++
            }
            else
            {
                TA_Base_Core::ILocation* currentloc = NULL;
                try
                {
                    currentloc = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey( locationKey);
                    locationName = currentloc->getName();
                }
                catch (...) 
                {
                }

				if(currentloc != NULL)
				{
					delete currentloc;
				}
                //statusMessage.Format( "Loading all equipment at %s", locationName.c_str());
                //m_progressBar.SetMessage( statusMessage );

				try
				{
					std::string oldAgentName( "" );
					std::string newAgentName;
					//TA_Base_Core::CorbaNameList corbaNames = EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation("DataNodeAgent", locationName,true);
					TA_Base_Core::CorbaNameList corbaNames = EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation("DataNode", locationName,true);
					TA_Base_Core::CorbaNameList::iterator corbaitr;
					for (corbaitr = corbaNames.begin(); corbaitr != corbaNames.end(); ++corbaitr)
					{
						newAgentName = (*corbaitr).getAgentName();
						if ( newAgentName!=oldAgentName )
						{
							oldAgentName = newAgentName;
							if ( newAgentName!="" )
							{
								std::map< std::string, DataNodeAgentProxy* >::iterator itr;
								itr = m_DatanodeAgentProxy.find( newAgentName );
								if ( itr!=m_DatanodeAgentProxy.end() )
								{
									(*itr).second->setToBadNotConnectedQuality();
								}
								else
								{
									TA_Base_Core::CorbaNameList toinsert;
									toinsert.push_back((*corbaitr));
									m_DatanodeAgentProxy.insert( std::pair< std::string, DataNodeAgentProxy* >(newAgentName, new DataNodeAgentProxy(toinsert, this)) );

									itr = m_DatanodeAgentProxy.find( newAgentName );
									(*itr).second->start();
								}
							}
						} 
					}
					//m_progressBar.StepIt();
				}
				catch (...)
				{
					//m_progressBar.Clear();
                    TA_Base_Bus::TransActiveMessage userMsg;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_083004);
					AdjustGUIWhileLoading(true);    //  enable the comboboxes to let the user try again
                    return;
				}
            }

    		//m_StopButton.ShowWindow(SW_SHOW);
        }
        else
        {
            //  Prepare to display a user-configured report

            statusMessage = "Loading report...";
            try
            {
                //statusMessage.Format( "Loading report %s", TA_Base_Core::EquipReportAccessFactory::getInstance().getEquipReportByKey(aReportKey)->getName().c_str());
                //m_progressBar.SetMessage( statusMessage );
            }
            catch (...)
            {
                //  Not worth stopping just because of the status bar text (it will probably get caught below anyway)
            }

            //  Get the Datanode/Datapoint entity keys to be watched through this Equipment Report
            std::vector<IEquipReportData *> equipReportRows;

            try
            {
                equipReportRows = TA_Base_Core::EquipReportDataAccessFactory::getInstance().getAllEquipReportDatumByReportKey(aReportKey);
            }
            catch (...)
            {
                //m_progressBar.Clear();
                TA_Base_Bus::TransActiveMessage userMsg;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_083004);
		        AdjustGUIWhileLoading(true);    //  enable the comboboxes to let the user try again
                return;
            }

            //m_progressBar.StepIt();

            //  Now fill the entity key vector 

            for (std::vector<IEquipReportData *>::const_iterator it = equipReportRows.begin(); it != equipReportRows.end(); it++)
            {
                if (0 < (*it)->getDataNodeKey())
                {
                    m_unprocessedEntities.push_back( EquipmentReportEntity( DATANODE, (*it)->getDataNodeKey(), (*it)->getId() ));
                }
                else
                {
                    m_unprocessedEntities.push_back( EquipmentReportEntity( DATAPOINT, (*it)->getDataPointKey(), (*it)->getId()));
                }
            }
        }

        //m_progressBar.StepIt();
        //m_progressBar.SetRange( 0, m_unprocessedEntities.size());
        
        //TD13200
        //Mintao++
        if ( ( ACTIVE_PROFILE_ALL_REPORT != aReportKey ) && ( ACTIVE_PROFILE_MMS_REPORT != aReportKey ) )
        {
            AdjustGUIWhileLoading(true);
        }
	}*/

	void CEquipmentStatusViewerDlg::PopulateEquipmentStatusList(unsigned long locationKey)
    {
        CWaitCursor waitCursor;
        SetStatusBarText(2, "");
		AdjustGUIWhileLoading(false);

		//1. stop DataNodeAgent from polling
		std::map< std::string, DataNodeAgentProxy* >::iterator itDnAgent;
		for(itDnAgent = m_DatanodeAgentProxy.begin(); itDnAgent != m_DatanodeAgentProxy.end(); ++itDnAgent)
		{
			(itDnAgent->second)->pausePoll();
		}

		//2. terminate DnUpdateProcessor
		m_DnUpdateProcessor->terminateAndWait();

		//3. clear resource
        m_reportItems.clear();

		//4. reset UI
        m_EquipmentStatusList.DeleteAllItems();
		m_EquipmentStatusList.prepareColumns(m_selectedReportKey);
    
		//5. reset and restart DnUpdateProcessor
		m_DnUpdateProcessor->clearQueue();
		m_DnUpdateProcessor->start();

		//6. recalculate DataNodeAgentProxy accroding to new locationKey
		try
        {
			unsigned long timeBegin = GetTickCount();
			std::vector<std::string> AgentNames = EntityAccessFactory::getInstance().getAgentNamesOfTypeAtLocation("DataNode",locationKey);
			unsigned long timeEnd = GetTickCount(); 

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"get agent count:%d at location:%d cost time:%d", 
				AgentNames.size(), locationKey, (timeEnd-timeBegin));

			std::vector<std::string>::iterator itAgentName;
			std::map< std::string, DataNodeAgentProxy* >::iterator itDnAgentProxy;

			for(itAgentName = AgentNames.begin(); itAgentName != AgentNames.end(); ++itAgentName)
			{
				if (*itAgentName != PARENT_ENTITY_NAME)
				{
					itDnAgentProxy = m_DatanodeAgentProxy.find(*itAgentName);
					if(itDnAgentProxy != m_DatanodeAgentProxy.end())
					{
						(itDnAgentProxy->second)->setToBadNotConnectedQuality();
						(itDnAgentProxy->second)->resumePoll();
					}
					else
					{
						DataNodeAgentProxy* pDnAgentProxy = new DataNodeAgentProxy(*itAgentName, this);
						m_DatanodeAgentProxy[*itAgentName] = pDnAgentProxy;
						pDnAgentProxy->start();
					}
				}
				else
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
						"agent name is 'Parent Entity'");
				}
			}
        }
        catch (...)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_083004);
        }

		AdjustGUIWhileLoading(true);
	}

	LRESULT CEquipmentStatusViewerDlg::OnSessionIdChange(WPARAM wParam, LPARAM lParam)
	{
		LRESULT result(0);
		CWaitCursor waitCursor;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Handle sessionId changed message");

		std::map< std::string, DataNodeAgentProxy* >::iterator itDnAgent;
		for(itDnAgent = m_DatanodeAgentProxy.begin(); itDnAgent != m_DatanodeAgentProxy.end(); ++itDnAgent)
		{
			(itDnAgent->second)->pausePoll();
		}
		m_DnUpdateProcessor->terminateAndWait();
        m_reportItems.clear();
        m_EquipmentStatusList.DeleteAllItems();
        //reset the report name combo box
        m_ReportName.ResetContent();
        //reset the location and reload the location combo box
        m_selectedLocationKey = -1;
	    PopulateLocations();
        checkPermissions();
		
		return result;
	}

	LRESULT CEquipmentStatusViewerDlg::OnListItemUpdate(WPARAM wParam, LPARAM lParam)
	{
        CWaitCursor waitCursor;

        unsigned long entityKey( wParam);
		
		EquipmentStatusListItemSharedPtr itemPtr;
		{
			TA_THREADGUARD(m_reportItemsLock);
			EquipmentStatusListItemMap::iterator iter = m_reportItems.find( entityKey);
			if ( iter != m_reportItems.end() )
			{
				itemPtr = iter->second;
			}
		}
		
        LRESULT result(0);
        if ( itemPtr.get() != NULL )
        {
            itemPtr->CheckFilters( m_currentFilters);
            result = itemPtr->UpdateInList( m_EquipmentStatusList);

            if ( itemPtr->isToBeDeleted() )
            {
                //  Probably a DataNode with child datanodes (ie. Location, system or subsystem)
                //  Take a copy of the shared pointer to keep its object alive until the end of this block.
                // EquipmentStatusListItemSharedPtr eslPtr = it->second;
                // TA_Base_Core::ThreadGuard guard( eslPtr->m_lock );
                // m_reportItems.erase( it);
            }
        }

        UpdateStatusBar();

		//here we use post message instead of calling m_EquipmentStatusList.SortColumn()
		//directly to avoid too many call sorting m_EquipmentStatusList, it is a better way
		//when a sudden Item Update storm appear.
        if (m_EquipmentStatusList.getCurrentSortCol() != ESL_COLUMN_MAX && !m_needSortListCtrl)
        {
			m_needSortListCtrl = true;
			PostMessage(WM_SORT_LIST_CTRL);
        }

        return result;
	}

	LRESULT CEquipmentStatusViewerDlg::OnSortListCtrl(WPARAM wParam, LPARAM lParam)
	{
		LRESULT result(0);
		CWaitCursor waitCursor;
		m_EquipmentStatusList.SortColumn(m_EquipmentStatusList.getCurrentSortCol());
		m_needSortListCtrl = false;
		return result;
	}

	LRESULT CEquipmentStatusViewerDlg::onLaunchActive(WPARAM wParam, LPARAM lParam)
	{
		m_Location.SetCurSel(0);
		OnSelchangeLocationCombo();

		return 1;
	}

    void CEquipmentStatusViewerDlg::CombineFilters(std::string& aReportKey)
	{
		int counter = 0;

		// clear our current filters
		ClearCurrentFilters();

		// clear db filters 
		UnloadFiltersFromDB();

		// load configured filters from database
		if (!LoadFiltersFromDB(aReportKey))
			return;

        m_currentFilters = m_storedFilters;

		// unload database filters as they are not needed anymore
		UnloadFiltersFromDB();
	}

	void CEquipmentStatusViewerDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
	{
		TransActiveDialog::OnShowWindow(bShow, nStatus);

		Resize();

        //TD16889 Mintao 
        //remove the not working code
        /*
		bool isActiveProfile = RunParams::getInstance().isSet("ShowActiveProfile");
		if (isActiveProfile)
			PostMessage(WM_LAUNCH_ACTIVE, 0L, 0L);
        */

	}

	void CEquipmentStatusViewerDlg::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu) 
	{
		CDialog::OnMenuSelect(nItemID, nFlags, hSysMenu);

		TCHAR szFullText[256];
		CString cstStatusText;

		if ((nItemID > 3))			// NOTE : Ignore first 3 menu popups
		{
			LoadString(AfxGetResourceHandle(), nItemID, szFullText, 256);

			AfxExtractSubString(cstStatusText, szFullText, 0, '\n');
			//SetStatusBarText(0, cstStatusText);
		} 
		else
		{
			//SetStatusBarText(0, "Ready");
		}
	}


	/*void CEquipmentStatusViewerDlg::OnFileDeletereport() 
	{
        CWaitCursor waitCursor;
		CString report;
		m_ReportName.GetWindowText(report);

		CString locationName;
		m_Location.GetWindowText(locationName);
        CString actionName;
        actionName.LoadString(IDS_DELETE_REPORT);
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << actionName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UW_010003);

        // TD #3230: Changed to OK/Cancel dialog
        // sant: changed to Yes/No to be consistent with the rest of the system messages
		if (selectedButton == IDYES)
		{
			//look for the actual string id in the map	
			std::string reportIdToDelete;
			std::map<int, std::string>::iterator itr;
			itr = m_reportKeyStringToIdMap.find(m_ReportName.GetItemData(m_ReportName.GetCurSel()));
			if (itr != m_reportKeyStringToIdMap.end())
			{
				reportIdToDelete = itr->second;
			}
			else
			{
				return;
			}
			unsigned long locationIdToRefresh = m_Location.GetItemData(m_Location.GetCurSel());

			// delete the selected report
			using TA_Base_Core::IConfigEquipReport;

			IConfigEquipReport *reportToDelete = NULL;
			try
			{
				reportToDelete = dynamic_cast<IConfigEquipReport *>(TA_Base_Core::EquipReportAccessFactory::getInstance().getEquipReportByKey(reportIdToDelete, true));
			}
			catch (...)
			{
				delete reportToDelete;
				return;
			}

			reportToDelete->deleteThisEquipReport();

			delete reportToDelete;
			reportToDelete = NULL;

			m_EquipmentStatusList.DeleteAllItems();

			//SetStatusBarText(0,"Ready");
			//SetStatusBarText(1,"Filters : OFF");
			//SetStatusBarText(2,"");

			ClearCurrentFilters();

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << report;
            UINT selectedButton = userMsg.showMsgBox(IDS_UI_083011);
            
			// refresh the reports combo box
			PopulateReports(locationIdToRefresh);

			// write audit log message
			CAuditMessageUtility auditMsgWriter;
			std::string locName(locationName.GetBuffer(locationName.GetLength()));
			std::string repName(report.GetBuffer(report.GetLength()));
			auditMsgWriter.LogAuditMessage(repName, locName, ESVAudit::DeleteReportLog);

		}
	}*/

	void CEquipmentStatusViewerDlg::OnHelpEquipmentstatusviewerhelp() 
	{

		TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
	}

	void CEquipmentStatusViewerDlg::OnToolbarHelp() 
	{
		OnHelpEquipmentstatusviewerhelp();
	}

	/*void CEquipmentStatusViewerDlg::CreateStatusBar()
	{
		if (!m_WndStatusBar.CreateEx(this, SBT_TOOLTIPS, WS_CHILD | WS_VISIBLE  | CBRS_BOTTOM, AFX_IDW_STATUS_BAR) ||
			!m_WndStatusBar.SetIndicators(indicators, 3))
		{
			TRACE0("Failed to create status bar\n");
		}

		m_WndStatusBar.SetPaneInfo(0, m_WndStatusBar.GetItemID(0), SBPS_STRETCH | SBPS_POPOUT | SBPS_NOBORDERS, NULL);
		m_WndStatusBar.SetPaneInfo(1, m_WndStatusBar.GetItemID(1), SBPS_NORMAL, 84);
		m_WndStatusBar.SetPaneInfo(2, m_WndStatusBar.GetItemID(2), SBPS_NORMAL, 120);

		SetStatusBarText(0, "Ready");
		SetStatusBarText(1,"Filters : OFF");
		SetStatusBarText(2,"");
	}*/

	void CEquipmentStatusViewerDlg::CreateStatusBar()
	{
		if (!m_WndStatusBar.CreateEx(this, SBT_TOOLTIPS, WS_CHILD | WS_VISIBLE  | CBRS_BOTTOM | CBRS_RIGHT, AFX_IDW_STATUS_BAR) ||
			!m_WndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(indicators[0])))
		{
			TRACE0("Failed to create status bar\n");
		}

		m_WndStatusBar.SetPaneInfo(0, m_WndStatusBar.GetItemID(0), SBPS_STRETCH | SBPS_POPOUT | SBPS_NOBORDERS, NULL);
		m_WndStatusBar.SetPaneInfo(1, m_WndStatusBar.GetItemID(1), SBPS_NORMAL, 84);
		m_WndStatusBar.SetPaneInfo(2, m_WndStatusBar.GetItemID(2), SBPS_NORMAL, 120);

		/*if (!m_WndStatusBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM | CBRS_RIGHT) ||
			!m_WndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(indicators[0])))
		{
			TRACE0("Failed to create status bar\n");
		}
		m_WndStatusBar.SetPaneInfo(0, m_WndStatusBar.GetItemID(0), SBPS_STRETCH | SBPS_POPOUT, NULL);*/

		SetStatusBarText(0,"");
		SetStatusBarText(1,"");
		SetStatusBarText(2,"");
	}

	void CEquipmentStatusViewerDlg::CalculateToolBarStatusBarHeights()
	{
		CRect rcClientStart;
		CRect rcClientNow;

		GetClientRect(rcClientStart);
		RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST, 0, reposQuery, rcClientNow);

		m_ToolBarHeight   = rcClientNow.top;
		m_StatusBarHeight = rcClientStart.bottom - rcClientNow.bottom;
	}

	void CEquipmentStatusViewerDlg::OnSelchangeLocationCombo() 
	{
        CWaitCursor waitCursor;
		// this represents a key from the database corresponding to this location
		unsigned long aSelectedLocationKey = m_Location.GetItemData(m_Location.GetCurSel());

		// if there is difference
		if ( m_selectedLocationKey != aSelectedLocationKey )
		{
			m_selectedLocationKey = aSelectedLocationKey;

			//PopulateReports ( m_selectedLocationKey );
			PopulateEquipmentStatusList(m_selectedLocationKey);

			// need to reset the report key since the user has changed the location.
			//m_selectedReportKey = CB_ERR;
		}
	}

	/*void CEquipmentStatusViewerDlg::OnSelchangeReportCombo() 
	{
        CWaitCursor waitCursor;
		//SetStatusBarText(1,"Filters : OFF");
		//SetStatusBarText(2,"");

		// this represents a key from the database corresponding to the selected report
		std::string aSelectedReportKey;
		std::map<int, std::string>::iterator itr;
		itr = m_reportKeyStringToIdMap.find(m_ReportName.GetItemData(m_ReportName.GetCurSel()));
		if (itr != m_reportKeyStringToIdMap.end())
		{
			aSelectedReportKey = itr->second;
		}
		else
		{
			return;
		}

		// continue if there is difference
		if ( m_selectedReportKey == aSelectedReportKey )
		{
            return;
        }
        m_selectedReportKey = aSelectedReportKey;

        // stops and removes any further queued updates still to be processed
        OnStop();

        if( (m_selectedReportKey == ACTIVE_PROFILE_ALL_REPORT) || (m_selectedReportKey == ACTIVE_PROFILE_MMS_REPORT) )
        {
            m_isActiveReport = true;
            ClearCurrentFiltersForActive();
        }
        else
        {
            m_isActiveReport = false;
            // new report, load filters from db and set them to current filters
            CombineFilters(m_selectedReportKey);
        }

        m_isFiltered = IsFiltered();
        PopulateEquipmentStatusList(m_selectedReportKey);
	}*/

	/*void CEquipmentStatusViewerDlg::OnEditConfigurefilter() 
	{
        CWaitCursor waitCursor;
		CString locationName;
		CString reportName;

		m_Location.GetWindowText(locationName);
		m_ReportName.GetWindowText(reportName);

		//look for the actual string id in the map		
		std::map<int, std::string>::iterator itr;
		std::string reportID;
		itr = m_reportKeyStringToIdMap.find(m_ReportName.GetItemData(m_ReportName.GetCurSel()));
		if (itr != m_reportKeyStringToIdMap.end())
		{
			reportID = itr->second;
		}
		else
		{
			return;
		}

		LoadFiltersFromDB( reportID);

		CFilterDialog dlg( true,  // true means we want to save filters to DB
            m_storedFilters,
            reportID,
    		reportName,
            locationName,
            m_reportItems);

        if (dlg.DoModal() == IDOK)
		{
			// configured filters have changed so update the current filters
			CombineFilters( reportID);

			m_isFiltered = IsFiltered();

			// need to process the equipment list again as the filters have changed
            EquipmentStatusListItemMap::iterator iter = m_reportItems.begin();
            for ( ; iter != m_reportItems.end(); iter++)
            {
                iter->second->CheckFilters( m_currentFilters);
                iter->second->UpdateInList( m_EquipmentStatusList);
            }
		}
		UnloadFiltersFromDB();
        UpdateStatusBar();
	}*/


	void CEquipmentStatusViewerDlg::OnExit() 
	{
        CWaitCursor waitCursor;
		
		CleanUpResource();

		TransActiveDialog::DestroyWindow();
	}


	void CEquipmentStatusViewerDlg::OnHelpAboutequipmentstatusviewer() 
	{
		TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();
	}


	void CEquipmentStatusViewerDlg::SetStatusBarText(int aIndex, const CString &message)
	{
		CDC *dc = m_WndStatusBar.GetDC();

		if (dc != NULL)
		{
			int width; 
			unsigned int a, b;

			m_WndStatusBar.GetPaneInfo(aIndex, a, b, width);

			int textExtent		= dc->GetTextExtent(message).cx;
			int spaceExtent		= dc->GetTextExtent(_T(" ")).cx;
			int numSpacesNeeded = ((width - textExtent) / 2) / spaceExtent;
			CString str(message);

			for (int i = 1; i <= numSpacesNeeded + 2; i++)
				str.Insert(0, " ");
			m_WndStatusBar.SetPaneText(aIndex, str);

			m_WndStatusBar.ReleaseDC(dc);
			dc = NULL;
		}
	}


	/*void CEquipmentStatusViewerDlg::OnEditInspect() 
	{
        CWaitCursor waitCursor;
		using TA_Base_Bus::AppLauncher;
		using TA_Base_Core::ApplicationException;
		using TA_Base_Core::DataPointEntityData;

		if (m_EquipmentStatusList.GetItemCount() == 0)
			return;

		// check if location selected
		if (m_Location.GetCurSel() == CB_ERR)
		{
            CString itemName;
            itemName.LoadString(IDS_LOCATION);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << itemName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_020066);
			m_Location.SetFocus();

			return;
		};

		// check if report name selected
		if (m_ReportName.GetCurSel() == CB_ERR)
		{
            CString itemName;
            itemName.LoadString(IDS_REPORT);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << itemName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_020066);

			m_ReportName.SetFocus();

			return;
		};

		// check if equipment list item selected
		POSITION pos = m_EquipmentStatusList.GetFirstSelectedItemPosition();
		if (pos == NULL)
		{
            CString itemName;
            itemName.LoadString(IDS_EQUIPMENT);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << itemName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_020066);
			return;
		}

        unsigned long entityKey = m_EquipmentStatusList.GetItemData(m_EquipmentStatusList.GetNextSelectedItem(pos));
        EquipmentStatusListItemSharedPtr esl = getReportItem( entityKey);

		if ( esl.get() == NULL)    //* *** TODO: get a better error message
		{
            CString itemName;
            itemName.LoadString(IDS_EQUIPMENT);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << itemName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_020066);
			return;
		}

		// launch Inspector Panel 
		try
		{
			IResource* resource = ResourceAccessFactory::getInstance().getResourceFromEntity(INSPECTOR_PANEL_GUI_APPTYPE);

			unsigned long appInspectorPanel = resource->getKey();

			delete resource;

			CString params("--entity-name=InspectorPanel --screen-name=INFORMATION --control-entity=");
			params.Insert(params.GetLength()+1, esl->getItemDataNodeEntityName().c_str());

			std::string commandLine = params.GetBuffer(params.GetLength());
            //Mintao ++ TD16199
            CPoint pt(GetMessagePos());
            using namespace TA_Base_Bus::TA_GenericGui;
            RECT boundary = TA_Base_Bus::AppLauncher::getInstance().getRect(SCREEN_CURRENT, AREA_SCHEMATIC, pt.x);
			//CL-17486--lkm
            TA_Base_Bus::AppLauncher::getInstance().launchApplication(appInspectorPanel, commandLine, POS_NONE,//POS_BOUNDED,
                                                                  ALIGN_HORIZONTAL | ALIGN_VERTICAL,
                                                                  NULL,
                                                                  &boundary);//CL-17486--lkm
            //Mintao ++ TD16199

		}
		catch (ScadaProxyException&)
		{
            CString applicationName;
            applicationName.LoadString(IDS_INSPECTOR_PANEL);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << applicationName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_010002);
		}

		catch (ApplicationException&)
		{
            CString applicationName;
            applicationName.LoadString(IDS_INSPECTOR_PANEL);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << applicationName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_010002);
		}

	}*/


	/*void CEquipmentStatusViewerDlg::OnToolbarInspect() 
	{
		OnEditInspect();
	}*/


	void CEquipmentStatusViewerDlg::OnClickEquipmentList(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		if (!m_StopButton.IsWindowVisible())
			AdjustGUI();

		*pResult = 0;
	}

	void CEquipmentStatusViewerDlg::AdjustGUI()
	{
		if (this->m_hWnd == NULL)
			return;

		// get control states as a base of all further decisions
		POSITION pos		= m_EquipmentStatusList.GetFirstSelectedItemPosition();
		int repNameIndex	= m_ReportName.GetCurSel();

		// get a reference to the toolbar control
		CToolBarCtrl &tlbCtrl = m_WndToolBar.GetToolBarCtrl();

		// get a pointer to the menu
		CMenu *menu = GetMenu();

		/*if (m_isAllowedToConfigure)
		{
			//menu->EnableMenuItem(ID_FILE_NEWREPORT, MF_ENABLED);
			//menu->EnableMenuItem(ID_FILE_EDITREPORT, MF_ENABLED);
		}
		else
		{
			//menu->EnableMenuItem(ID_FILE_NEWREPORT, MF_GRAYED);
			//menu->EnableMenuItem(ID_FILE_EDITREPORT, MF_GRAYED);
		}*/

		// if there is no report name selected
		if (repNameIndex == CB_ERR)
		{
			//menu->EnableMenuItem(ID_FILE_EDITREPORT, MF_GRAYED);
			//menu->EnableMenuItem(ID_FILE_DELETEREPORT, MF_GRAYED);
			//menu->EnableMenuItem(ID_FILE_EXPORT, MF_GRAYED);
			//menu->EnableMenuItem(ID_FILE_PRINT, MF_GRAYED);
			//menu->EnableMenuItem(ID_EDIT_CONFIGUREREPORT, MF_GRAYED);
			//menu->EnableMenuItem(ID_EDIT_CONFIGUREFILTER, MF_GRAYED);
			//menu->EnableMenuItem(ID_VIEW_FILTER, MF_GRAYED);

			//tlbCtrl.EnableButton(ID_TOOLBAR_EXPORT, FALSE);
			//tlbCtrl.EnableButton(ID_TOOLBAR_PRINT, FALSE);
			//tlbCtrl.EnableButton(ID_TOOLBAR_FILTER, FALSE);
			tlbCtrl.EnableButton(ID_TOOLBAR_PRINT1, FALSE);

		}	
		else
		{
			/*if ((m_isAllowedToConfigure == TRUE) && (m_isActiveReport == false))
			{
                //  TD 13290 Allow edit or delete of reports belonging only to this session's profile(s)
                UINT menuFlagConfigureEditDelete = MF_GRAYED;

			    try
			    {
                    //  Get the key of the profile that this report belongs to
                    unsigned long reportProfileKey = TA_Base_Core::EquipReportAccessFactory::getInstance().getEquipReportByKey(m_selectedReportKey)->getProfile();

			        // Need to get all the profiles for this session
    		        std::vector<unsigned long> profileKeys;
			        TA_Base_Core::Session session(TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID));

				    profileKeys =
					    m_authenticationLibrary->getProfiles(
						    TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID));

                    //  Check if the report's profile key matches any of this session's profiles
                    for ( std::vector<unsigned long>::const_iterator iter = profileKeys.begin(); iter != profileKeys.end(); iter++)
                    {
                        if ( *iter == reportProfileKey)
                        {
                            menuFlagConfigureEditDelete = MF_ENABLED;
                            break;
                        }
                    }
			    }
			    catch (...)
			    {
			    }

    			//menu->EnableMenuItem(ID_FILE_EDITREPORT, menuFlagConfigureEditDelete);
                //menu->EnableMenuItem(ID_FILE_DELETEREPORT, menuFlagConfigureEditDelete);
				//menu->EnableMenuItem(ID_EDIT_CONFIGUREREPORT, menuFlagConfigureEditDelete);
				//menu->EnableMenuItem(ID_EDIT_CONFIGUREFILTER, menuFlagConfigureEditDelete);
			}
			else
			{
    			//menu->EnableMenuItem(ID_FILE_EDITREPORT, MF_GRAYED);
				//menu->EnableMenuItem(ID_FILE_DELETEREPORT, MF_GRAYED);
				//menu->EnableMenuItem(ID_EDIT_CONFIGUREREPORT, MF_GRAYED);
				//menu->EnableMenuItem(ID_EDIT_CONFIGUREFILTER, MF_GRAYED);
			}*/

			if (m_isAllowedToPrintOrExport == FALSE)
			{
				//menu->EnableMenuItem(ID_FILE_EXPORT, MF_GRAYED);
				menu->EnableMenuItem(ID_FILE_PRINT, MF_GRAYED);

				//tlbCtrl.EnableButton(ID_TOOLBAR_EXPORT, FALSE);
				//tlbCtrl.EnableButton(ID_TOOLBAR_PRINT, FALSE);
				tlbCtrl.EnableButton(ID_TOOLBAR_PRINT1, FALSE);
			} 
			else
			{
				//menu->EnableMenuItem(ID_FILE_EXPORT, MF_ENABLED);
				menu->EnableMenuItem(ID_FILE_PRINT, MF_ENABLED);

				//tlbCtrl.EnableButton(ID_TOOLBAR_EXPORT, TRUE);
				//tlbCtrl.EnableButton(ID_TOOLBAR_PRINT, TRUE);
				tlbCtrl.EnableButton(ID_TOOLBAR_PRINT1, TRUE);
			}

			//menu->EnableMenuItem(ID_VIEW_FILTER, MF_ENABLED);
			//tlbCtrl.EnableButton(ID_TOOLBAR_FILTER, TRUE);
		}

		// if there is no equipment selected in the list ctrl
		// or if the item is not an equipment but rather a datapoint
		/*if (pos == NULL)
		{
			//tlbCtrl.EnableButton(ID_TOOLBAR_INSPECT, FALSE);
			//menu->EnableMenuItem(ID_EDIT_INSPECT, MF_GRAYED);
		} 
		else
		{
			//tlbCtrl.EnableButton(ID_TOOLBAR_INSPECT, TRUE);
			//menu->EnableMenuItem(ID_EDIT_INSPECT, MF_ENABLED);
		}*/

		m_WndToolBar.Invalidate();
		UpdateWindow();
	}


	void CEquipmentStatusViewerDlg::checkPermissions()
	{
		m_isAllowedToConfigure = false;
		m_isAllowedToPrintOrExport = false;
		m_isAllowedToViewAllReports = false;

		//
		// get the response from the ScadaUtilities
		//
        TA_Base_Bus::ScadaUtilities& scadaUtilities = TA_Base_Bus::ScadaUtilities::getInstance();
		// modified by hongzhi
		unsigned long locationKey = m_genericGUICallback.getGUIEntity()->getLocation();
		unsigned long physicalSubsystemKey = m_genericGUICallback.getGUIEntity()->getPhysicalSubsystem();
		unsigned long subsystemKey = m_genericGUICallback.getGUIEntity()->getSubsystem();
		if(physicalSubsystemKey<=0)
		{
			subsystemKey=physicalSubsystemKey;
		}

        m_isAllowedToConfigure = scadaUtilities.isActionAllowedWithLocSub( (unsigned int) TA_Base_Bus::aca_REPORT_CONFIGURATION_ESV,
                                        m_guiEntityKey, locationKey, subsystemKey, 
                                        TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID) );

        m_isAllowedToPrintOrExport = scadaUtilities.isActionAllowedWithLocSub ( (unsigned int) TA_Base_Bus::aca_PRINT_EXPORT_ESV,
                                        m_guiEntityKey, locationKey, subsystemKey, 
                                        TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID) );

        m_isAllowedToViewAllReports = scadaUtilities.isActionAllowedWithLocSub ( (unsigned int) TA_Base_Bus::aca_VIEW_ALL_REPORTS_ESV,
                                        m_guiEntityKey, locationKey, subsystemKey, 
                                        TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID) );

		AdjustGUI();
    }


    bool CEquipmentStatusViewerDlg::LoadFiltersFromDB(std::string& aReportKey)
	{
		using TA_Base_Core::IEquipReport;

		IEquipReport *aReport = NULL;

		try
		{
			aReport = TA_Base_Core::EquipReportAccessFactory::getInstance().getEquipReportByKey(aReportKey);
		}
		catch (...)
		{
			// something went wrong
			return false;
		}

        m_storedFilters.push_back( FilterElement( aReport->getReportTypeFilterFlag(), aReport->getReportTypeFilter()) );
        m_storedFilters.push_back( FilterElement( aReport->getAssetFilterFlag(), aReport->getAssetFilter()) );
        m_storedFilters.push_back( FilterElement( aReport->getDescFilterFlag(), aReport->getDescFilter()) );
        m_storedFilters.push_back( FilterElement( aReport->getDPNameFilterFlag(), aReport->getDPNameFilter()) );
        m_storedFilters.push_back( FilterElement( aReport->getDPTypeFilterFlag(), aReport->getDPTypeFilter()) );
        m_storedFilters.push_back( FilterElement( aReport->getDPValueFilterFlag(), aReport->getDPValueFilter()) );
        m_storedFilters.push_back( FilterElement( aReport->getStatusFilterFlag(), "") );
        m_storedFilters.push_back( FilterElement( aReport->getScanInhibitFlag(), "") );
        m_storedFilters.push_back( FilterElement( aReport->getAlarmInhibitFlag(), "") );
        m_storedFilters.push_back( FilterElement( aReport->getControlInhibitFlag(), "") );
        m_storedFilters.push_back( FilterElement( aReport->getMmsInhibitFlag(), "") );
        m_storedFilters.push_back( FilterElement( aReport->getTaggedFlag(), "") );
        m_storedFilters.push_back( FilterElement( aReport->getTaggedPTWFlag(), "") );
        m_storedFilters.push_back( FilterElement( aReport->getOverriddenFlag(), "") );
        m_storedFilters.push_back( FilterElement( aReport->getStatusChangeByFilterFlag(), aReport->getStatusChangeByFilter()) );
        m_storedFilters.push_back( FilterElement( aReport->getTimestampFilterFlag(), aReport->getTimestampFilter()) );

		delete aReport;

		return true;
	}


	void CEquipmentStatusViewerDlg::UnloadFiltersFromDB()
	{
        m_storedFilters.clear();
	}


	bool CEquipmentStatusViewerDlg::IsFiltered()
	{
		bool isFiltered = false;

		FilterFlagsAndStrings::const_iterator iter = m_currentFilters.begin();
		int counter = 0;

		while ((iter != m_currentFilters.end()) && (isFiltered == false))
		{
			if (!((m_isActiveReport) && (counter == 0)))
			{
				if ((iter->filterInUse) == true)
					isFiltered = true;
			}
			counter++;
			iter++;
		}

		if (isFiltered)
		{
			//SetStatusBarText(1, "Filters : ON");
		}
		else
		{
			//SetStatusBarText(1, "Filters : OFF");
		}

		return isFiltered;
	}



	void CEquipmentStatusViewerDlg::OnDblclkEquipmentList(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		/*if (!m_StopButton.IsWindowVisible())
			OnEditInspect();*/

		*pResult = 0;
	}


    void CEquipmentStatusViewerDlg::OnRdblclkEquipmentList(NMHDR* pNMHDR, LRESULT* pResult) 
    {
	    // Developers' tool: this will trigger the buildColumnText() method for each
        // column for the selected EquipmentStatusListItem
        //  Note: you can copy the text of messageboxes by using Copy hotkey (Ctrl-C) when
        //  the messagebox is the active window.
        /*EquipmentStatusListItemSharedPtr esl;

        POSITION pos = m_EquipmentStatusList.GetFirstSelectedItemPosition();
		if  (pos == NULL)
		{
            CString itemName;
            itemName.LoadString(IDS_EQUIPMENT);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << itemName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_020066);
			return;
		}

        unsigned long entityKey = m_EquipmentStatusList.GetItemData(m_EquipmentStatusList.GetNextSelectedItem(pos));
        esl = getReportItem( entityKey);

        CString s, s2;
        if (esl.get() != NULL)
        {
            Node_Item itemType = esl->getItemType();
            unsigned int dnKey = 0;
            std::string dnname;
            try
            {
                dnname = esl->getItemDataNodeEntityName();
                dnKey = esl->getItemDataNodeEntityKey();
            }
            catch(...)
            {
            }
            s.Format( "Equipment Status List Item\r\n\r\n%s\r\nEntity:\t%s\r\nKey:\t%i\r\nDataNode:%i\r\n", (itemType==DATANODE)?"DataNODE":(itemType==DATAPOINT)?"DataPOINT":(itemType==DATANODE_SUMMARY)?"DataNODE SUMMARY":"Unknown", dnname, entityKey, dnKey );
            for ( int col=ESL_COLUMN_MIN; col < ESL_COLUMN_MAX; col++)
            {
                TA_Base_App::EslColumns column = static_cast<TA_Base_App::EslColumns>(col);
                s2.Format( "\r\n%2i: %s", col, esl->buildNewColumnText( column));
                s+= s2;
            }
        }
        else
        {
            s.Format( "Unable to locate item with entity key %i\r\n.", entityKey);
        }

        AfxMessageBox( s, MB_OK|MB_ICONEXCLAMATION, 0);
        return;*/

	    *pResult = 0;
    }


    void CEquipmentStatusViewerDlg::ClearCurrentFilters()
	{
        m_currentFilters.clear();

		for (int col = 0; col < ESL_FILTER_OFFSET_MAX; col++)
		{
            m_currentFilters.push_back( FilterElement());
		}

		//SetStatusBarText(1, "Filters : OFF");
	}


	void CEquipmentStatusViewerDlg::ClearCurrentFiltersForActive()
	{
        //  Remove existing filters and allocate the entire vector
        ClearCurrentFilters();

        //  Set "Equipment Only" report type
        //  m_currentFilters[REPORT_TYPE_FILTER_OFFSET] = FilterElement( true, REPORT_TYPE_EQUIPMENT);

		//  Set "Equipment and DataPoint" report type
        m_currentFilters[REPORT_TYPE_FILTER_OFFSET] = FilterElement( true, REPORT_TYPE_ALL);

        //  Set Status filters
        //  First set the common filters for both the Active All & MMS reports
        m_currentFilters[STATUS_FILTER_OFFSET] = FilterElement( true, "");
        m_currentFilters[STATUS_MMS_INHIBIT_FILTER_OFFSET] = FilterElement( true, (LPCTSTR) FilterString[MMS_FILTERSTRING]);

        //  Now set the specific filters for just the Active All report
		if( ACTIVE_PROFILE_ALL_REPORT  == m_selectedReportKey )
		{
            m_currentFilters[STATUS_SCAN_INHIBIT_FILTER_OFFSET] =   FilterElement( true, (LPCTSTR) FilterString[SCAN_FILTERSTRING]);
            m_currentFilters[STATUS_ALARM_INHIBIT_FILTER_OFFSET] =  FilterElement( true, (LPCTSTR) FilterString[ALARM_FILTERSTRING]);
            m_currentFilters[STATUS_CONTROL_INHIBIT_FILTER_OFFSET]= FilterElement( true, (LPCTSTR) FilterString[CONTROL_FILTERSTRING]);
            m_currentFilters[STATUS_TAGGED_FILTER_OFFSET] =         FilterElement( true, (LPCTSTR) FilterString[TAGGED_FILTERSTRING]);
            m_currentFilters[STATUS_TAGGED_PTW_FILTER_OFFSET] =     FilterElement( true, (LPCTSTR) FilterString[TAGGED_PTW_FILTERSTRING]);
            m_currentFilters[STATUS_OVERRIDDEN_FILTER_OFFSET] =     FilterElement( true, (LPCTSTR) FilterString[OVERRIDDEN_FILTERSTRING]);
        }

        //SetStatusBarText(1, "Filters : OFF");
	}


	void CEquipmentStatusViewerDlg::OnClose() 
	{
		OnExit();
	}


	/*void CEquipmentStatusViewerDlg::OnStop() 
	{
        CWaitCursor waitCursor;

		//stop DataNodeAgent from continuing poll, now that select a new locaiton
		std::map< std::string, DataNodeAgentProxy* >::iterator itDnAgent;
		for(itDnAgent = m_DatanodeAgentProxy.begin(); itDnAgent != m_DatanodeAgentProxy.end(); ++itDnAgent)
		{
			(itDnAgent->second)->pausePoll();
		}

        // Empty the vector of entity keys - no more proxies will be created now
  		m_unprocessedEntities.clear();

        //m_progressBar.Clear();
    	//m_StopButton.ShowWindow(SW_HIDE);
		// enables all GUI components then checks current conditions and enable/disables
		// the relevant components
		AdjustGUIWhileLoading(true);
	}*/


	void CEquipmentStatusViewerDlg::AdjustGUIWhileLoading(bool aEnableAllGUIComponents)
	{
		CMenu *menu = GetMenu();
		CToolBarCtrl &tlbCtrl = m_WndToolBar.GetToolBarCtrl();
        UINT nEnable = aEnableAllGUIComponents?MF_ENABLED:MF_GRAYED;

        CMenu* sysMenu = GetSystemMenu(FALSE);
		sysMenu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | nEnable);

    	//menu->EnableMenuItem(ID_FILE_NEWREPORT, nEnable);
		//menu->EnableMenuItem(ID_FILE_EDITREPORT, nEnable);
		//menu->EnableMenuItem(ID_FILE_DELETEREPORT, nEnable);
		//menu->EnableMenuItem(ID_FILE_EXPORT, nEnable);
		menu->EnableMenuItem(ID_FILE_PRINT, nEnable);
		menu->EnableMenuItem(IDEXIT, nEnable);

		//menu->EnableMenuItem(ID_EDIT_INSPECT, nEnable);
		//menu->EnableMenuItem(ID_EDIT_CONFIGUREREPORT, nEnable);
		//menu->EnableMenuItem(ID_EDIT_CONFIGUREFILTER, nEnable);

		//menu->EnableMenuItem(ID_VIEW_FILTER, nEnable);

		//tlbCtrl.EnableButton(ID_TOOLBAR_EXPORT, aEnableAllGUIComponents);
		//tlbCtrl.EnableButton(ID_TOOLBAR_PRINT, aEnableAllGUIComponents);
		//tlbCtrl.EnableButton(ID_TOOLBAR_INSPECT, aEnableAllGUIComponents);
		//tlbCtrl.EnableButton(ID_TOOLBAR_FILTER, aEnableAllGUIComponents);
		tlbCtrl.EnableButton(ID_TOOLBAR_PRINT1, aEnableAllGUIComponents);

		m_Location.EnableWindow(aEnableAllGUIComponents);
		//m_ReportName.EnableWindow(aEnableAllGUIComponents);

        if (aEnableAllGUIComponents)
        {   // adjust gui due to different conditions present
			AdjustGUI();
        }

		m_WndToolBar.Invalidate();
		UpdateWindow();
	}


	void CEquipmentStatusViewerDlg::OnSysCommand(UINT nID, LPARAM lParam) 
	{
		if (m_StopButton.IsWindowVisible())
		{
			if ((nID & 0xFFF0) == SC_CLOSE)
				return;
		}

		if ((nID & 0xFFF0) == 16) // ABOUT
		{
			OnHelpAboutequipmentstatusviewer();
		}

		CDialog::OnSysCommand(nID, lParam);
	}


	void CEquipmentStatusViewerDlg::OnSize(UINT nType, int cx, int cy) 
	{
		TransActiveDialog::OnSize(nType, cx, cy);

		if (nType != SIZE_MAXHIDE && nType != SIZE_MINIMIZED)
		{
			Resize();
		}
	}


	void CEquipmentStatusViewerDlg::Resize()
	{
		// get the client size of our main dialog window
		// this is where we need to fit in our controls.
		CRect thisRect;
		GetClientRect(&thisRect);
		int clientHeight = thisRect.bottom - thisRect.top;
		int clientWidth = thisRect.right - thisRect.left;

		// resize the menu line
        CWnd* menuLine = GetDlgItem(IDC_MENU_LINE_CHEAT);
        if (menuLine != NULL)
            menuLine->MoveWindow(0, 0, clientWidth, 2);

		BOOL m_hasToolbar;
		BOOL m_hasStatusbar;

		if (GetMenu()->GetMenuState(ID_VIEW_TOOLBAR, MF_BYCOMMAND) == MF_CHECKED)
			m_hasToolbar = true;
		else
			m_hasToolbar = false;

		if (GetMenu()->GetMenuState(ID_VIEW_STATUSBAR, MF_BYCOMMAND) == MF_CHECKED)
			m_hasStatusbar = true;
		else
			m_hasStatusbar = false;

		int comboHeight = 0;
		int comboWidth = 0;
		int GAP = 3;

		// get the location label
		CWnd *locLabel = GetDlgItem(IDC_LOC_LABEL);
		if (locLabel)
		{
			CRect locRect;
			locLabel->GetWindowRect(&locRect);
			ScreenToClient(&locRect);
			locLabel->MoveWindow(5, 
				(m_hasToolbar ? m_ToolBarHeight : 0) + (2 * GAP),
				locRect.right - locRect.left,
				locRect.bottom - locRect.top);
		}

		// get dimensions of the location combo box
		if (m_Location.m_hWnd != NULL)
		{
			CRect comboRect;
			m_Location.GetWindowRect(&comboRect);
			ScreenToClient(&comboRect);
			comboHeight = comboRect.bottom - comboRect.top;
			comboWidth = comboRect.right - comboRect.left;
			m_Location.MoveWindow(
				75, 
				(m_hasToolbar ? m_ToolBarHeight : 0) + GAP, 
				comboWidth, 
				comboHeight);
		}

		// get the report name label
		CWnd *repLabel = GetDlgItem(IDC_REPORT_LABEL);
		if (repLabel)
		{
			CRect repRect;
			repLabel->GetWindowRect(&repRect);
			ScreenToClient(&repRect);
			repLabel->MoveWindow(150,
				(m_hasToolbar ? m_ToolBarHeight : 0) + (2 * GAP),
				repRect.right - repRect.left,
				repRect.bottom - repRect.top);
		}

		// get the report name combo
		if (m_ReportName.m_hWnd != NULL)
		{
			CRect comboRect;
			m_ReportName.GetWindowRect(&comboRect);
			ScreenToClient(&comboRect);
			comboHeight = comboRect.bottom - comboRect.top;
			comboWidth = comboRect.right - comboRect.left;

			m_ReportName.MoveWindow(
				210, 
				(m_hasToolbar ? m_ToolBarHeight : 0) + GAP, 
				comboWidth, 
				comboHeight);
		}

		// get the stop button
		if (m_StopButton.m_hWnd)
		{
			CRect stopRect;
			m_StopButton.GetWindowRect(&stopRect);
			ScreenToClient(&stopRect);
			m_StopButton.MoveWindow(
				600, 
				(m_hasToolbar ? m_ToolBarHeight : 0) + GAP, 
				stopRect.right - stopRect.left, 
				stopRect.bottom - stopRect.top);
		}

		if (m_EquipmentStatusList.m_hWnd != NULL)
		{
			CRect listRect;

			m_EquipmentStatusList.GetWindowRect(&listRect);
			ScreenToClient(&listRect);
			m_EquipmentStatusList.MoveWindow(
				2, 
				(m_hasToolbar ? m_ToolBarHeight : 0) + comboHeight + (2 * GAP), 
				clientWidth - 2,  
				clientHeight - (2 * GAP) - (comboHeight + (m_hasStatusbar ? m_StatusBarHeight : 0) + (m_hasToolbar ? m_ToolBarHeight : 0)));
		}

		if (m_WndToolBar.m_hWnd && m_hasToolbar)
		{
			m_WndToolBar.MoveWindow(0, 0, clientWidth, m_ToolBarHeight);
		}

		if (m_WndStatusBar.m_hWnd && m_hasStatusbar)
		{
			m_WndStatusBar.MoveWindow(
				0,
				clientHeight - m_StatusBarHeight,
				clientWidth,
				m_StatusBarHeight);
		}

		RedrawWindow(thisRect);
	}

	/*void CEquipmentStatusViewerDlg::UpdateStatusBar()
	{
		// get the total before filter is applied
		int Total = m_reportItems.size();

		// get the total after filter is applied
		int FilteredTotal = m_EquipmentStatusList.GetItemCount();

		CString s = "";;
		if (IsFiltered())
		{
			if (Total == 0)
				s.Format("0 items");
			else
				s.Format("%d / %d", FilteredTotal, Total);
		}
		else
		{
			if (Total == 0)
				s = "";
			else if (Total == 1)
					s.Format("%d item",Total);
			else 
					s.Format("%d items",Total);
		} 

		SetStatusBarText(2, s);
	}*/

	void CEquipmentStatusViewerDlg::UpdateStatusBar()
	{
		// get the total after filter is applied
		int FilteredTotal = m_EquipmentStatusList.GetItemCount();

		CString s = "";

		s.Format("%d items", FilteredTotal);

		SetStatusBarText(2, s);
	}

    /*void CEquipmentStatusViewerDlg::SelectReportByKey( std::string& reportKey)
    {
        int indexOfRequiredReport = -1;

        // find the index of the required report
        for (int i = 0; i < m_ReportName.GetCount(); i++)
        {
			//look for the actual string id in the map		

			int reportIndex = m_ReportName.GetItemData(i);
			std::string reportId;
			std::map<int, std::string>::iterator itr;
			itr = m_reportKeyStringToIdMap.find(m_ReportName.GetItemData(reportIndex));
			if (itr != m_reportKeyStringToIdMap.end())
			{
				reportId = itr->second;
				//get the string id
				if ( reportId == reportKey)
				{
					indexOfRequiredReport = i;
					break;
				}
			}

        }

        if (indexOfRequiredReport == -1)
            return;

        m_ReportName.SetCurSel(indexOfRequiredReport);

        OnSelchangeReportCombo();

        AdjustGUI();
    }*/


    /*EquipmentStatusListItemSharedPtr CEquipmentStatusViewerDlg::getReportItem( unsigned long entityKey) const
    {
        EquipmentStatusListItemMap::const_iterator it = m_reportItems.find( entityKey);
        if (m_reportItems.end() != it)
        {
            return it->second;
        }
        EquipmentStatusListItemSharedPtr notFound;
        return notFound;
    }*/

	void CEquipmentStatusViewerDlg::processorCallBack(boost::shared_ptr<DnUpdateRequest> newRequest)
	{
		unsigned long entitykey = newRequest->getEntityKey();
		Node_Item nodeItem = newRequest->getNodeItem();

		{
			TA_THREADGUARD( m_reportItemsLock );
			if( m_reportItems.find(entitykey) != m_reportItems.end() )
			{
				return;
			}
		}
		
		EquipmentStatusListItemSharedPtr pItem;

		if (DATAPOINT == nodeItem)
		{
			pItem.reset( new DataPointListItem( entitykey, m_scadaProxyFactory, GetSafeHwnd() ));
			TA_THREADGUARD( m_reportItemsLock );
			m_reportItems[entitykey] = pItem;
		}
		else if ( DATANODE == nodeItem)
		{
			pItem.reset( new DataNodeListItem( entitykey, m_scadaProxyFactory, GetSafeHwnd() ));
			TA_THREADGUARD( m_reportItemsLock );
			m_reportItems[entitykey] = pItem;
		}
		else if (DATANODE_SUMMARY == nodeItem)
		{
			//a. load DataNodeEntityData
			boost::shared_ptr<TA_Base_Core::DataNodeEntityData> dnEntityDataPtr;
			std::map<unsigned long, boost::shared_ptr<TA_Base_Core::DataNodeEntityData> >::iterator iterDn;
			iterDn = m_dnEntityDataPtrMap.find( entitykey );
			if(iterDn != m_dnEntityDataPtrMap.end())
			{
				dnEntityDataPtr = iterDn->second;
			}
			else
			{
				TA_Base_Core::IEntityData* pEntityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entitykey);
				TA_Base_Core::DataNodeEntityData* pDnEntityData = dynamic_cast<TA_Base_Core::DataNodeEntityData*>(pEntityData);					
				TA_ASSERT( pDnEntityData != NULL, "Load DataNode EntityData failed!");
				dnEntityDataPtr = boost::shared_ptr<TA_Base_Core::DataNodeEntityData>(pDnEntityData);
				m_dnEntityDataPtrMap[entitykey] = dnEntityDataPtr;
			}

			bool isEquipmentDN = false;

			//b. get DataNode's child datapoints and handle them	
			TA_Base_Core::SharedIEntityDataList childDps = dnEntityDataPtr->getChildDataPoints();
			TA_Base_Core::SharedIEntityDataList::iterator iterDp;
			for(iterDp = childDps.begin(); iterDp != childDps.end(); ++iterDp)
			{
				boost::shared_ptr<DataPointEntityData> dpEntityData;
				dpEntityData = boost::dynamic_pointer_cast<DataPointEntityData>(*iterDp);

				if(dpEntityData.get() == NULL) 
					continue;

				unsigned long parentKey = dpEntityData->getParent();

				if(parentKey == dnEntityDataPtr->getKey())
				{
					isEquipmentDN = true;

					TA_THREADGUARD( m_reportItemsLock );

					if( m_reportItems.find(dpEntityData->getKey()) == m_reportItems.end() )
					{
						pItem.reset( new DataPointListItem(dpEntityData, dnEntityDataPtr, m_scadaProxyFactory, GetSafeHwnd()));
						m_reportItems[ dpEntityData->getKey()] = pItem;
					}
				}
			}
			//c. add DataNodeItem
			if (isEquipmentDN)
			{
				pItem.reset( new DataNodeSummaryListItem(dnEntityDataPtr, m_scadaProxyFactory, GetSafeHwnd()) );
				TA_THREADGUARD( m_reportItemsLock );
				m_reportItems[entitykey] = pItem;
			}
		}
		else
		{
			TA_ASSERT( false, "Invalid item node type (not Datanode, Datapoint nor DataNodeSummary).");
		}
	}

	void CEquipmentStatusViewerDlg::notifyDnUpdate(std::vector<unsigned long> & dnKeys)
	{
		std::vector<unsigned long>::iterator iter;
		for (iter = dnKeys.begin(); iter != dnKeys.end(); ++iter)
		{
			boost::shared_ptr<DnUpdateRequest> request(new DnUpdateRequest(*iter, DATANODE_SUMMARY));
			m_DnUpdateProcessor->insertUnique(request);
		}
	}

	void CEquipmentStatusViewerDlg::CleanUpResource()
	{
		try
		{
			if ( !m_hasCleanUp )
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to cleanup Resource before exit");

				m_hasCleanUp = true;

				TA_Base_Core::RunParams::getInstance().deregisterRunParamUser(this);

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "deregisterRunParamUser");

				TA_Base_Bus::ScadaUtilities::removeInstance();
				
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ScadaUtilities::removeInstance()");

				std::map< std::string, DataNodeAgentProxy* >::iterator itDnAgent;

				for(itDnAgent = m_DatanodeAgentProxy.begin(); itDnAgent != m_DatanodeAgentProxy.end(); ++itDnAgent)
				{
					(itDnAgent->second)->terminate();
				}
				for(itDnAgent = m_DatanodeAgentProxy.begin(); itDnAgent != m_DatanodeAgentProxy.end(); ++itDnAgent)
				{
					delete itDnAgent->second;
					itDnAgent->second = NULL;
				}

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "clean DatanodeAgentProxy thread");

				m_DatanodeAgentProxy.clear();

				if(m_DnUpdateProcessor != NULL)
				{
					delete m_DnUpdateProcessor;
					m_DnUpdateProcessor = NULL;
				}

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "clean DnUpdateProcessor thread");

				m_reportItems.clear();

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "clean all Report items");

				TA_Base_Bus::ScadaProxyFactory::removeInstance();

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ScadaProxyFactory::removeInstance()");

				if (m_authenticationLibrary != NULL)
				{
					delete m_authenticationLibrary;
					m_authenticationLibrary = NULL;
				}

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "finish cleanup Resource before exit");
			}
		}
		catch ( ... )
		{

		}
	}
} // end of namespace
