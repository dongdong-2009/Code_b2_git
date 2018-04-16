/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/reporting/crystal_reports_viewer/src/crutilDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// crutilDlg.cpp : implementation file
//

#include "stdafx.h"
#include "crutil.h"
#include "crutilDlg.h"
#include "OpenFileDialog.h"
#include "AppSettings.hpp"
#include "HistoryReportsDlg.h"
#include "resource.h"
#include "globals.h"
#include "strings.h"
#include <afxpriv.h>
#include "ReportSelectorDlg.h"

#ifdef TRANSACTIVE_BUILD
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/about_box/src/TransActiveAboutBox.h"
#include "core/exceptions/src/ReportManagerException.h"

#include "cots/VisualCpp/Include/Winspool.h"

const int DATABASENAME_INDEX = 1;
const int USERID_INDEX = 2;
const int PASSWORD_INDEX = 3;
const int STANDARD_MIN_HEIGHT = 205;
const int STANDARD_MIN_WIDTH = 500;

#endif // TRANSACTIVE_BUILD

// TD #2372 - 4669 Report Manager - Initiating main window for the very first time
// The code below fixes the above defect.
const static long DEFAULT_LEFT_VALUE = 50;
const static long DEFAULT_TOP_VALUE = 50;
const static long DEFAULT_RIGHT_VALUE = 700;
const static long DEFAULT_BOTTOM_VALUE = 500;
// End Defect Fix.

// Timer for ProgressCtrl bar
const UINT RM_PROGRESSCTRL_TIMER = 585;
//static int OPEN_REPORT_FILE_RATE = 0;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef TRANSACTIVE_BUILD
using TA_Base_Bus::TransActiveDialog;
using namespace TA_Base_Core;

namespace TA_Base_App
{
    namespace TA_Reporting
    {
#endif // TRANSACTIVE_BUILD

		/////////////////////////////////////////////////////////////////////////////
		// CAboutDlg dialog used for App About

		class CAboutDlg : public CDialog
		{
		public:
			CAboutDlg();

		// Dialog Data
			//{{AFX_DATA(CAboutDlg)
			enum { IDD = IDD_ABOUTBOX };
			//}}AFX_DATA

			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CAboutDlg)
			protected:
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
			//}}AFX_VIRTUAL

		// Implementation
		protected:
			//{{AFX_MSG(CAboutDlg)
			virtual BOOL OnInitDialog();
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()
		};

		CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
		{
			//{{AFX_DATA_INIT(CAboutDlg)
			//}}AFX_DATA_INIT
		}

		BOOL CAboutDlg::OnInitDialog() 
		{
			CDialog::OnInitDialog();

			CString cstrAppVersion;
			cstrAppVersion = APPLICATION_NAME;
			cstrAppVersion += " V";
			cstrAppVersion += APPLICATION_VERSION;
			this->SetDlgItemText( IDC_STATIC_VERSION, cstrAppVersion );
			return TRUE;  // return TRUE unless you set the focus to a control
						  // EXCEPTION: OCX Property Pages should return FALSE
		}

		void CAboutDlg::DoDataExchange(CDataExchange* pDX)
		{
			CDialog::DoDataExchange(pDX);
			//{{AFX_DATA_MAP(CAboutDlg)
			//}}AFX_DATA_MAP
		}

		BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
			//{{AFX_MSG_MAP(CAboutDlg)
			//}}AFX_MSG_MAP
		END_MESSAGE_MAP()


		/////////////////////////////////////////////////////////////////////////////
		// CCrutilDlg dialog

		#ifdef TRANSACTIVE_BUILD
		CCrutilDlg::CCrutilDlg(TA_Base_Bus::IGUIAccess* genericGUICallback, CWnd* pParent /*=NULL*/)
		: TransActiveDialog(*genericGUICallback, CCrutilDlg::IDD, pParent), m_bShowMaxWindow( false ),
		m_bShowToolBar( true ), m_bShowStatusBar( true ), m_bFileClosed( false )
		#else
		CCrutilDlg::CCrutilDlg(CWnd* pParent /*=NULL*/)
		: CDialog(CCrutilDlg::IDD, pParent), m_bShowMaxWindow( false ), 
		m_bShowToolBar( true ), m_bShowStatusBar( true ), m_bFileClosed( false )
		#endif // TRANSACTIVE_BUILD
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin CCrutilDlg::CCrutilDlg...");

			//{{AFX_DATA_INIT(CCrutilDlg)
				// NOTE: the ClassWizard will add member initialization here
			//}}AFX_DATA_INIT
			// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
			m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

			#ifdef TRANSACTIVE_BUILD

            // Create an instance of CrystalReportHelper and setup
            // its default operating parameters.
			m_pCrystalReports = new CrystalReportsHelper();

            // Set report viewing to OnDemand.
			m_pCrystalReports->setViewingType( CrystalReportsHelper::crhOnDemand );

			m_pCrystalReportsBlank = new CrystalReportsHelper();
			m_pCrystalReportsBlank->setViewingType( CrystalReportsHelper::crhOnDemand );


            // Defect #2843 - Report Manager not launched within the schematic area
            // Set the resizing properties
            TA_Base_Bus::ResizingProperties properties;
            properties.canMaximise = true;
            properties.maxHeight = -1;
            properties.maxWidth = -1;
            properties.minHeight = STANDARD_MIN_HEIGHT;
            properties.minWidth = STANDARD_MIN_WIDTH;
            setResizingProperties(properties);

			#else

			m_pCrystalReports = NULL;
			m_pCrystalReportsBlank = NULL;

			#endif // TRANSACTIVE_BUILD
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "End CCrutilDlg::CCrutilDlg...");
		}

		CCrutilDlg::~CCrutilDlg()
		{
			if (m_pCrystalReports)
            {
                m_pCrystalReports->setViewerInstance( NULL );
				m_pCrystalReports->setBrowserInstance( NULL );
    			#ifdef TRANSACTIVE_BUILD
                delete m_pCrystalReports;
				m_pCrystalReports = NULL;
                #endif // TRANSACTIVE_BUILD
            }
			if (m_pCrystalReportsBlank)
			{
				m_pCrystalReportsBlank->setViewerInstance( NULL );
				m_pCrystalReportsBlank->setBrowserInstance( NULL );
				#ifdef TRANSACTIVE_BUILD
				delete m_pCrystalReportsBlank;
				m_pCrystalReportsBlank = NULL;
				#endif // TRANSACTIVE_BUILD
			}
		}

		void CCrutilDlg::DoDataExchange(CDataExchange* pDX)
		{
			CDialog::DoDataExchange(pDX);
			//{{AFX_DATA_MAP(CCrutilDlg)
			DDX_Control(pDX, IDC_CRVIEWER1, m_ReportViewer);
			DDX_Control(pDX, IDC_EXPLORER1, m_HistoryReportViewer);

			DDX_Control(pDX, IDC_CRVIEWER_BLANK, m_ReportViewerBlank);
			//}}AFX_DATA_MAP
		}
        
        #ifdef TRANSACTIVE_BUILD
        BEGIN_MESSAGE_MAP(CCrutilDlg, TransActiveDialog)
        #else
		BEGIN_MESSAGE_MAP(CCrutilDlg, CDialog)
        #endif
			//{{AFX_MSG_MAP(CCrutilDlg)
			ON_WM_SYSCOMMAND()
			ON_WM_PAINT()
			ON_WM_QUERYDRAGICON()
			ON_WM_SIZE()
			ON_COMMAND(IDM_FILE_EXIT, OnFileExit)
			ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
			ON_COMMAND(IDM_FILE_OPEN, OnFileOpen)
			ON_COMMAND(IDM_FILE_CLOSE, OnFileClose)
			ON_COMMAND(ID_HELP_REPORTMANAGERHELP, OnReportManagerHelp)
			ON_WM_DESTROY()
			ON_COMMAND(ID_VIEW_HISTORY, OnViewHistory)
			ON_COMMAND(ID_VIEW_REFRESH, OnViewRefresh)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_UPDATE_COMMAND_UI(ID_VIEW_RM_TOOLBAR, OnUpdateShowToolBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RM_STATUSBAR, OnUpdateShowStatusBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_HISTORY, OnUpdateViewHistory)
	//}}AFX_MSG_MAP
			ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
		END_MESSAGE_MAP()

        #ifdef TRANSACTIVE_BUILD
        BEGIN_EVENTSINK_MAP(CCrutilDlg, TransActiveDialog)
        #else
		BEGIN_EVENTSINK_MAP(CCrutilDlg, CDialog)
        #endif
            //{{AFX_EVENTSINK_MAP(CCrutilDlg)
	        ON_EVENT(CCrutilDlg, IDC_CRVIEWER1, 409 /* PrintButtonClicked */, OnPrintButtonClicked, VTS_PBOOL)
			ON_EVENT(CCrutilDlg, IDC_CRVIEWER1, 408 /* RefreshButtonClicked */, OnRefreshButtonClicked, VTS_PBOOL)////cl-18123--lkm
	        //}}AFX_EVENTSINK_MAP
        END_EVENTSINK_MAP()

        /////////////////////////////////////////////////////////////////////////////
		// CCrutilDlg message handlers

		void CCrutilDlg::init()
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Start CCrutilDlg::init...");

            // Create an instance of the Command Line Parser object.
            CommandLineParser objArgv;
			
            // If the report name parameter exists, then set the report name.
            if (objArgv.parameterExists( REPORTNAME_LABEL ))
			{
                std::vector<std::string> value = objArgv.getParameterValue( REPORTNAME_LABEL );
				m_cstrReportName = value[0].c_str();
				m_pCrystalReports->setReportName( m_cstrReportName.GetBuffer( m_cstrReportName.GetLength() ) );

				m_pCrystalReportsBlank->setReportName( m_cstrReportName.GetBuffer( m_cstrReportName.GetLength() ) );
            }
			//TD18040 
			//xxs add
			//get the historical report path
			if ( objArgv.parameterExists( HISTORY_REPORT_PATH_LABEL ) )
			{
                std::vector<std::string> value = objArgv.getParameterValue( HISTORY_REPORT_PATH_LABEL );
                m_pCrystalReports->setHistoryReportPathFromCmdLine( value[0].c_str());

				m_pCrystalReportsBlank->setHistoryReportPathFromCmdLine( value[0].c_str());
			}
			//end xxs add
			
            // Enable parameter prompting if the parameter has been supplied.
			if (objArgv.parameterExists( NOPARAMPROMPTING_LABEL )) m_pCrystalReports->enableParameterPrompting( false );
			if (objArgv.parameterExists( NOPARAMPROMPTING_LABEL )) m_pCrystalReportsBlank->enableParameterPrompting( false );

            // Configure the Database connection parameters if the parameter 
            // has been supplied.
            if (objArgv.parameterExists( RPARAM_DBCONNECTION )) 
            {
                std::vector<std::string> value = objArgv.getParameterValue( RPARAM_DBCONNECTION );
                std::string connectionString = value[0];
                std::vector<std::string> dbConnData;
                SimpleDb::ParseConnectStr(connectionString, dbConnData);
                if (dbConnData.size() >=3)
                {
					m_pCrystalReports->setServerName( const_cast<char*>(dbConnData[DATABASENAME_INDEX].c_str()) );
					m_pCrystalReports->setDatabaseName( const_cast<char*>(dbConnData[DATABASENAME_INDEX].c_str()) );
					m_pCrystalReports->setUserID( const_cast<char*>(dbConnData[USERID_INDEX].c_str()) );
					m_pCrystalReports->setPassword( const_cast<char*>(dbConnData[PASSWORD_INDEX].c_str()) );

					m_pCrystalReportsBlank->setServerName( const_cast<char*>(dbConnData[DATABASENAME_INDEX].c_str()) );
					m_pCrystalReportsBlank->setDatabaseName( const_cast<char*>(dbConnData[DATABASENAME_INDEX].c_str()) );
					m_pCrystalReportsBlank->setUserID( const_cast<char*>(dbConnData[USERID_INDEX].c_str()) );
					m_pCrystalReportsBlank->setPassword( const_cast<char*>(dbConnData[PASSWORD_INDEX].c_str()) );
                }
            }

			// Setup Report Viewing.
			if (m_pCrystalReports)
			{
				m_pCrystalReports->setViewerInstance( &m_ReportViewer );
				m_pCrystalReports->setBrowserInstance( &m_HistoryReportViewer );
				
#ifdef TRANSACTIVE_BUILD
                
                // Retrieve current Session ID and Operator Name and pass it
                // to the Crystal Reports Helper Class.
                std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).c_str();
				std::string operatorName = TA_Base_Core::RunParams::getInstance().get("OperatorName").c_str();
                if (sessionId != "")
                {
                  /*
                    TA_Base_Core::IOperator* op = TA_Base_Core::OperatorAccessFactory::
                  						getInstance().getOperatorFromSession(sessionId, false);
                                      operatorName = op->getName();
                                      delete op;*/
                  
					
                    m_pCrystalReports->setSessionID( (char*)(sessionId.c_str()) );
                    m_pCrystalReports->setOperatorName( (char*)(operatorName.c_str()) );
                }
				
#endif // TRANSACTIVE_BUILD
				
                // Create an instance of the Command Line Parser object and retrieve the report parameters
                CommandLineParser objArgv;
                std::vector<std::string> reportParameters;
                if(objArgv.parameterExists( REPORTPARAMETER_LABEL ))
                {
                    reportParameters = objArgv.getParameterValue( REPORTPARAMETER_LABEL );
                }
				
                // And view the report.
                try
                {
					if (objArgv.parameterExists( REPORTNAME_LABEL ))
					{
						SetTimer( RM_PROGRESSCTRL_TIMER, 100, NULL );
					}

                    if (m_pCrystalReports->viewReport(reportParameters) == false)
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "CCrutilDlg::init, view report failed");

						GetMenu()->GetSubMenu(0)->EnableMenuItem(IDM_FILE_CLOSE, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
						// Error.
					}

					if (objArgv.parameterExists( REPORTNAME_LABEL ))
					{
						OPEN_REPORT_FILE_RATE = OPEN_REPORT_RATE_COMPLETED;
						GetMenu()->GetSubMenu(0)->EnableMenuItem(IDM_FILE_CLOSE, MF_BYCOMMAND | MF_ENABLED);

						if ( m_bShowStatusBar == true )
						{
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CCrutilDlg::init, make sure status bar show correct when use --file parameters");

							//m_ProgressCtrl.ShowWindow(SW_HIDE);
							//m_ProgressCtrl.ShowWindow(SW_SHOW);
							this->ShowWindow(SW_SHOWMINIMIZED);
							this->ShowWindow(SW_RESTORE);
						}

						UpdateWindow();
					}
                }
                catch(const TA_Base_Core::ReportManagerException& ex)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "ReportManagerException", ex.what());
                    TA_Base_Bus::TransActiveMessage userMsg;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_140036);
                }
				// Initialise show tool bar and status bar or not when open report file
				//m_pCrystalReports->setShowToolBar(m_bShowToolBar);
				m_pCrystalReports->setShowToolBar( false );
				// Initialise "Toolbar" item to check/uncheck on "View" menu.
				if ( m_bShowToolBar ==true )
				{
					m_pCrystalReports->setShowToolBar( true );

					GetMenu()->GetSubMenu(1)->CheckMenuItem(ID_VIEW_RM_TOOLBAR, MF_BYCOMMAND | MF_CHECKED);
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CCrutilDlg::init, 2...");
				}
				else
				{
					GetMenu()->GetSubMenu(1)->CheckMenuItem(ID_VIEW_RM_TOOLBAR, MF_BYCOMMAND | MF_UNCHECKED);
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CCrutilDlg::init, 2...");
				}

				if (IsWindow(m_ProgressCtrl.GetSafeHwnd()))
				{
					if ( m_bShowStatusBar == true )
					{
						m_ProgressCtrl.ShowWindow(SW_SHOW);
						GetMenu()->GetSubMenu(1)->CheckMenuItem(ID_VIEW_RM_STATUSBAR, MF_BYCOMMAND|MF_CHECKED);
						//m_bShowStatusBar = false;
					} 
					else
					{
						m_ProgressCtrl.ShowWindow(SW_HIDE);
						GetMenu()->GetSubMenu(1)->CheckMenuItem(ID_VIEW_RM_STATUSBAR, MF_BYCOMMAND|MF_UNCHECKED);
						//m_bShowStatusBar = true;
					}
				}
			}

			if (m_pCrystalReportsBlank)
			{
				m_pCrystalReportsBlank->setViewerInstance( &m_ReportViewerBlank );
				//m_pCrystalReportsBlank->setBrowserInstance( &m_HistoryReportViewer );

				#ifdef TRANSACTIVE_BUILD

				// Retrieve current Session ID and Operator Name and pass it
				// to the Crystal Reports Helper Class.
				std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).c_str();
				std::string operatorName = TA_Base_Core::RunParams::getInstance().get("OperatorName").c_str();
				if (sessionId != "")
				{
					/*
					TA_Base_Core::IOperator* op = TA_Base_Core::OperatorAccessFactory::
											getInstance().getOperatorFromSession(sessionId, false);
										operatorName = op->getName();
										delete op;*/
					

					m_pCrystalReportsBlank->setSessionID( (char*)(sessionId.c_str()) );
					m_pCrystalReportsBlank->setOperatorName( (char*)(operatorName.c_str()) );
				}

				#endif // TRANSACTIVE_BUILD

				// Create an instance of the Command Line Parser object and retrieve the report parameters
				CommandLineParser objArgv;
				std::vector<std::string> reportParameters;
				if(objArgv.parameterExists( REPORTPARAMETER_LABEL ))
				{
					reportParameters = objArgv.getParameterValue( REPORTPARAMETER_LABEL );
				}

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CCrutilDlg::init, 1...");
				m_pCrystalReportsBlank->setDisplayBackgroundEdge( TRUE );
				m_pCrystalReportsBlank->setDisplayBorder( TRUE );

				// Initialise show tool bar or not when open report file
				m_pCrystalReportsBlank->setShowToolBar(m_bShowToolBar);

				{
					CRect rectTempViewer1;
					CRect rectTempViewerBlank;

					m_ReportViewer.GetWindowRect(rectTempViewer1);
					m_ReportViewerBlank.GetWindowRect(rectTempViewerBlank);

					/////////////////////////////////////////////////////////////////////
					CRect rectTempHistoryViewer;
					m_HistoryReportViewer.GetWindowRect(rectTempHistoryViewer);
					////////////////////////////////////////////////////////////////////

					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
						"CCrutilDlg::init, Viewer1 width: %d, height: %d. ViewerBlank width: %d, height: %d. HistoryViewer width: %d, height:%d.",
						rectTempViewer1.Width(), rectTempViewer1.Height(), rectTempViewerBlank.Width(), rectTempViewerBlank.Height(), rectTempHistoryViewer.Width(), rectTempHistoryViewer.Height() );

					if ( (rectTempViewer1.Width() != rectTempViewerBlank.Width()) 
						|| (rectTempViewer1.Height() != rectTempViewerBlank.Height()) )
					{
						//m_HistoryReportViewer.ShowWindow(SW_HIDE);
						repaintCurrentDialog(SIZE_RESTORED, rectTempViewer1.Width(), rectTempViewer1.Height() );

						//m_HistoryReportViewer.ShowWindow(SW_HIDE);
						//m_HistoryReportViewer.MoveWindow(0, 0, 0, 0);

						UpdateWindow();
					}
				}

				m_HistoryReportViewer.MoveWindow(0, 0, 0, 0);
			}			
			// Restore the previous Windows size.
			restoreWindowSize();
			
			m_bViewHistory = false;			//not allow to view history reports when no report open
			if ( !objArgv.parameterExists( REPORTNAME_LABEL ) )
			{
				GetMenu()->GetSubMenu(0)->EnableMenuItem(IDM_FILE_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			}
			
#ifndef TRANSACTIVE_BUILD
			
			// And display the window.
			if (m_bShowMaxWindow) ::ShowWindow( this->m_hWnd, SW_SHOWMAXIMIZED );
			
#endif
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "End CCrutilDlg::init...");

		}

		BOOL CCrutilDlg::OnInitDialog()
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Start CCrutilDlg::OnInitDialog...");
            #ifdef TRANSACTIVE_BUILD
            
			CommandLineParser objArgv;
            // Defect #2843 - Report Manager not launched within the schematic area
            TransActiveDialog::OnInitDialog();

			// Set to fullscreen if the parameter has been supplied.
			if (objArgv.parameterExists( FULLSCREEN_LABEL )) m_bShowMaxWindow = true;

		if (objArgv.parameterExists( REPORTNAME_LABEL ) )
		{
				SetTimer( RM_PROGRESSCTRL_TIMER, 100, NULL );
		}
			//TD18065
			//xxs add
			if ( m_bShowMaxWindow )
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "In CCrutilDlg::OnInitDialog, maximize ReportManager when start");
				ShowWindow(SW_SHOWMAXIMIZED);   
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "In CCrutilDlg::OnInitDialog, Normal show and will not maximize when start using CommandLine");
				ShowWindow(SW_SHOW);
			}
			//end xxs add

			
            #else
            
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Not In TRANSACTIVE_BUILD");
            // Add "About..." menu item to system menu.

			// IDM_ABOUTBOX must be in the system command range.
			ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
			ASSERT(IDM_ABOUTBOX < 0xF000);

			CMenu* pSysMenu = GetSystemMenu(FALSE);
			if (pSysMenu != NULL)
			{
				CString strAboutMenu;
				strAboutMenu.LoadString(IDS_ABOUTBOX);
				if (!strAboutMenu.IsEmpty())
				{
					pSysMenu->AppendMenu(MF_SEPARATOR);
					pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
				}
			}
            
            #endif // TRANSACTIVE_BUILD

			
			// Set the icon for this dialog.  The framework does this automatically
			//  when the application's main window is not a dialog
			SetIcon(m_hIcon, TRUE);			// Set big icon
			SetIcon(m_hIcon, FALSE);		// Set small icon

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CCrutilDlg::OnInitDialog, 1...");

			static UINT indicators[] =
			{
				ID_SEPARATOR, // status line indicator
				//ID_INDICATOR_CAPS, //CAP lock indicator.
				//ID_INDICATOR_NUM, //NUM lock indicator.
				//ID_INDICATOR_SCRL, //SCRL lock indicator.
			};

			if (!m_ProgressCtrl.Create(this, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM) ||
				!m_ProgressCtrl.SetIndicators(indicators,
				sizeof(indicators)/sizeof(UINT)))
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
					"CCrutilDlg::OnInitDialog, Failed to create Statusbar.");

				return -1; // fail to create
			}

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CCrutilDlg::OnInitDialog, 2...");

			m_ProgressCtrl.ShowProgressBar( TRUE );

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CCrutilDlg::OnInitDialog, 3...");

			RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);

			m_ProgressCtrl.SetRange(0, 1000);
			//m_ProgressCtrl.SetStep(5);
			m_ProgressCtrl.SetPos(0);

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "End CCrutilDlg::OnInitDialog...");
			return TRUE;  // return TRUE  unless you set the focus to a control
		}

		void CCrutilDlg::OnSysCommand(UINT nID, LPARAM lParam)
		{
			if ((nID & 0xFFF0) == IDM_ABOUTBOX)
			{
				displayAboutBox();
			}
			else
			{
				CDialog::OnSysCommand(nID, lParam);
			}
		}

		// If you add a minimize button to your dialog, you will need the code below
		//  to draw the icon.  For MFC applications using the document/view model,
		//  this is automatically done for you by the framework.

		void CCrutilDlg::OnPaint() 
		{
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
		}

		// The system calls this to obtain the cursor to display while the user drags
		//  the minimized window.
		HCURSOR CCrutilDlg::OnQueryDragIcon()
		{
			return (HCURSOR) m_hIcon;
		}

		void CCrutilDlg::OnSize(UINT nType, int cx, int cy) 
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Start CCrutilDlg::OnSize...");

			CDialog::OnSize(nType, cx, cy);

			repaintCurrentDialog( nType, cx, cy );

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "End CCrutilDlg::OnSize...");
		}

		void CCrutilDlg::repaintCurrentDialog( UINT nType, int cx, int cy )
		{
			if (m_pCrystalReports)
			{
				//m_pCrystalReports->setWindowWidth( cx );
				//m_pCrystalReports->setWindowHeight( cy );

				if (m_pCrystalReports->getViewingType() == CrystalReportsHelper::crhOnDemand)
				{
					if (m_ReportViewer.m_hWnd)
					{
						if ( m_bShowStatusBar )
						{
							m_pCrystalReports->setWindowWidth( cx );
							m_pCrystalReports->setWindowHeight( cy - 20 );

							//m_ReportViewer.MoveWindow(0, 0, cx, cy - 20);
							if (IsWindow(m_ReportViewer.GetSafeHwnd()))
							{
								RECT rt;
								GetClientRect(&rt);
								rt.bottom = rt.bottom - 20;
								m_ReportViewer.MoveWindow(&rt);

								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
									"Report Viewer size: left = %lu, right = %lu, top = %lu, bottom = %lu",
									rt.left, rt.right, rt.top, rt.bottom);
							}

							ensureStatusBarPositionCorrect();
						}
						else
						{
							m_pCrystalReports->setWindowWidth( cx );
							m_pCrystalReports->setWindowHeight( cy );

							m_ReportViewer.MoveWindow(0, 0, cx, cy);	
						}
					}
				}
				else
				{
					if (m_HistoryReportViewer.m_hWnd) 
					{
						if ( m_bShowStatusBar )
						{
							//m_HistoryReportViewer.MoveWindow(0, 0, cx, cy - 20);
							if (IsWindow(m_HistoryReportViewer.GetSafeHwnd()))
							{
								RECT rt;
								GetClientRect(&rt);
								rt.bottom = rt.bottom - 20;
								m_HistoryReportViewer.MoveWindow(&rt);
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
									"History Report Viewer size: left = %lu, right = %lu, top = %lu, bottom = %lu",
									rt.left, rt.right, rt.top, rt.bottom);
							}


							ensureStatusBarPositionCorrect();
						} 
						else
						{
							m_HistoryReportViewer.MoveWindow(0, 0, cx, cy);
						}
					}	
				}
			}

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CCrutilDlg::repaintCurrentDialog, 1...");

			if (m_pCrystalReportsBlank)
			{
				//m_pCrystalReportsBlank->setWindowWidth( cx );
				//m_pCrystalReportsBlank->setWindowHeight( cy );

				if (m_pCrystalReportsBlank->getViewingType() == CrystalReportsHelper::crhOnDemand)
				{
					if (m_ReportViewerBlank.m_hWnd)
					{
						if ( m_bShowStatusBar )
						{
							m_pCrystalReportsBlank->setWindowWidth( cx );
							m_pCrystalReportsBlank->setWindowHeight( cy - 20 );

							//m_ReportViewerBlank.MoveWindow(0, 0, cx, cy - 20);
							if (IsWindow(m_ReportViewerBlank.GetSafeHwnd()))
							{
								RECT rt;
								GetClientRect(&rt);
								rt.bottom = rt.bottom - 20;
								m_ReportViewerBlank.MoveWindow(&rt);
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
									"Report Viewer Blank size: left = %lu, right = %lu, top = %lu, bottom = %lu",
									rt.left, rt.right, rt.top, rt.bottom);
							}

							ensureStatusBarPositionCorrect();
						}
						else
						{
							m_pCrystalReportsBlank->setWindowWidth( cx );
							m_pCrystalReportsBlank->setWindowHeight( cy );

							m_ReportViewerBlank.MoveWindow(0, 0, cx, cy);
						}
					}
					else
					{
						//
					}
				}
			}

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CCrutilDlg::repaintCurrentDialog, 2...");
			CRect rcClient(0, 0, cx, cy);
			RepositionBars(0, 0xffff, AFX_IDW_PANE_FIRST, 0, 0, &rcClient);
			RepositionBars(0, 0xffff, AFX_IDW_PANE_FIRST, reposQuery, &rcClient, &rcClient);
		}

		void CCrutilDlg::ensureStatusBarPositionCorrect()
		{
			if (IsWindow(m_ProgressCtrl.GetSafeHwnd()))
			{
				RECT rt;
				GetClientRect(&rt);
				rt.top = rt.bottom - 20;
				m_ProgressCtrl.MoveWindow(&rt);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					"CCrutilDlg::ensureStatusBarPositionCorrect, Status Bar Position size: left = %lu, right = %lu, top = %lu, bottom = %lu",
					rt.left, rt.right, rt.top, rt.bottom);
			}
			m_ProgressCtrl.AdjustProgressBarPosition();
		}

        void CCrutilDlg::OnPrintButtonClicked(BOOL FAR* UseDefault) 
        {
            // Crystal Reports only prints to the default printer. There isn't a way to set the desired printer. 
            // The only way is to set the default printer. So here we retrieve the current default printer, then
            // launch the standard print dialog so the user can select a printer. This printer is set as the default
            // printer and once printing is complete, the orginal default printer will be reset as the default.

            DWORD lpPrinterStringSize = 200;
            CString lpOriginalPrinter;
            ::GetDefaultPrinter(lpOriginalPrinter.GetBuffer(lpPrinterStringSize), &lpPrinterStringSize);

	        CPrintDialog dlg(false,  PD_ALLPAGES | PD_USEDEVMODECOPIES | PD_NOPAGENUMS | PD_HIDEPRINTTOFILE | PD_NOSELECTION, this);
            if(dlg.DoModal() == IDOK)
            {
                LPDEVNAMES lpDevNames;
                LPTSTR lpszDeviceName;

                lpDevNames = (LPDEVNAMES) GlobalLock(dlg.m_pd.hDevNames);
                lpszDeviceName = (LPTSTR )lpDevNames + lpDevNames->wDeviceOffset;
                
                // Set the default printer
                ::SetDefaultPrinter(lpszDeviceName);
                
                // No parameters to pass in - pass in empty vector
                std::vector<std::string> parameterValuePairs;
                m_pCrystalReports->printReport(parameterValuePairs);

                // Reset original default printer
                ::SetDefaultPrinter(lpOriginalPrinter);
            }
                
            // Setting this parameter to false will prevent the default Crystal Reports print dialog from
            // being launched
            *UseDefault = false;
	    }

        void CCrutilDlg::OnFileExit() 
		{	
			DestroyWindow();
		}

		void CCrutilDlg::OnHelpAbout() 
		{
			displayAboutBox();
        }

		void CCrutilDlg::OnViewHistory() 
		{
			if (m_pCrystalReports)
			{
				HistoryReportsDlg selectHistory;
				selectHistory.m_cstrDirectory = m_pCrystalReports->getHistoryReportPath( true );
				selectHistory.m_cstrFileSpec = "*";
				selectHistory.m_cstrFileSpec += m_pCrystalReports->getReportName();
				selectHistory.m_cstrFileSpec += ".*";

				//TD18040 
				//xxs add
				CString fileName_log = "dir=";
				fileName_log += selectHistory.m_cstrDirectory;
				fileName_log += " fileName=";
				fileName_log += selectHistory.m_cstrFileSpec;
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, fileName_log );
				//end xxs add

                if (selectHistory.filesExist())
				{
					int nResponse = selectHistory.DoModal();
					if (nResponse == IDOK)
					{
						CString cstrTemp;
						//cstrTemp = m_pCrystalReports->getHistoryReportPath(  );
						cstrTemp = selectHistory.m_cstrDirectory;
						cstrTemp += selectHistory.m_cstrSelection;
						m_pCrystalReports->setReportName( cstrTemp.GetBuffer( cstrTemp.GetLength() ) );
						m_pCrystalReports->setViewingType( CrystalReportsHelper::crhHistorical );
						
                        // No parameters to pass in - pass in empty vector
                        std::vector<std::string> parameterValuePairs;
                        m_pCrystalReports->viewReport(parameterValuePairs);
					}
					else if (nResponse == IDCANCEL)
					{
					}
				}
				else
				{
                    TA_Base_Bus::TransActiveMessage userMsg;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UI_140015);
                }
			}
		}
		//cl-18123--lkm
		void CCrutilDlg::OnRefreshButtonClicked(BOOL FAR* UseDefault)
		{
		// Setting this parameter to false will prevent the default Crystal Reports print dialog from
        // being launched
		* UseDefault = false;
		OnViewRefresh();

		}
		//cl-18123--lkm
		void CCrutilDlg::OnViewRefresh() 
		{
			if (m_pCrystalReports) m_pCrystalReports->refreshReport();
		}

		void CCrutilDlg::OnFileOpen() 
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin CCrutilDlg::OnFileOpen");

			{
				CRect rectTempViewer1;
				CRect rectTempViewerBlank;

				m_ReportViewer.GetWindowRect(rectTempViewer1);
				m_ReportViewerBlank.GetWindowRect(rectTempViewerBlank);
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					"CCrutilDlg::OnFileOpen, Viewer1 width: %d, height: %d. ViewerBlank width: %d, height: %d",
					rectTempViewer1.Width(), rectTempViewer1.Height(), rectTempViewerBlank.Width(), rectTempViewerBlank.Height() );
			}

			if (m_pCrystalReports)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CCrutilDlg::OnFileOpen, m_pCrystalReports != NULL");

				OpenFileDialog objOpenFile( TRUE, NULL, "", 0, "Report Templates (*.rpt)|*.rpt||", 0);
				objOpenFile.m_ofn.Flags = OFN_HIDEREADONLY | OFN_ENABLEHOOK | OFN_EXPLORER;
				// TD18033
				//HINSTANCE hInst = ::ShellExecute(this->m_hWnd, "explore", "\\\\adm-occ-1\\C$\\transactive\\reports\\R-Report", NULL, NULL, SW_HIDE);
				HINSTANCE hInst = ::ShellExecute(this->m_hWnd, "explore", "C:\\transactive\\reports\\R-Report", NULL, NULL, SW_HIDE);

				if(reinterpret_cast<int>(hInst) <= 32)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CCrutilDlg::OnFileOpen, open local C disk folder");

					objOpenFile.m_ofn.lpstrInitialDir="C:\\transactive\\reports\\R-Report";

					// popup message dialog, "Unable to connect to ADMIN MFT."
					TA_Base_Bus::TransActiveMessage userMsg;
					UINT selectedButton = userMsg.showMsgBox(IDS_UW_140002);
				}
				else
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CCrutilDlg::OnFileOpen, open adm folder");

					//objOpenFile.m_ofn.lpstrInitialDir="\\\\adm-occ-1\\C$\\transactive\\reports\\R-Report";
					objOpenFile.m_ofn.lpstrInitialDir="C:\\transactive\\reports\\R-Report";
				}
				// TD18033
				ReportSelectorDlg reportSelector;
				bool isATS;
				if(reportSelector.DoModal() ==IDOK)
					isATS = true;
				else
					isATS = false;


				if (objOpenFile.DoModal() == IDOK)
				{
					SetTimer( RM_PROGRESSCTRL_TIMER, 100, NULL );
					OPEN_REPORT_FILE_RATE = OPEN_REPORT_RATE_INIT_TO_ZERO;

					if ( m_bShowStatusBar == true )
					{
						// Hide then show statusbar because progress control cannot show correctly without this way.
						m_ProgressCtrl.ShowWindow(SW_HIDE);
						m_ProgressCtrl.ShowWindow(SW_SHOW);
					} 

					m_pCrystalReports->setReportName( objOpenFile.GetPathName().GetBuffer(100) );
					m_pCrystalReports->setViewingType( CrystalReportsHelper::crhOnDemand );
					m_pCrystalReports->setDialogInstance( this );

                    // No parameters to pass in - pass in empty vector
                    std::vector<std::string> parameterValuePairs;

					if (m_pCrystalReports->viewReport(parameterValuePairs,isATS) == false)
					{
						GetMenu()->GetSubMenu(0)->EnableMenuItem(IDM_FILE_CLOSE, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);

                        TA_Base_Bus::TransActiveMessage userMsg;
                        UINT selectedButton = userMsg.showMsgBox(IDS_UE_140033);
                    }
					else
					{
						GetMenu()->GetSubMenu(0)->EnableMenuItem(IDM_FILE_CLOSE, MF_BYCOMMAND | MF_ENABLED);

						m_bViewHistory = true;		//allow to view history
  					}

					m_bFileClosed = false;

					if ( !m_bFileClosed )
					{
						//GetDlgItem(IDC_CRVIEWER1)->ShowWindow(SW_SHOW);
						m_ReportViewer.ShowWindow(SW_SHOW);
						UpdateWindow();
					} 
					else
					{
						//GetDlgItem(IDC_CRVIEWER1)->ShowWindow(SW_HIDE);
						m_ReportViewer.ShowWindow(SW_HIDE);
						UpdateWindow();
					}

					Sleep(300);
					OPEN_REPORT_FILE_RATE = OPEN_REPORT_RATE_COMPLETED;
					//KillTimer( RM_PROGRESSCTRL_TIMER );

				}
				else
				{
					if ( m_bShowStatusBar == true )
					{
						m_ProgressCtrl.ShowWindow(SW_HIDE);
						m_ProgressCtrl.ShowWindow(SW_SHOW);
					}
				}
			}

			{
					CRect rectTempViewer1;
					CRect rectTempViewerBlank;

					m_ReportViewer.GetWindowRect(rectTempViewer1);
					m_ReportViewerBlank.GetWindowRect(rectTempViewerBlank);
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
						"CCrutilDlg::OnFileOpen, Viewer1 width: %d, height: %d. ViewerBlank width: %d, height: %d",
						rectTempViewer1.Width(), rectTempViewer1.Height(), rectTempViewerBlank.Width(), rectTempViewerBlank.Height() );
			}
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "End CCrutilDlg::OnFileOpen");

		}

		void CCrutilDlg::OnFileClose()
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin CCrutilDlg::OnFileClose");
			if ( m_pCrystalReports->closeReport() == true )
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin CCrutilDlg::OnFileClose, closeReport = true");

				//m_ProgressCtrl.AdjustProgressBarPosition();

				OPEN_REPORT_FILE_RATE = OPEN_REPORT_RATE_COMPLETED;
				KillTimer( RM_PROGRESSCTRL_TIMER );
				OPEN_REPORT_FILE_RATE = OPEN_REPORT_RATE_INIT_TO_ZERO;

				m_bFileClosed = true;

				if ( !m_bFileClosed )
				{
					//GetDlgItem(IDC_CRVIEWER1)->ShowWindow(SW_SHOW);
					m_ReportViewer.ShowWindow(SW_SHOW);
					UpdateWindow();
				} 
				else
				{
					CRect rectTempViewer1;
					CRect rectTempViewerBlank;
					//GetDlgItem(IDC_CRVIEWER1)->GetWindowRect(rectTempViewer1);
					//GetDlgItem(IDC_CRVIEWER_BLANK)->GetWindowRect(rectTempViewerBlank);
					m_ReportViewer.GetWindowRect(rectTempViewer1);
					m_ReportViewerBlank.GetWindowRect(rectTempViewerBlank);
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
						"CCrutilDlg::OnFileClose, Viewer1 width: %d, height: %d. ViewerBlank width: %d, height: %d",
						rectTempViewer1.Width(), rectTempViewer1.Height(), rectTempViewerBlank.Width(), rectTempViewerBlank.Height() );

					//GetDlgItem(IDC_CRVIEWER1)->ShowWindow(SW_HIDE);
					//GetDlgItem(IDC_CRVIEWER_BLANK)->ShowWindow(SW_SHOW);
					m_ReportViewer.ShowWindow(SW_HIDE);
					m_ReportViewerBlank.ShowWindow(SW_SHOW);


					UpdateWindow();
				}

				if ( m_bShowToolBar == true )
				{
					m_pCrystalReportsBlank->setShowToolBar( false );
					m_pCrystalReportsBlank->setShowToolBar( true );
				}
				else
				{
					m_pCrystalReportsBlank->setShowToolBar( false );
				}

				//if ( m_bShowStatusBar == true )
				//{
				//	m_ProgressCtrl.ShowWindow(SW_HIDE);
				//	m_ProgressCtrl.ShowWindow(SW_SHOW);
				//} 

				GetMenu()->GetSubMenu(0)->EnableMenuItem(IDM_FILE_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

				this->SetWindowText(CAPTION_TITLE);
				m_ProgressCtrl.SetPos(0);
			}

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "End CCrutilDlg::OnFileClose");
		}

		void CCrutilDlg::OnReportManagerHelp()
		{
			TA_Base_Bus::HelpLauncher::getInstance().displayHelp(true);
		}

		void CCrutilDlg::OnUpdateShowToolBar(CCmdUI* pCmdUI)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin CCrutilDlg::OnUpdateShowToolBar");

			if ( m_bShowToolBar == true )
			{
				if ( ( true == m_pCrystalReports->setShowToolBar( false ) ) 
					&& ( true == m_pCrystalReportsBlank->setShowToolBar( false ) ) )
				{
					GetMenu()->GetSubMenu(1)->CheckMenuItem(ID_VIEW_RM_TOOLBAR, MF_BYCOMMAND | MF_UNCHECKED);

					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CCrutilDlg::OnUpdateShowToolBar, setToolBar unchecked success.");
					
					m_bShowToolBar = false;
				}
			} 
			else
			{
				if ( ( true == m_pCrystalReports->setShowToolBar( true ) ) 
					&& ( true == m_pCrystalReportsBlank->setShowToolBar( true ) ) )
				{
					GetMenu()->GetSubMenu(1)->CheckMenuItem(ID_VIEW_RM_TOOLBAR, MF_BYCOMMAND | MF_CHECKED);

					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CCrutilDlg::OnUpdateShowToolBar, setToolBar success.");

					m_bShowToolBar = true;
				}
			}

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "End CCrutilDlg::OnUpdateShowToolBar");
		}

		void CCrutilDlg::OnUpdateShowStatusBar(CCmdUI* pCmdUI)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin CCrutilDlg::OnUpdateShowStatusBar");

			if (IsWindow(m_ProgressCtrl.GetSafeHwnd()))
			{
				if ( m_bShowStatusBar == true )
				{
					m_ProgressCtrl.ShowWindow(SW_HIDE);
					GetMenu()->GetSubMenu(1)->CheckMenuItem(ID_VIEW_RM_STATUSBAR, MF_BYCOMMAND|MF_UNCHECKED);
					m_bShowStatusBar = false;
				} 
				else
				{
					m_ProgressCtrl.ShowWindow(SW_SHOW);
					GetMenu()->GetSubMenu(1)->CheckMenuItem(ID_VIEW_RM_STATUSBAR, MF_BYCOMMAND|MF_CHECKED);
					m_bShowStatusBar = true;
				}
			}

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "End CCrutilDlg::OnUpdateShowStatusBar");
		}

		void CCrutilDlg::saveWindowSize()
		{
			RECT rcSize;
			GetWindowRect( &rcSize );
			AppSettings obj( COMPANY_NAME, APPLICATION_NAME );
			obj.setNumberValue( REG_WINDOWTOP_LABEL, rcSize.top );		
			obj.setNumberValue( REG_WINDOWLEFT_LABEL, rcSize.left );		
			obj.setNumberValue( REG_WINDOWBOTTOM_LABEL, rcSize.bottom - rcSize.top );		
			obj.setNumberValue( REG_WINDOWRIGHT_LABEL, rcSize.right - rcSize.left );		
		}

		void CCrutilDlg::restoreWindowSize()
		{
            // Disabled to address the following TransActive defect.
            // Defect #2843 - Report Manager not launched within the schematic area
			//AppSettings obj( COMPANY_NAME, APPLICATION_NAME );
			//SetWindowPos( &wndTop, 
			//			  obj.getNumberValue( REG_WINDOWLEFT_LABEL),
			//			  obj.getNumberValue( REG_WINDOWTOP_LABEL),
			//			  obj.getNumberValue( REG_WINDOWRIGHT_LABEL),
			//			  obj.getNumberValue( REG_WINDOWBOTTOM_LABEL),
			//			  SWP_SHOWWINDOW );
		}

		void CCrutilDlg::OnDestroy() 
		{
			CDialog::OnDestroy();
			saveWindowSize();
		}

		void CCrutilDlg::OnClose() 
		{
			#ifdef TRANSACTIVE_BUILD
			TransActiveDialog::OnClose();
			#else
			CDialog::OnClose();
			#endif // TRANSACTIVE_BUILD
			DestroyWindow();
		}

		void CCrutilDlg::OnTimer(UINT nIDEvent)
		{
			if ( RM_PROGRESSCTRL_TIMER == nIDEvent )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					"CCrutilDlg::OnTimer" );
				int currentPos;
				currentPos = m_ProgressCtrl.GetPos();
				stepProgressBar( currentPos );
			}
		}

		void CCrutilDlg::stepProgressBar(int _currentPos)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"CCrutilDlg::stepProgressBar, CurrentPosition = %d, m_bOpenReportFileRate = %d", 
				_currentPos, OPEN_REPORT_FILE_RATE );

			if ( OPEN_REPORT_FILE_RATE >= OPEN_REPORT_RATE_COMPLETED
				|| _currentPos >= OPEN_REPORT_RATE_COMPLETED )
			{
				//m_ProgressCtrl.SetStep( 30 );
				m_ProgressCtrl.SetPos( OPEN_REPORT_RATE_COMPLETED );

				KillTimer( RM_PROGRESSCTRL_TIMER );

				OPEN_REPORT_FILE_RATE = OPEN_REPORT_RATE_INIT_TO_ZERO;

				return;
			}

			if ( _currentPos < OPEN_REPORT_FILE_RATE )
			{
				m_ProgressCtrl.SetPos(OPEN_REPORT_FILE_RATE);
			}
			else if ( OPEN_REPORT_FILE_RATE <= _currentPos < OPEN_REPORT_FILE_RATE + OPEN_REPORT_RATE_STEP )
			{
				m_ProgressCtrl.SetStep( 5 );
				m_ProgressCtrl.StepIt();
			}
			else // _currentPos >= m_bOpenReprotFileRate + 20
			{
				// Do nothing. Just wait for m_bOpenReportFileRate to a new value.
			}

			return;
		}

		void CCrutilDlg::displayAboutBox()
		{
			#ifdef TRANSACTIVE_BUILD

        	TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();

			#else

			CAboutDlg dlgAbout;
			dlgAbout.DoModal();

			#endif // TRANSACTIVE_BUILD

		}

		void CCrutilDlg::OnUpdateViewHistory(CCmdUI* pCmdUI)
		{
			pCmdUI->Enable(m_bViewHistory);
			pCmdUI->SetCheck(false);
		}

		LRESULT CCrutilDlg::OnKickIdle(WPARAM, LPARAM)
		{
			CMenu* pMainMenu = GetMenu();
			CCmdUI cmdUI;
			for (UINT n = 0; n < pMainMenu->GetMenuItemCount(); ++n)
			{
				CMenu* pSubMenu = pMainMenu->GetSubMenu(n);
				cmdUI.m_nIndexMax = pSubMenu->GetMenuItemCount();
				for (UINT i = 0; i < cmdUI.m_nIndexMax;++i)
				{
					cmdUI.m_nIndex = i;
					cmdUI.m_nID = pSubMenu->GetMenuItemID(i);
					cmdUI.m_pMenu = pSubMenu;
					cmdUI.DoUpdate(this, FALSE);
				}
			}

			m_ProgressCtrl.SendMessage(WM_IDLEUPDATECMDUI, TRUE);

			return TRUE;
		}

		void CCrutilDlg::OnUpdateKeyIndicator(CCmdUI* pCmdUI)
		{
			UINT nVK;
			UINT flag = 0x0001;

			switch (pCmdUI->m_nID)
			{
			case ID_INDICATOR_CAPS:
				nVK = VK_CAPITAL;
				break;

			case ID_INDICATOR_NUM:
				nVK = VK_NUMLOCK;
				break;

			case ID_INDICATOR_SCRL:
				nVK = VK_SCROLL;
				break;

			default:
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
					"CCrutilDlg::OnUpdateKeyIndicator, ¡°Warning: OnUpdateKeyIndicator ¨C unknown indicator");
				pCmdUI->ContinueRouting();
				return; // not for us
			}

			pCmdUI->Enable(::GetKeyState(nVK) & flag);
			// enable static text based on toggled key state
			TA_ASSERT(pCmdUI->m_bEnableChanged, "Change failed!");
		}


#ifdef TRANSACTIVE_BUILD
	} // TA_Reporting
} // TA_Base_App
#endif // TRANSACTIVE_BUILD

