/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/stis_manager/src/STISManagerDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2014/06/13 16:43:25 $
  * Last modified by:  $Author: huang.wenbo $
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
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/ApplicationException.h"
#include "core/exceptions/src/EntityTypeException.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "core/utilities/src/DebugUtil.h"
#include "boost/tokenizer.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Bus::TransActiveDialog;
using TA_Base_Core::RunParams;


namespace
{
	const int MINVALUE = 2400;
	const int NUMSCREENS = 3;
	std::string SELECTED_STIS_MSG_TAG("SelectedSTISMessage");
	std::string DISPLAY_MSG_TAG("Display");
}

namespace TA_IRS_App
{


	extern bool userClose;

    CSTISManagerDlg::CSTISManagerDlg( TA_Base_Bus::IGUIAccess& controlClass )
	    : TransActiveDialog(controlClass, CSTISManagerDlg::IDD, NULL),
		  m_shouldRepostionDialog(false)
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
		
		// Register for --display runparams here as well as 
		// the PIDSelectionManager
		// This object has to handle the restoration of the app from minimisation
		TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, "Display");
		TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, SELECTED_STIS_MSG_TAG.c_str());

		ModifyWindowPos();
		// Dialog should be minimize if user not press "Launch ..."
        std::string cmdline(GetCommandLine());
		if(cmdline.find("select") != std::string::npos)
		{
			ShowWindow(SW_MINIMIZE);
		}
		InitAll();
	    return TRUE;  // return TRUE  unless you set the focus to a control
    }

	void CSTISManagerDlg::OnShowWindow( BOOL bShow, UINT nStatus )
	{
		TransActiveDialog::OnShowWindow(bShow, nStatus);
	}

	void CSTISManagerDlg::InitAll()
	{
		m_mainTab.Init();
        // unsuppress messages
        UserMessages::getInstance().setMessageSuppression(false);
		// If the STIS Manager has crashed, pressing the 'Launch STIS Manager'
		// will start the app, as opposed to just sending a --display=xxx,n,SHOW
		// so the SHOW run param will be on the startup list, so it needs
		// to be processed here

		std::string preselectMsg = TA_Base_Core::RunParams::getInstance().get(SELECTED_STIS_MSG_TAG.c_str());
		std::string displayValue = TA_Base_Core::RunParams::getInstance().get("Display");
		if( preselectMsg != "")
		{
			onRunParamChange( SELECTED_STIS_MSG_TAG, preselectMsg);
		}
        m_pidController.initialise();
		m_initReady = true;
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
	    DestroyWindow();
    }


    void CSTISManagerDlg::OnHelpAbout() 
    {
	    TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();
    }


    LRESULT CSTISManagerDlg::OnSetWindowPosition(WPARAM wParam, LPARAM lParam)
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "wparam:%d, lparam:%d", wParam, lParam);
		// When press "WIN" button
		// 0 mean no rect argument, 2 mean FOCUS
		if( 0 == wParam && lParam == 2)
		{
			return TransActiveDialog::OnSetWindowPosition(wParam, lParam);
		}
		if(false == m_shouldRepostionDialog)
		{
			return 0;
		}
		m_shouldRepostionDialog = false;
		return TransActiveDialog::OnSetWindowPosition(wParam, lParam);
    }


    void CSTISManagerDlg::OnHelpStismanagerhelp() 
 	    
	{
		TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
    }


    void CSTISManagerDlg::onHelpButton() 
    {
        TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
    }


	void CSTISManagerDlg::OnClose()
	{
		userClose = true;
        DestroyWindow();
	}

	void CSTISManagerDlg::onBtnClose()
	{
		userClose = true;
		DestroyWindow();
	}


	void CSTISManagerDlg::activateSchematic()
	{
	}

	void CSTISManagerDlg::onRunParamChange( const std::string& name, const std::string& paramvalue )
    {
		std::vector<std::string> valueParts = tokenizeString( paramvalue, "," );
		if( (name.compare(SELECTED_STIS_MSG_TAG) == 0) && !(paramvalue.empty()) )
		{
			DisplayPage& displayPage = m_mainTab.getDisplayPage();
			displayPage.findAndSelectStationMessage(convertRunParam(paramvalue));
		}
		// Monitor --display parameter change
		if(name.compare(DISPLAY_MSG_TAG) == 0 && valueParts.size() == 3)
		{
			if(valueParts[2] == "HIDE")
			{
				m_shouldRepostionDialog = false;
			}
			else if(valueParts[2] == "SHOW")
			{
				m_shouldRepostionDialog = true;
			}
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

	void CSTISManagerDlg::OnAppExit() 
	{
		DestroyWindow();
	}

	bool CSTISManagerDlg::ModifyWindowPos(void) //zhongjie++ this function to modify the STisManager Dialog position in three monitor
	{
		int left,top,width,height;
		int screenWidth, screenHeight, currentScreen, perScreenWidth;
		int numScreens = 1;
		CPoint   pt;   
		GetCursorPos(&pt);   

		// Get dialog size
		RECT windowRect;
		GetWindowRect(&windowRect);
		LONG windowWidth = windowRect.right - windowRect.left + 1;
		LONG windowHeight = windowRect.bottom - windowRect.top + 1;

		// Get schematic rect
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

		// Get screen information
		screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		screenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
		if(screenWidth >= MINVALUE)
			numScreens = NUMSCREENS;
		perScreenWidth = screenWidth / numScreens;
		currentScreen = pt.x / perScreenWidth;

		// Calculate the window position
		left	= currentScreen * perScreenWidth + ( perScreenWidth - windowWidth) / 2;
		width	= windowWidth;
		if(windowHeight > (boundary.bottom - boundary.top + 1))
		{
		    top = boundary.top;
		}
		else
		{
			top = boundary.top + (boundary.bottom - boundary.top + 1 - windowHeight)/2; 
		}
		height	= windowHeight;

		this->MoveWindow(left,top,width,height,NULL);
		return true;
	}

} // TA_IRS_App
