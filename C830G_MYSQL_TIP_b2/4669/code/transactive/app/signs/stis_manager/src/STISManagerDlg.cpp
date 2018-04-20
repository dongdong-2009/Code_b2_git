/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
// STISManagerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "STISManager.h"
#include "STISManagerDlg.h"
#include "UserMessages.h"

//#include "app/signs/STISManager/src/RunParamListener.h"
#include "app/signs/stis_manager/src/GraphworxComms.h"
#include "app/signs/stis_manager/src/DisplayPage.h"

#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/GenericGuiConstants.h"

#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/utilities/src/RunParams.h"

#include "core/data_access_interface/src/Location.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

#include "core\data_access_interface\entity_access\src\EntityAccessFactory.h"
#include "core\data_access_interface\entity_access\src\IEntityData.h"
#include "core\data_access_interface\entity_access\src\DataNodeEntityData.h"
#include "core\data_access_interface\entity_access\src\DataPointEntityData.h"

#include "core\data_access_interface\entity_access\src\TISAgentEntityData.h"
#include "core\data_access_interface\entity_access\src\STISEntityData.h"

#include "core/exceptions/src/ApplicationException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/EntityTypeException.h"
#include "bus/generic_gui/src/AppLauncher.h"	//zhongjie++
#include "core/utilities/src/DebugUtil.h"//zhongjie++



#include "boost/tokenizer.hpp"
//haipeng added
#include "REBProgressManager.h"
//haipeng added

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TA_INIT_STIS (WM_USER + 1002)


#define SM_CXVIRTUALSCREEN      78		//zhongjie++
#define MINVALUE				2400	//zhongjie++
#define NUMSCREENS				3		//zhongjie++


using TA_Base_Bus::TransActiveDialog;
using TA_Base_Core::RunParams;

namespace
{
	std::string SELECTED_STIS_MSG_TAG("SelectedSTISMessage");
}

namespace TA_IRS_App
{


    CSTISManagerDlg::CSTISManagerDlg( TA_Base_Bus::IGUIAccess& controlClass )
	    : TransActiveDialog(controlClass, CSTISManagerDlg::IDD, NULL),
          m_initialDisplay( false ), m_wantToShow( false ),m_initReady(false)
    {
	    //{{AFX_DATA_INIT(CSTISManagerDlg)
	    //}}AFX_DATA_INIT
	    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    }


    CSTISManagerDlg::~CSTISManagerDlg()
    {
    }


    void CSTISManagerDlg::DoDataExchange(CDataExchange* pDX)
    {
	    TransActiveDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(CSTISManagerDlg)
	    DDX_Control(pDX, IDC_MAINPAGE_TAB, m_mainTab);
	    //}}AFX_DATA_MAP
    }
	
	
    BEGIN_MESSAGE_MAP(CSTISManagerDlg, TransActiveDialog)
		//{{AFX_MSG_MAP(CSTISManagerDlg)
		ON_WM_PAINT()
		ON_WM_QUERYDRAGICON()
		ON_COMMAND(ID_FILE_EXIT, OnFileExit)
		ON_COMMAND(ID_HELP_ABOUTSTISMANAGER, OnHelpAbout)
		ON_COMMAND(ID_HELP_STISMANAGERHELP, OnHelpStismanagerhelp)
		ON_WM_SIZE()
        ON_WM_ACTIVATEAPP()
		ON_BN_CLICKED(IDC_HELP_BUTTON, onHelpButton)
		ON_BN_CLICKED(IDC_CLOSE, onBtnClose)
        ON_REGISTERED_MESSAGE(TA_Base_Bus::TA_GenericGui::WM_SET_WINDOW_POSITION, OnSetWindowPosition)
		ON_WM_SHOWWINDOW()
		ON_MESSAGE(TA_INIT_STIS, OnInitSTIS)
		ON_WM_CLOSE()
		ON_COMMAND(ID_APP_EXIT, OnAppExit)
		//}}AFX_MSG_MAP
        END_MESSAGE_MAP()


    BOOL CSTISManagerDlg::OnInitDialog()
    {
	    CRect windowSize;
        GetWindowRect(&windowSize);
        TA_Base_Bus::ResizingProperties properties;
        properties.canMaximise = false;
        properties.maxHeight = -1;
        properties.maxWidth = -1;
        properties.minHeight = windowSize.bottom - windowSize.top;
        properties.minWidth = windowSize.right - windowSize.left;
        setResizingProperties(properties);

        TransActiveDialog::OnInitDialog();
		// Set the icon for this dialog.  The framework does this automatically
		//  when the application's main window is not a dialog
		SetIcon(m_hIcon, TRUE);			// Set big icon
		SetIcon(m_hIcon, FALSE);		// Set small icon
		
		//Maochun++
		//TD12780
		CDialog* displayDlg = m_mainTab.getMainTabPage(0);
		CDialog* versionDlg = m_mainTab.getMainTabPage(1);
		
		
		displayDlg->SetWindowPos(NULL, windowSize.left+20, 
			windowSize.top+30, windowSize.Width()-42, 
			windowSize.Height()-133, SWP_NOZORDER);
		
		versionDlg->SetWindowPos(NULL, windowSize.left+20, 
			windowSize.top+30, windowSize.Width()-42, 
			windowSize.Height()-133, SWP_NOZORDER);

		ModifyWindowPos();
        std::string cmdline(GetCommandLine());
		if(cmdline.find("select") != std::string::npos)
		{
			ShowWindow(SW_MINIMIZE);
		}
         
	    return TRUE;  // return TRUE  unless you set the focus to a control
    }

	void CSTISManagerDlg::OnShowWindow( BOOL bShow, UINT nStatus )
	{
		TransActiveDialog::OnShowWindow(bShow, nStatus);
	}

	LRESULT CSTISManagerDlg::OnInitSTIS( WPARAM wParam, LPARAM lParam)
	{
		CWaitCursor cos;
		InitAll();
		cos.Restore();
		return 0;
	}

	void CSTISManagerDlg::InitAll()
	{
		//show the progress dialog
		// TODO: Add extra validation here
		//REBProgressManager mgr;
		
		//mgr.Init(GetDesktopWindow()->m_hWnd);	//
		//mgr.EnableProgress(false);
		//mgr.SetVisible(false);	
		//mgr.EnableCancel(FALSE);
		//mgr.BeginProgressDialog();
		//UserMessages::getInstance().setParent(mgr.getProgressWnd());
		//mgr.SetCaption("STIS manager initializing...");		
		
		//mgr.SetProgress(1);
		//mgr.SetStaticText(0, "Initializing the subsystem pages...");

		// Hide the GUI by default initially
		m_wantToShow = false;
		//ShowWindow(SW_MINIMIZE);//to show

		m_mainTab.Init();

        // unsuppress messages
        UserMessages::getInstance().setMessageSuppression(false);
		
		// Register for --display runparams here as well as 
		// the PIDSelectionManager
		// This object has to handle the restoration of the app from minimisation
		TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, "Display");
		TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, SELECTED_STIS_MSG_TAG.c_str());
		
		// If the STIS Manager has crashed, pressing the 'Launch STIS Manager'
		// will start the app, as opposed to just sending a --display=xxx,n,SHOW
		// so the SHOW run param will be on the startup list, so it needs
		// to be processed here

		//16350 libo 
		m_bFirstShow = true;
		std::string preselectMsg = TA_Base_Core::RunParams::getInstance().get(SELECTED_STIS_MSG_TAG.c_str());
		if(preselectMsg !="")
			m_bMaxFlag = true;
		else
			m_bMaxFlag = false;
		//16350 libo 

		std::string displayValue = TA_Base_Core::RunParams::getInstance().get("Display");
		if( displayValue != "")
        {
            onRunParamChange( "Display", displayValue);
        }
		
		//TD 15349
		//zhou yuan++
	//	std::string preselectMsg = TA_Base_Core::RunParams::getInstance().get(SELECTED_STIS_MSG_TAG.c_str());
		if( preselectMsg != "")
		{
			onRunParamChange( SELECTED_STIS_MSG_TAG, preselectMsg);
		}
		//++zhou yuan
		
        m_pidController.initialise();
		//mgr.SetProgress(100);
		m_initReady = true;
		//UserMessages::getInstance().setParent(this);
	}

    // If you add a minimize button to your dialog, you will need the code below
    //  to draw the icon.  For MFC applications using the document/view model,
    //  this is automatically done for you by the framework.
    void CSTISManagerDlg::OnPaint() 
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
		    TransActiveDialog::OnPaint();
	    }
    }


    // The system calls this to obtain the cursor to display while the user drags
    //  the minimized window.
    HCURSOR CSTISManagerDlg::OnQueryDragIcon()
    {
	    return (HCURSOR) m_hIcon;
    }


    void CSTISManagerDlg::OnFileExit() 
    {
		//TD18095, jianghp, to fix the performance of showing manager application
	    DestroyWindow();
    }


    void CSTISManagerDlg::OnHelpAbout() 
    {
	    TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();
    }

    void CSTISManagerDlg::OnSize(UINT nType, int cx, int cy) 
    {
	    // If the 'Launch...' button has been pressed
	    // only then should the app be 
	    if(( m_wantToShow ) && ( nType == 0 ))
	    {
		    TransActiveDialog::OnSize(nType, cx, cy);
	    }
	    else
	    {	
		    if( nType == 1 ) // Minimise
		    {
			    m_wantToShow = false;  // make sure the window doesn't pop up on PID selections now it is minimised
			    return;
		    }
	    }
    }


    void CSTISManagerDlg::OnActivateApp( BOOL bActive, DWORD hTask )
    {
        if (!bActive)
        {
            // De-activation, no longer want to show the application when it's de-activated
            if (m_initialDisplay)
            {
                m_wantToShow = false;

                // suppress messages
                UserMessages::getInstance().setMessageSuppression(true);
            }
        }

        TransActiveDialog::OnActivateApp( bActive, hTask );
    }


    LRESULT CSTISManagerDlg::OnSetWindowPosition(WPARAM wParam, LPARAM lParam)
    {
        // from the PA manager
        // TES #720
        // don't show it if we don't want it to be shown
        if (lParam == TA_Base_Bus::TA_GenericGui::NO_REPOSITION)
        {
	
            return 0;
        }
		//TD16102
		if(lParam == TA_Base_Bus::TA_GenericGui::FOCUS)
		{
	
			return TransActiveDialog::OnSetWindowPosition(wParam, lParam); 
	
		}
		//TD16102

		else
		{
			return -1;
		}

        if(!m_wantToShow && lParam == TA_Base_Bus::TA_GenericGui::REPOSITION)
        {
		
             //return TransActiveDialog::OnSetWindowPosition(wParam, TA_Base_Bus::TA_GenericGui::REPOSITION_NO_RESTORE);
            //TransActiveDialog::OnSetWindowPosition(wParam, TA_Base_Bus::TA_GenericGui::REPOSITION_NO_RESTORE);
			if(m_bFirstShow)
			{
				return TransActiveDialog::OnSetWindowPosition(wParam, TA_Base_Bus::TA_GenericGui::MINIMISE);
		
			}
        }
		else if(m_wantToShow && lParam == TA_Base_Bus::TA_GenericGui::REPOSITION && m_bMaxFlag)
		{
			
  		  m_bMaxFlag = false;
          return TransActiveDialog::OnSetWindowPosition(wParam, TA_Base_Bus::TA_GenericGui::REPOSITION_NO_RESTORE);
		  
		}
		m_bFirstShow = false;
		{
			ACE_GUARD_RETURN(ACE_Thread_Mutex, ace_mon, PidSelectionManager::Tag_mutex, -1);
			if(PidSelectionManager::DONTGOFRONT_TAG == 0)
			{
		
				return TransActiveDialog::OnSetWindowPosition(wParam, lParam);
				
	
			}
			-- PidSelectionManager::DONTGOFRONT_TAG;
		}
		return 0;
		// TD16350,16285 - STIS Manager overlap screen on LED View Current message
		// remove else statement
		/* else 
        {   
            return TransActiveDialog::OnSetWindowPosition(wParam, lParam);
        } */

		//return TransActiveDialog::OnSetWindowPosition(wParam, lParam); 
    }


    void CSTISManagerDlg::OnHelpStismanagerhelp() 
 	    
	{
		TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
    }


    void CSTISManagerDlg::onHelpButton() 
    {
        TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
    }


    void CSTISManagerDlg::onClose() 
    {
		//TD18095, jianghp, to fix the performance of showing manager application
        DestroyWindow();
    }

	void CSTISManagerDlg::onBtnClose()
	{
		onClose();
	}

	void CSTISManagerDlg::activateSchematic()
	{
        /*
		bool schematicUpdated = false;

		// Now we've finished loading, we may activate the graphworx display we're operating for
		try
		{
			//std::string schematicId = RunParamListener::getInstance().getSchematicIdentifier();
			std::string schematicId;

			schematicUpdated = TA_IRS_App::GraphworxComms::getInstance().activateGraphworxDisplay(schematicId);
		}
		catch (const TA_Base_Core::ValueNotSetException& e)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "ValueNotSetException", e.what());        
		}
		catch (...)
		{
			// Silently log any error communicating with the actual schematic, may still run
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "While activating schematic");        
		}
*/
/*    
	#ifdef _DEBUG
		// Perform an override if a debug session
		if (CachedConfig::getInstance()->getSessionId().compare("debug") == 0)
		{
			schematicUpdated = true;
		}
	#endif

		if (!schematicUpdated)
		{
			//PaErrorHandler::displayModalError(PaErrorHandler::ERROR_SCHEMATIC_COMMS);
			//PostQuitMessage(0);    
		}
*/

	}

	void CSTISManagerDlg::onRunParamChange( const std::string& name, const std::string& value )
    {
		std::vector<std::string> valueParts = tokenizeString( value, "," );
		if( (name.compare("Display") == 0) && (valueParts[2].compare("SHOW") == 0))
		{
			m_wantToShow = true;
            if ( ::IsWindow(m_hWnd) )
			{
                ShowWindow(SW_RESTORE);
                SetForegroundWindow();
            }
		}

		if( (name.compare(SELECTED_STIS_MSG_TAG) == 0) && !(value.empty()) )
		{
			m_bMaxFlag = true;
			DisplayPage& displayPage = m_mainTab.getDisplayPage();
			displayPage.findAndSelectStationMessage(convertRunParam(value));
		}

	}

    std::vector<std::string> CSTISManagerDlg::tokenizeString(std::string theString, const std::string& separatorList )
	{
        std::vector<std::string> parts;
        typedef boost::tokenizer< boost::char_separator<char> > tokenizer;

        boost::char_separator<char> sep( separatorList.c_str() );
        tokenizer tokens(theString, sep);
        
        for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
        {
            parts.push_back(*tok_iter);
        }

        // if parts is empty, then this should return the entire string
        if (parts.size() == 0)
        {
            parts.push_back(theString);
        }

        return parts;
	}

	//TD 15349
	//zhou yuan++
	std::string CSTISManagerDlg::convertRunParam(const std::string& org)
	{
		std::string ret = org;
		std::string::size_type index = ret.find('_'); 
		while(index != std::string::npos)
		{
			ret.replace(index, 1, " ");
			index = ret.find('_', index);
		}
		return ret;
	}
	//++zhou yuan
	void CSTISManagerDlg::OnAppExit() 
	{
		DestroyWindow();
	}





	bool CSTISManagerDlg::ModifyWindowPos(void) //zhongjie++ this function to modify the STisManager Dialog position in three monitor
	{
		int left,top,width,height;
		int screenWidth,currentScreen;
		int numScreens = 1;
		int x_pos;

		CPoint   pt;   
        GetCursorPos(&pt);   
		RECT boundary;
		try
		{
			boundary = TA_Base_Bus::AppLauncher::getInstance().getRect(TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT, 
																	TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC,
	        															pt.x);
		}
		catch(const TA_Base_Core::ApplicationException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ApplicationException", ex.what());
			return true;
		}
        x_pos = pt.x;
		screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		if(screenWidth >= MINVALUE)
			numScreens = NUMSCREENS;
        currentScreen = x_pos / (screenWidth / numScreens);
		left	= currentScreen * (screenWidth / numScreens)+1;
		width	= screenWidth / numScreens;
		top		= boundary.top;
		height	= boundary.bottom - boundary.top;

		this->MoveWindow(left,top,width,height,NULL);
		return true;
	}

} // TA_IRS_App
