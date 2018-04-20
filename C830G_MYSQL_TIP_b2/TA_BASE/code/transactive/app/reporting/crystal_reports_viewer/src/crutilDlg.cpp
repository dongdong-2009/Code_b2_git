/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/reporting/crystal_reports_viewer/src/crutilDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
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

#ifdef TRANSACTIVE_BUILD
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/about_box/src/TransActiveAboutBox.h"
#include "core/exceptions/src/ReportManagerException.h"

#include "cots/VisualCpp/Include/Winspool.h"

const int DATABASENAME_INDEX = 0;
const int USERID_INDEX = 1;
const int PASSWORD_INDEX = 2;
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
		: TransActiveDialog(*genericGUICallback, CCrutilDlg::IDD, pParent), m_bShowMaxWindow( false )
		#else
		CCrutilDlg::CCrutilDlg(CWnd* pParent /*=NULL*/)
			: CDialog(CCrutilDlg::IDD, pParent), m_bShowMaxWindow( false )
		#endif // TRANSACTIVE_BUILD
		{
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

			#endif // TRANSACTIVE_BUILD
		}

		CCrutilDlg::~CCrutilDlg()
		{
			if (m_pCrystalReports)
            {
                m_pCrystalReports->setViewerInstance( NULL );
				m_pCrystalReports->setBrowserInstance( NULL );
    			#ifdef TRANSACTIVE_BUILD
                delete m_pCrystalReports;
                #endif // TRANSACTIVE_BUILD
            }
		}

		void CCrutilDlg::DoDataExchange(CDataExchange* pDX)
		{
			CDialog::DoDataExchange(pDX);
			//{{AFX_DATA_MAP(CCrutilDlg)
			DDX_Control(pDX, IDC_CRVIEWER1, m_ReportViewer);
			DDX_Control(pDX, IDC_EXPLORER1, m_HistoryReportViewer);
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
			ON_WM_DESTROY()
			ON_COMMAND(ID_VIEW_HISTORY, OnViewHistory)
			ON_COMMAND(ID_VIEW_REFRESH, OnViewRefresh)
	ON_WM_CLOSE()
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
            // Create an instance of the Command Line Parser object.
            CommandLineParser objArgv;
			
            // If the report name parameter exists, then set the report name.
            if (objArgv.parameterExists( REPORTNAME_LABEL ))
			{
                std::vector<std::string> value = objArgv.getParameterValue( REPORTNAME_LABEL );
				m_cstrReportName = value[0].c_str();
				m_pCrystalReports->setReportName( m_cstrReportName.GetBuffer( m_cstrReportName.GetLength() ) );
            }
			
            // Set to fullscreen if the parameter has been supplied.
            if (objArgv.parameterExists( FULLSCREEN_LABEL )) m_bShowMaxWindow = true;
			
			//TD18040 
			//xxs add
			//get the historical report path
			if ( objArgv.parameterExists( HISTORY_REPORT_PATH_LABEL ) )
			{
                std::vector<std::string> value = objArgv.getParameterValue( HISTORY_REPORT_PATH_LABEL );
                m_pCrystalReports->setHistoryReportPathFromCmdLine( value[0].c_str());
			}
			//end xxs add
			
            // Enable parameter prompting if the parameter has been supplied.
			if (objArgv.parameterExists( NOPARAMPROMPTING_LABEL )) m_pCrystalReports->enableParameterPrompting( false );
			
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
                std::string operatorName = "";
                if (sessionId != "")
                {
                    TA_Base_Core::IOperator* op = TA_Base_Core::OperatorAccessFactory::
						getInstance().getOperatorFromSession(sessionId, false);
                    operatorName = op->getName();
                    delete op;
					
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
                    if (m_pCrystalReports->viewReport(reportParameters) == false)
					{
						// Error.
					}
                }
                catch(const TA_Base_Core::ReportManagerException& ex)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "ReportManagerException", ex.what());
                    TA_Base_Bus::TransActiveMessage userMsg;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_140036);
                }
			}
			
			// Restore the previous Windows size.
			restoreWindowSize();
			
			m_bViewHistory = false;			//not allow to view history reports when no report open
			
#ifndef TRANSACTIVE_BUILD
			
			// And display the window.
			if (m_bShowMaxWindow) ::ShowWindow( this->m_hWnd, SW_SHOWMAXIMIZED );
			
#endif
		}

		BOOL CCrutilDlg::OnInitDialog()
		{
            #ifdef TRANSACTIVE_BUILD
            
            // Defect #2843 - Report Manager not launched within the schematic area
            TransActiveDialog::OnInitDialog();
 
			//TD18065
			//xxs add
			if ( m_bShowMaxWindow )
			{
				ShowWindow(SW_SHOWMAXIMIZED);   
			}
			//end xxs add

			
            #else
            
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
			CDialog::OnSize(nType, cx, cy);

			if (m_pCrystalReports)
			{
				m_pCrystalReports->setWindowWidth( cx );
				m_pCrystalReports->setWindowHeight( cy );

				if (m_pCrystalReports->getViewingType() == CrystalReportsHelper::crhOnDemand)
				{
					if (m_ReportViewer.m_hWnd) m_ReportViewer.MoveWindow(0, 0, cx, cy);	
				}
				else
				{
					if (m_ReportViewer.m_hWnd) m_HistoryReportViewer.MoveWindow(0, 0, cx, cy);	
				}
			}
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

			if (m_pCrystalReports)
			{
				OpenFileDialog objOpenFile( TRUE, NULL, "", 0, "Report Templates (*.rpt)|*.rpt||", 0);
				objOpenFile.m_ofn.Flags = OFN_HIDEREADONLY | OFN_ENABLEHOOK | OFN_EXPLORER;
				// TD18033
				HINSTANCE hInst = ::ShellExecute(this->m_hWnd, "explore", "\\\\adm-occ-1\\C$\\transactive\\reports\\R-Report", NULL, NULL, SW_HIDE);

				if(reinterpret_cast<int>(hInst) <= 32)
				{
					objOpenFile.m_ofn.lpstrInitialDir="C:\\transactive\\reports\\R-Report";

					// popup message dialog, "Unable to connect to ADMIN MFT."
					TA_Base_Bus::TransActiveMessage userMsg;
					UINT selectedButton = userMsg.showMsgBox(IDS_UW_140002);
				}
				else
				{
					objOpenFile.m_ofn.lpstrInitialDir="\\\\adm-occ-1\\C$\\transactive\\reports\\R-Report";
				}
				// TD18033
				
				if (objOpenFile.DoModal() == IDOK)
				{
					m_pCrystalReports->setReportName( objOpenFile.GetPathName().GetBuffer(100) );
					m_pCrystalReports->setViewingType( CrystalReportsHelper::crhOnDemand );
					m_pCrystalReports->setDialogInstance( this );

                    // No parameters to pass in - pass in empty vector
                    std::vector<std::string> parameterValuePairs;
					if (m_pCrystalReports->viewReport(parameterValuePairs) == false)
					{
                        TA_Base_Bus::TransActiveMessage userMsg;
                        UINT selectedButton = userMsg.showMsgBox(IDS_UE_140033);
                    }
					else
					{
						m_bViewHistory = true;		//allow to view history
  					}
				}
			}
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
			return TRUE;
		}

#ifdef TRANSACTIVE_BUILD
	} // TA_Reporting
} // TA_Base_App
#endif // TRANSACTIVE_BUILD
