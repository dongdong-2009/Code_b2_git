/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/system_control/banner/pages/call_page/src/CallPage.cpp $
  * @author:  C. DeWolfe - Heavily modified by Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This class provides a banner that lists the communications for the
  * operator.
  * The operator may find out more information by double-clicking on an entry
  * in the list.
  *
  * This class derives from BannerPage which allows it to be a page in the Banner
  * Framework.
  */


// Disable "identifier truncated" warning
#if defined(_MSC_VER)
#pragma warning(disable:4786 4284 4250)
#endif // _MSC_VER


#include <sstream>
#include <crtdbg.h>

#include "app\system_control\banner\pages\call_page\src\stdafx.h"
#include "app/system_control/banner/pages/call_page/src/CallPage.h"
#include "app/system_control/banner/pages/call_page/src/CallBannerPage_rc.h"
#include "app/system_control/banner/pages/call_page/src/RightsChecker.h"
#include "bus/banner_framework/src/constants.h"
#include "bus/banner_framework/src/BannerFrameworkDialog.h"
#include "bus/resources/resource.h"
#include "app/system_control/banner/pages/call_page/src/CallBannerInitialiser.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/Banner.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "bus/banner_pages/call_page_library/src/CallEntryDoesNotExistException.h"
#include "bus/banner_pages/call_page_library/src/ICallEntry.h"
#include "bus/banner_pages/call_page_library/src/AbstractCallEntry.h"
#include "bus/radio/AuthorisationCallBannerLibrary/src/AuthorisationCallEntry.h"
#include "bus/radio/RadioCallBannerLibrary/src/RadioCallEntry.h"
#include "bus/radio/RadioCallBannerLibrary/src/CallDriverCallEntry.h"
//#include "bus/telephony/TelephoneCallBannerLibrary/src/TelephoneCallEntry.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"


#include "bus/trains/PECCallBannerLibrary/src/PECCallEntry.h"



using TA_IRS_Bus::AuthorisationCallEntry;
using TA_IRS_Bus::RadioCallEntry;
using TA_IRS_Bus::CallDriverCallEntry;
using TA_IRS_Bus::ICallEntry;
using TA_IRS_Bus::AbstractCallEntry;
using TA_IRS_Bus::PECCallEntry;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::EntityAccessFactory;
using TA_Base_Core::IEntityData;
using TA_Base_Core::Banner;
using TA_Base_Core::DataException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::CorbaUtil;



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace TA_IRS_App
{
    const COLORREF CallPage::DEFAULT_TEXT_COLOUR = RGB(255, 255, 255);
    const COLORREF CallPage::BACKGROUND_COLOUR   = RGB(0,   0,   0);

    // Propweb case #3240: Red text on black background        
    static const COLORREF EMERGENCY_ITEM_FOREGROUND_COLOUR   = RGB(255,   0,   0);
    static const COLORREF EMERGENCY_ITEM_BACKGROUND_COLOUR   = RGB(0,   0,   0);

    //static const DWORD WM_ADD_CALL = RegisterWindowMessage("CallPage_addCall");
   

	// ++
    // 4669Pre-FAT TD#13762
    const int CallPage::PEC_FLASH_TIMER_ID = 1001;
    const int CallPage::PEC_FLASH_TIMER_INTERVAL = 500;
    // ++

    static enum 
    {
        COL_TIME,
        COL_TYPE,
        COL_SOURCE,
        COL_LOC,
        COL_INFO,
        COL_STATUS,
        CALL_MAX
    };

    CallPage::CallPage(const std::string& buttonName, int xPos, int yPos, int width, int height, TA_Base_Bus::BannerFrameworkDialog* parentDialog)
	    :
        BannerPage(buttonName, CallPage::IDD, xPos, yPos, width, height, parentDialog),
        m_callEntryManager(NULL),
        m_callBannerInitialiser(NULL),
        m_initialised(false),
        m_CallListCtrl(CALL_MAX),
		m_resetCounter(0),
        m_locationKey(0)
    {
	    //{{AFX_DATA_INIT(CallPage)
	    //}}AFX_DATA_INIT
    }


    void CallPage::initialiseBanner()
    {
        std::string errorMessage;


		//m_pecBtn.ShowWindow(SW_HIDE);
		m_vettingBtn.ShowWindow(SW_HIDE);
		m_rxhBtn.SetWindowText("Telephony");	
		m_rxhBtn.ShowWindow(SW_HIDE);
		m_authorBtn.SetWindowText("Auth");
		m_authorBtn.setCheck(false);
		
		// no specification yet to include auth
		m_authorBtn.ShowWindow(SW_HIDE);

        try
        {
            delete m_callEntryManager;
            m_callEntryManager = NULL;

            m_callEntryManager =
                new TA_IRS_Bus::CallEntryManager(*this, m_locationKey);

			m_CallListCtrl.setCallEntryManager(m_callEntryManager);
            
            initialiseCallTypeFilterInfo();

            m_initialised = true;

            // post the message to the GUI thread to do the GUI updates
            // because this method runs in its own thread.
            int ret = PostMessage( CALL_PAGE_INITIALISED, 0, 0 );
            if (ret == 0)
            {
                LOG_GENERIC(
                SourceInfo, DebugUtil::DebugError,
                "Error posting Call Page Initialised message");
            }
        }
        catch(const DataException& ex)
        {
            errorMessage = "Data Exception: " + std::string(ex.what());
        }
        catch(const DatabaseException& ex)
        {
            errorMessage = "Database Exception: " + std::string(ex.what());
        }
        catch(const CORBA::Exception& ex)
        {
            errorMessage = "Corba Exception: " +
                           CorbaUtil::getInstance().exceptionToString(ex);
        }
        catch(const std::exception& ex)
        {
            errorMessage = "Exception: " + std::string(ex.what());
        }
        catch(...)
        {
            errorMessage = "Unknown Exception.";
        }

        if( !errorMessage.empty() )
        {
            //
            // If the call banner could not be initialised, show a message in
            // the banner saying so.
            //
            m_CallListCtrl.SetRedraw(FALSE);

            m_CallListCtrl.DeleteAllItems();
            m_CallListCtrl.clearItemColourSettings();
            
            int index = m_CallListCtrl.InsertItem(0, "");

            // In the source column
            m_CallListCtrl.SetItem(
                index, COL_SOURCE, LVIF_TEXT,
                "Could not initialise call banner", // TODO: UE ????
                0, 0, 0, 0 );

            // In the information column
            m_CallListCtrl.SetItem(
                index, COL_INFO, LVIF_TEXT, errorMessage.c_str(), 0, 0, 0, 0 );

            
            m_CallListCtrl.setItemColour(0,
                                            DEFAULT_TEXT_COLOUR,
                                            TA_Base_Bus::ListCtrlSelNoFocus::I_ITEMDATA,
                                            TA_Base_Bus::ListCtrlSelNoFocus::CT_FOREGROUND);
            m_CallListCtrl.setItemColour(0,
                                            BACKGROUND_COLOUR,
                                            TA_Base_Bus::ListCtrlSelNoFocus::I_ITEMDATA,
                                            TA_Base_Bus::ListCtrlSelNoFocus::CT_BACKGROUND);
            
            m_CallListCtrl.SetRedraw(TRUE);
        }
    }


	
    // ++
    // 4669Pre-FAT TD#13762
    // After make a PEC call, the "PEC" button should be flashing in red color
    void CallPage::refreshPECButtonFlashingState()
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"[ENTRY] refreshPECButtonFlashingState");
        ToggleButton* pBtn = static_cast<ToggleButton*>( GetDlgItem(IDC_PEC_BUTTON) );
	    
        unsigned int numPECCalls =
                    m_callEntryManager->getNumberOfCallEntriesOfType(PECCallEntry::CALL_ENTRY_TYPE);
        
        if (numPECCalls > 0)
        {
            if (!m_isPECFlashing)
            {
                // there's a PEC call so make the "PEC" button flashing
                m_isPECFlashing = true;
                SetTimer(PEC_FLASH_TIMER_ID, PEC_FLASH_TIMER_INTERVAL, NULL);         				
            }
        }
        else
        {
            if (m_isPECFlashing)
            {
                m_isPECFlashing = false;

                // no PEC call so stop flashing            
                KillTimer(PEC_FLASH_TIMER_ID);            

                // set to default color incase the current 
                // color is not the default
                m_pecBtn.SetCheckedColour(BANNER_CONST::CCL_GOLD);
				m_pecBtn.SetUnCheckedColour(::GetSysColor(COLOR_BTNFACE));
            }
        }
    }    

    void CallPage::initialiseCallTypeFilterInfo()
    {
        //
        // Add each call type that the banner displays to this method.
        //

        TA_ASSERT( RunParams::getInstance().isSet(RPARAM_ENTITYNAME),
                   "Entity Key Run Param must be set" );

        std::string entity = RunParams::getInstance().get(RPARAM_ENTITYNAME);

        //
        // Get the colour of each call type from the database
        //
/*        std::auto_ptr<IEntityData> entityData(
                        EntityAccessFactory::getInstance().getEntity(entity));
        Banner* bannerData = dynamic_cast<Banner*>(entityData.get());
        
        TA_ASSERT(bannerData != NULL,
                  "Entity Data pointer must point to Banner subclass");

        COLORREF authColour  = DEFAULT_TEXT_COLOUR;
        COLORREF radioColour = DEFAULT_TEXT_COLOUR;

        try
        {
            authColour =
                bannerData->getCallTypeColour(
                    AuthorisationCallEntry::CALL_ENTRY_TYPE);

            radioColour =
                bannerData->getCallTypeColour(
                    RadioCallEntry::CALL_ENTRY_TYPE);
        }
        catch(...)
        {
            LOG_GENERIC(
                SourceInfo, DebugUtil::DebugError,
                "Could not retrieve call entry colours from database. "
                "Using default colours");

            authColour  = DEFAULT_TEXT_COLOUR;
            radioColour = DEFAULT_TEXT_COLOUR;
        }
*/
        COLORREF PECColour   = DEFAULT_TEXT_COLOUR;
        COLORREF authColour  = DEFAULT_TEXT_COLOUR;
        COLORREF radioColour = DEFAULT_TEXT_COLOUR;

        // clear the list before trying to fill it.
        // prevents duplicate entries if initialised multiple times by the initialisation thread.
        m_callTypeFilterInfo.clear();

        RightsChecker checker;

        CallTypeInfo info;
        info.isVisible  = true;

		
        info.hasRights  = checker.canPerformAction(TA_Base_Bus::aca_RADIO_ANSWER_PEC_CALL);
		info.buttonId   = IDC_PEC_BUTTON;
        info.callType   = PECCallEntry::CALL_ENTRY_TYPE;
        info.buttonName = "PEC";
        info.colour     = PECColour;
        m_callTypeFilterInfo.push_back(info);

// 		//Fixed TD#14311
//         info.hasRights  = true; //checker.canPerformAction(TA_Base_Bus::aca_RADIO_AUTHORISE_CALL);
//         info.buttonId   = IDC_AUTH_BUTTON;
//         info.callType   = AuthorisationCallEntry::CALL_ENTRY_TYPE;
//         info.buttonName = "Auth";
//         info.colour     = authColour;
//         m_callTypeFilterInfo.push_back(info);

        info.hasRights  = checker.canPerformAction(TA_Base_Bus::aca_RADIO_ANSWER_CALL);
        info.buttonId   = IDC_RADIO_BUTTON;
        info.callType   = RadioCallEntry::CALL_ENTRY_TYPE;
        info.buttonName = "Radio";
        info.colour     = radioColour;
        m_callTypeFilterInfo.push_back(info);
		
		// call driver should be 
		// using another set of button
		// do not share with radio
		// wait first the requiremnt change
 		info.hasRights  = checker.canPerformAction(TA_Base_Bus::aca_RADIO_MAKE_TRAIN_CALL);
		info.buttonId   = IDC_RADIO_BUTTON;
		info.callType   = CallDriverCallEntry::getStaticType();
		info.buttonName = "Radio";
		info.colour     = radioColour;
		m_callTypeFilterInfo.push_back(info);

        //info.hasRights  = true; //checker.canPerformAction(TA_Base_Bus::aca_STATION_TIS_RATIS_APPROVE);
        //info.buttonId   = IDC_RXH_BUTTON;
        //info.callType   = TA_IRS_Bus::TelephoneCallEntry::CALL_ENTRY_TYPE;
        //info.buttonName = "Telephony";
        //info.colour     = DEFAULT_TEXT_COLOUR;
        //m_callTypeFilterInfo.push_back(info);

    }


    CallPage::~CallPage()
    {
        if(m_callEntryManager != NULL)
		{
			//m_callEntryManager->performLogout();
			delete m_callEntryManager;
		}
        m_callEntryManager = NULL;
        
        if(m_callBannerInitialiser != NULL)
        {
            m_callBannerInitialiser->terminateAndWait();
            
            delete m_callBannerInitialiser;
            m_callBannerInitialiser = NULL;
        }
    }


    unsigned long CallPage::getLocationKey()
    {
		// Note: We need the physical location of this operator and not
		// the logical location (as specified at login)
		
		TA_Base_Core::IConsole* console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(RunParams::getInstance().get(RPARAM_SESSIONID));
		TA_ASSERT(console!=NULL,"No console entity returned.");
		unsigned long locationKey = console->getLocation();
		delete console;

        return locationKey;
    }


    void CallPage::DoDataExchange(CDataExchange* pDX)
    {
	    TStackedPage::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(CallPage)
	    DDX_Control(pDX, IDC_AUTH_BUTTON, m_authorBtn);
	    DDX_Control(pDX, IDC_RXH_BUTTON, m_rxhBtn);
	    DDX_Control(pDX, IDC_RADIO_BUTTON, m_radioBtn);
	    DDX_Control(pDX, IDC_PEC_BUTTON, m_pecBtn);
	    DDX_Control(pDX, IDC_CALL_LIST, m_CallListCtrl);
	    //}}AFX_DATA_MAP
        DDX_Control(pDX, IDC_VETTING_CHECK, m_vettingBtn);
    }
    
    BEGIN_MESSAGE_MAP(CallPage, TStackedPage)
	    //{{AFX_MSG_MAP(CallPage)
	    ON_NOTIFY(NM_DBLCLK, IDC_CALL_LIST, OnDblclkCallList)
        ON_WM_SHOWWINDOW()
        ON_MESSAGE(CALL_PAGE_INITIALISED, OnFullRefreshRequest)
		ON_MESSAGE(CALL_PAGE_TOBE_REMOVED,OnDisableCallPage)
        ON_MESSAGE(CALL_PAGE_ADD_CALL, onAddCall)
        ON_MESSAGE(CALL_PAGE_ACTIVATE_CALL, onActivateCall)
		ON_MESSAGE(CALL_PAGE_REMOVE_CALL, onRemoveCall)
		ON_MESSAGE(CALL_PAGE_REFRESH_CALL, onRefreshCall)
		ON_MESSAGE(CALL_PAGE_REFRESH_CALLS, onRefreshCalls)
		ON_WM_TIMER()
        ON_WM_DESTROY()
		//}}AFX_MSG_MAP
      	ON_BN_CLICKED(IDC_VETTING_CHECK, onVettingCheckClicked)

	    ON_CONTROL_RANGE( BN_CLICKED, IDC_PEC_BUTTON, IDC_AUTH_BUTTON, onButtonClicked )
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CallPage message handlers

    BOOL CallPage::OnInitDialog() 
    {
	    TStackedPage::OnInitDialog();
        
        TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);

        //
        // Put the list control in the required position
        //
	    m_CallListCtrl.MoveWindow( 190, 0, getWidth() - 190, getHeight() - 1 );
	    m_CallListCtrl.SetExtendedStyle( m_CallListCtrl.GetExtendedStyle() | 
		    LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

        //
        // Add the columns (approx 1060 pixels to work with)
        //
	    m_CallListCtrl.InsertColumn( COL_TIME,      "Date/Time",     LVCFMT_LEFT,    150);
        m_CallListCtrl.InsertColumn( COL_TYPE,      "Type",     LVCFMT_LEFT,    80);
	    m_CallListCtrl.InsertColumn( COL_SOURCE,    "Source",   LVCFMT_LEFT,    235 );
        m_CallListCtrl.InsertColumn( COL_LOC,       "Loc",      LVCFMT_LEFT,    60 );
	    m_CallListCtrl.InsertColumn( COL_INFO,      "Information", LVCFMT_LEFT, 385 );
	    m_CallListCtrl.InsertColumn( COL_STATUS,    "Status",   LVCFMT_LEFT,    150 );

        // Fill in remaining space..
        m_CallListCtrl.SetColumnWidth(COL_STATUS, LVSCW_AUTOSIZE_USEHEADER);
        DWORD colWidth = m_CallListCtrl.GetColumnWidth(COL_STATUS) - GetSystemMetrics(SM_CXVSCROLL);
        m_CallListCtrl.SetColumnWidth(COL_STATUS, colWidth);

		TA_Base_Bus::FixedHeaderCtrl::expandColumnToFitListWidth(m_CallListCtrl,5,true);

        //
        // Set the background colour
        //
        m_CallListCtrl.SetBkColor(BACKGROUND_COLOUR);

        m_fixedHeaderHelper.subclassHeader(m_CallListCtrl.GetHeaderCtrl());

        //
        // Set the push-button colour
        //
	    setButtonPushColour(BANNER_CONST::CCL_GOLD);

        //
        // Start the thread that initialises the call banner.
        //
        m_callBannerInitialiser = new CallBannerInitialiser(*this);
        m_callBannerInitialiser->start();
        
        // Make the call list sort using our custom sort function
        m_CallListCtrl.initialise(COL_TIME, this);

        // Set the initial state of the vetting checkbox
        m_vettingManager = new VettingManager(*this, m_locationKey);
        bool isVetting = m_vettingManager->getVettingState();
        m_vettingBtn.SetCheck(isVetting ? 1 : 0);

		
        // 4669Pre-FAT TD#13762
        m_isPECFlashing = false;

	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }


	void CallPage::OnDestroy()
    {    
        TA_Base_Core::RunParams::getInstance().deregisterRunParamUser(this);
    }


	LRESULT CallPage::OnDisableCallPage(WPARAM wParam, LPARAM lParam)
	{
		try
		{
			if (m_vettingManager != NULL)
			{
				delete m_vettingManager;
				m_vettingManager = NULL;
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "m_vettingManager disabled");
			}
			if (m_callEntryManager != NULL)
			{
				delete m_callEntryManager;
				m_callEntryManager = NULL;
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "m_callEntryManager disabled");

			}
			
			TA_Base_Core::RunParams::getInstance().deregisterRunParamUser(this);
			
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "catch exception when trying to remove call page after intialization failure");

		}
		return 0;
	}
    /** 
      * CallPage::OnFullRefreshRequest
      *
      * This is called when the initialise method posts the CALL_PAGE_INITIALISED message.
      * It does the GUI initialisation that is dependent on the banner initialisation.
      * This must be done this way, because if the GUI elements are modified in a non-gui
      * thread it doesnt work.
      *
      * 4669FAT TD #454: This is called upon activation and on session ID change, to
      *     reflect current rights set up
      */
    LRESULT CallPage::OnFullRefreshRequest(WPARAM wParam, LPARAM lParam)
    {
        try
        {
            if (1 == wParam)
            {
                // Refresh rights settings
                initialiseCallTypeFilterInfo();        
            }
    
            // set up the checked state of the buttons
            initialiseButtonStates();
    
            // populate the list
            if (0 != m_callEntryManager)
            {
                refreshCalls();
            }
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(
                SourceInfo, 
                "Unknown Exception",
                "CallPage::OnFullRefreshRequest");
        }
        return TRUE;
    }


    void CallPage::initialiseButtonStates()
    {
        //
        // For each filter button
        //
        for(unsigned long i = 0; i < m_callTypeFilterInfo.size(); i ++)
        {
            UINT btnId = m_callTypeFilterInfo[i].buttonId;
            ToggleButton* pBtn = static_cast<ToggleButton*>( GetDlgItem(btnId) );

            //
            // Enable it if the client has rights for that call type,
            // and set the check status.
            //

			LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
				"Button Name : %s : has a right : %d",m_callTypeFilterInfo[i].buttonName.c_str(),m_callTypeFilterInfo[i].hasRights);

            if( m_callTypeFilterInfo[i].hasRights )
            {
                pBtn->EnableWindow(true);
                pBtn->setCheck( m_callTypeFilterInfo[i].isVisible );
            }
            else
            {
                pBtn->EnableWindow(false);
                pBtn->setCheck(false);
            }
        }
    }


    bool CallPage::isInitialised()
    {
        return m_initialised;
    }


    void CallPage::onButtonDoubleClick()
    {
        //
        // This method is required when inheriting from BannerPage.
        // The call page does not respond to it, however.
        //
    }


    void CallPage::addBannerPage()
    {
        FUNCTION_ENTRY("addBannerPage");

        try
        {
            m_locationKey = getLocationKey();
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Error occurred while attempting to fetch location key");
        }

        TA_Base_Bus::BannerIcon bannerIcon;
        bannerIcon.onIcon = IDB_CALL;
        bannerIcon.offIcon = IDB_CALLG;
	    bannerIcon.onDownIcon = IDB_CALLD;
	    bannerIcon.offDownIcon = IDB_CALLDG;

        getParentDialog()->AddBannerPage(IDD_CALLS, getButtonName(), bannerIcon, this);

        FUNCTION_EXIT;
    }


    void CallPage::onButtonClicked(UINT btnId)
    {
        //
        // If the banner has not been initialised, ignore the button click
        //
        if( !m_initialised )
        {
            return;
        }

        //
        // Toggle the button (ie change its checked state to the opposite)
        //
	    ToggleButton* pBtn = static_cast<ToggleButton*>( GetDlgItem(btnId) );
	    
        pBtn->setCheck( !pBtn->isChecked() );


        //
        // Toggle the visibility of the call type
        //
        for(unsigned long i = 0; i < m_callTypeFilterInfo.size(); i ++)
        {
            if( m_callTypeFilterInfo[i].buttonId == btnId )
            {
                bool isVisible = m_callTypeFilterInfo[i].isVisible;
                m_callTypeFilterInfo[i].isVisible = pBtn->isChecked();
            }
        }
            
        refreshCalls();
    }


    void CallPage::setButtonPushColour(const COLORREF colour)
    {
	    m_authorBtn.SetCheckedColour(colour);
	    m_rxhBtn.SetCheckedColour(colour);
	    m_radioBtn.SetCheckedColour(colour);
	    m_pecBtn.SetCheckedColour(colour);
    }


    void CallPage::OnDblclkCallList(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        //
        // If the banner has not been initialised, ignore the double click
        //
        if( !m_initialised )
        {
            return;
        }


        //
        // Find the item that was double-clicked
        //
        POSITION pos = m_CallListCtrl.GetFirstSelectedItemPosition();
        if(pos == NULL)
        {
            return;
        }

        int index = m_CallListCtrl.GetNextSelectedItem(pos);
        if(index == -1)
        {
            return;
        }

        try
        {
            //
            // The data associated with the item that was double-clicked is
            // the ICallEntry's id. Get the call entry and call its onActivate
            // method.
            //
            unsigned long entryId = m_CallListCtrl.GetItemData(index);

            TA_IRS_Bus::ICallEntry& callEntry =
                m_callEntryManager->getCallEntry(entryId);
			
			if (!callEntry.isCallInvalid())
			{
				LOG_GENERIC (SourceInfo,
							 TA_Base_Core::DebugUtil::DebugInfo,
							 "Valid Call, Can Answer");
				callEntry.onActivate();
			}
            
        }
        catch(const TA_IRS_Bus::CallEntryDoesNotExistException&)
        {
            //
            // Ignore CallEntryDoesNotExistExceptions. Presumably they
            // would only ever be raised if something weird happens with
            // the double-click.
            //
        }
    }


    void CallPage::addCall(const TA_IRS_Bus::ICallEntry& call)
    {        
        //
        // If the dialog has not yet been initialised, ignore the add call
        // notification. The call will be retrieved when the dialog has
        // finished initialising and refreshCalls is called.
        //
        if( !m_initialised )
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                        "Dialog is not yet initialised. Ignoring add request.");
            return;
        }
		
        SendMessage(CALL_PAGE_ADD_CALL, call.getId(), 0);
    }


    LRESULT CallPage::onAddCall(WPARAM wParam, LPARAM lParam)
    {
        try
        {
            TA_IRS_Bus::ICallEntry& call = m_callEntryManager->getCallEntry(wParam);
    
            std::string message = "Adding " + getCallAsString(call);
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"%s", message.c_str() );

            //
            // If the user does not have access rights to view calls of that type
            // or the call type is not visible, just return immediately.
            //
            for(unsigned int i = 0; i < m_callTypeFilterInfo.size(); i ++)
            {
                if( m_callTypeFilterInfo[i].callType == call.getType() )
                {
                    if( m_callTypeFilterInfo[i].hasRights &&
                        m_callTypeFilterInfo[i].isVisible   )
                    {
                        addCallToListControl(call);
                    }                          
                    
                    //
                    // Refresh the text of the associated button so that the number of calls of
                    // that type is kept up to date.
                    //
                    refreshButtonText( call.getType() );      
                }
            }

            // 4669Pre-FAT TD#13762
            refreshPECButtonFlashingState();
            // TES #444: If the filter is hiding items that we have a right to view, the
            // button should still be flashing         
            refreshButtonFlashingState();
        }
        catch(const TA_IRS_Bus::CallEntryDoesNotExistException&)
        {
            // Call no longer exists                    
        }
		catch ( ... )
		{
			// limin: ignore all exception
		}

        return 0;
    }

	LRESULT CallPage::onActivateCall(WPARAM wParam, LPARAM lParam)
    {
        try
        {
            //
            // Find the item with the correct id.
            //
            LVFINDINFO findInfo;
            findInfo.flags       = LVFI_PARAM;
            findInfo.lParam      = lParam;

            int index = m_CallListCtrl.FindItem(&findInfo);

            //
            // If the item is in the list, update it.
            //
            if(index != -1)
            {
                ICallEntry& call = m_callEntryManager->getCallEntry(lParam);
                call.onActivate();
            }
        }
        catch(const TA_IRS_Bus::CallEntryDoesNotExistException&)
        {
            // Call no longer exists
        }
        return 0;
    }

    void CallPage::refreshButtonFlashingState()
    {
        unsigned int totalCalls = 0;

        for(unsigned long i = 0; i < m_callTypeFilterInfo.size(); i ++)
        {
            std::string callType = m_callTypeFilterInfo[i].callType;
            
            if (m_callTypeFilterInfo[i].hasRights)
            {
                unsigned int numCalls =
                    m_callEntryManager->getNumberOfCallEntriesOfType(callType);
                totalCalls += numCalls;
            }
        }

        if (totalCalls > 0)
        {
            enableButtonFlash();
        }
        else
        {
            disableButtonFlash();
        }
    }


    void CallPage::addCallToListControl(const TA_IRS_Bus::ICallEntry& call)
    {
        m_CallListCtrl.SetRedraw(FALSE);

        //
        // Add the item to the beginning of the list control,
        // and associate the call id with it.
        //
        int itemIndex = m_CallListCtrl.InsertItem(0, call.getType().c_str());

        m_CallListCtrl.SetItemData(itemIndex, call.getId());
        
        DWORD fgColour = DEFAULT_TEXT_COLOUR;
        DWORD bgColour = BACKGROUND_COLOUR;
        
        // Apply foreground colour based on item specific defaults
        for(unsigned int i = 0; i < m_callTypeFilterInfo.size(); i ++)
        {
            if(m_callTypeFilterInfo[i].callType == call.getType())
            {
                fgColour = m_callTypeFilterInfo[i].colour;
            }
        }

        // Propweb #3240, highlight emergency and PEC calls in red w/black bg
        if (isEmergencyCall(call))
        {
            fgColour = EMERGENCY_ITEM_FOREGROUND_COLOUR;
            bgColour = EMERGENCY_ITEM_BACKGROUND_COLOUR;
        }
		
 		m_CallListCtrl.setItemColour(call.getId(),
                                     fgColour,
									 TA_Base_Bus::ListCtrlSelNoFocus::I_ITEMDATA,
                                     TA_Base_Bus::ListCtrlSelNoFocus::CT_FOREGROUND);

        m_CallListCtrl.setItemColour(call.getId(),
                                        bgColour,
                                        TA_Base_Bus::ListCtrlSelNoFocus::I_ITEMDATA,
                                        TA_Base_Bus::ListCtrlSelNoFocus::CT_BACKGROUND);
        
       
		m_CallListCtrl.SetRedraw(TRUE);
		
        //
        // Set the item's type details
        //
        refreshItemDetails(itemIndex, call);
    }


    void CallPage::removeCall(unsigned long callId)
    {
        LOG_GENERIC(
            SourceInfo, DebugUtil::DebugInfo,
            "Removing call with id '%d'", callId);

        //
        // If the dialog has not yet been initialised, ignore the remove call
        // notification. The calls will be retrieved when the dialog has
        // finished initialising and refreshCalls is called.
        //
        if( !m_initialised )
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                        "Dialog is not yet initialised. Ignoring remove "
                        "request.");
            return;
        }


		SendMessage(CALL_PAGE_REMOVE_CALL, 0, callId);
    }


    void CallPage::refreshCalls()
    {
        PostMessage(CALL_PAGE_REFRESH_CALLS, 0, 0);
    }


    void CallPage::refreshCall(const TA_IRS_Bus::ICallEntry& call)
    {
		std::string message      = "Refreshing " + getCallAsString(call);
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, message.c_str());

		//
		// If the dialog has not yet been initialised, ignore the refresh call
		// notification. The call will be retrieved when the dialog has
		// finished initialising and refreshCalls is called.
		//
		if( !m_initialised )
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
				"Dialog is not yet initialised. Ignoring refresh call "
				"request.");
			return;
		}

		PostMessage(CALL_PAGE_REFRESH_CALL, 0, call.getId());
    }

	void CallPage::activateCall(const TA_IRS_Bus::ICallEntry& call)
    {
	
        std::string message      = "Activating " + getCallAsString(call);
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "%s",message.c_str());

        //
        // If the dialog has not yet been initialised, ignore the refresh call
        // notification. The call will be retrieved when the dialog has
        // finished initialising and refreshCalls is called.
        //
        if( !m_initialised )
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                        "Dialog is not yet initialised. Ignoring activate call "
                        "request.");
            return;
        }

        PostMessage(CALL_PAGE_ACTIVATE_CALL, 0, call.getId());
		
    }

    void CallPage::refreshItemDetails(int index, const TA_IRS_Bus::ICallEntry& call)
    {         
	    m_CallListCtrl.SetItem(
            index, COL_TIME,    LVIF_TEXT, call.getTimeStr().c_str(), 0, 0, 0, 0 );
        m_CallListCtrl.SetItem(
            index, COL_TYPE,    LVIF_TEXT, call.getType().c_str(), 0, 0, 0, 0 );
        m_CallListCtrl.SetItem(
            index, COL_SOURCE,  LVIF_TEXT, call.getSource().c_str(), 0, 0, 0, 0 );
        m_CallListCtrl.SetItem(
            index, COL_LOC,     LVIF_TEXT, call.getLocationStr().c_str(), 0, 0, 0, 0 );
        m_CallListCtrl.SetItem(
            index, COL_INFO,    LVIF_TEXT, call.getInformation().c_str(), 0, 0, 0, 0 );
        m_CallListCtrl.SetItem(
            index, COL_STATUS,  LVIF_TEXT, call.getStatusStr().c_str(), 0, 0, 0, 0 );
	
    }


    void CallPage::refreshAllButtonsText()
    {
        //
        // Set each button's text including the number of call entries of that
        // type.
        //
		
		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"refreshAllButtonText");

        for(unsigned long i = 0; i < m_callTypeFilterInfo.size(); i ++)
        {
            std::string callType = m_callTypeFilterInfo[i].callType;
            unsigned int numCalls =
                m_callEntryManager->getNumberOfCallEntriesOfType(callType);

            std::ostringstream text;
            text << m_callTypeFilterInfo[i].buttonName << " " << numCalls;

            UINT buttonId = m_callTypeFilterInfo[i].buttonId;

			LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
						 "refreshing Button Id : %d , with the test status : %s",
						 buttonId,text.str().c_str());

            GetDlgItem(buttonId)->SetWindowText( text.str().c_str() );
        }
    }


    void CallPage::refreshButtonText(const std::string& callType)
    {
        //
        // If the banner has not been initialised, ignore the refresh call
        //
        if( !m_initialised )
        {
            return;
        }
		
		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
					 "refreshButtonText : callType : %s",callType.c_str());

        //
        // Find the filter button corresponding to the given call type
        // and set its text including the number of call entries of that type.
        //
        for(unsigned long i = 0; i < m_callTypeFilterInfo.size(); i ++)
        {
            if( m_callTypeFilterInfo[i].callType == callType )
            {
                unsigned int numCalls =
                    m_callEntryManager->getNumberOfCallEntriesOfType(callType);

                std::ostringstream text;
                text << m_callTypeFilterInfo[i].buttonName << " " << numCalls;
				
                UINT buttonId = m_callTypeFilterInfo[i].buttonId;
				
				LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
							 "changing button name : %s @ type %s",m_callTypeFilterInfo[i].buttonName.c_str(),callType.c_str());


                GetDlgItem(buttonId)->SetWindowText( text.str().c_str() );
            }
        }
    }


    std::string CallPage::getCallAsString(const TA_IRS_Bus::ICallEntry& call)
    {
        std::ostringstream asStream;
        asStream << call.getType() << " call (" << call.getId() << ") from "
                 << call.getSource() << " with information '"
                 << call.getInformation() << "'";

        return asStream.str();
    }


	void CallPage::OnSetActive()
    {
        // 4669FAT TD #459 - depress all filtering buttons upon activation of the page
        // (if click 'Calls' button, and already on calls page, we will not get in here
        // and subsequently reset the button states)
        for(unsigned long i = 0; i < m_callTypeFilterInfo.size(); i ++)
        {
            if( m_callTypeFilterInfo[i].hasRights )
            {
                // Cycle through an apply each filter provided we have the 
                // appropriate rights
                m_callTypeFilterInfo[i].isVisible = true;
            }
        }

        PostMessage(CALL_PAGE_INITIALISED, 0, 0);

        TStackedPage::OnSetActive();
    }


    void CallPage::onRunParamChange(const std::string& name, const std::string& value)
    {        
        if (RPARAM_SESSIONID == name)
        {
            // 4669FAT TD #454 - buttons should be disabled when no rights present
            // (so if rights change due to operator profile change, must reflect changes here)
            // The session ID has changed, rights may have changed (so need update)
            PostMessage(CALL_PAGE_INITIALISED, 1, 0);
        }
    }


    bool CallPage::isEmergencyCall(const TA_IRS_Bus::ICallEntry& call)
    {
        if ("Radio" == call.getType())
        {
            // Check if this radio call has the isEmergency state
            const TA_IRS_Bus::RadioCallEntry& radioCall = dynamic_cast<const TA_IRS_Bus::RadioCallEntry&>(call);
            return radioCall.isEmergency();
        }        
        else if ("PEC" == call.getType())
		{
            // peter.wong, PEC Call will always be in emergency mode, regardless, still check the entry
			const TA_IRS_Bus::PECCallEntry& pecCall = dynamic_cast<const TA_IRS_Bus::PECCallEntry&>(call);
            return pecCall.isEmergency();
		} else
        {
            return false;
        }
    }

    
    int CallPage::sortCallback(LPARAM lParam1, LPARAM lParam2, int columnIndex, TA_Base_Bus::AutoSortListCtrl::ECurrentSort currentSort)
    {
        // Always sort in descending order of create time (so newest items are at head of list)        
        try
        {
            // getCallEntry updates asynchronously, must expect CallEntryDoesNotExistException exceptions
            TA_IRS_Bus::AbstractCallEntry* callEntry1 =
                dynamic_cast<AbstractCallEntry*>(&m_callEntryManager->getCallEntry(lParam1));
            TA_IRS_Bus::AbstractCallEntry* callEntry2 =
                dynamic_cast<AbstractCallEntry*>(&m_callEntryManager->getCallEntry(lParam2));
            
            if (0 != callEntry1 && 0 != callEntry2)
            {
                if (callEntry1->getCreateTime() > callEntry2->getCreateTime())
                {
                    // If call 1 was created later (more recent) then it
                    // belongs at top of list (no change)
                    return -1;
                }
                else if (callEntry1->getCreateTime() < callEntry2->getCreateTime())
                {
                    // Otherwise swap positions
                    return 1;
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                TA_ASSERT(FALSE, "CallPage::sortCallback: Invalid input arguments");
                return 0;
            }
        }
        catch(const TA_IRS_Bus::CallEntryDoesNotExistException&)
        {
            // Call no longer exists (can update asynchronously during a re-sort operation)
            return 0;
        }    
    }


    void CallPage::vettingStateChanged(bool isVetting)
    {
        m_vettingBtn.SetCheck(isVetting ? 1 : 0);
    }


  	void CallPage::onVettingCheckClicked()
    {
        bool isVetting = (m_vettingBtn.GetCheck() == BST_CHECKED);
        m_vettingManager->setVettingState(!isVetting);
    }
	
	void CallPage::OnTimer(UINT nIDEvent) 
	{

		static bool flashToggle = true;

		switch ( nIDEvent )
		{
		case PEC_FLASH_TIMER_ID:
			{                
				if (flashToggle) 
				{
					// red color
					m_pecBtn.SetBackgroundColour(BANNER_CONST::CCL_RED);
				}
				else
				{
					// gold color, the default
					if ( m_pecBtn.isChecked() )  //TD16152
					{
						m_pecBtn.SetBackgroundColour(BANNER_CONST::CCL_GOLD); //TD16152
					}
					else
					{
						m_pecBtn.SetBackgroundColour(RGB(192,192,192)); //TD16152
					}
				}

				flashToggle = !flashToggle;
			}
			break;

		default:
			break;
		}

		TStackedPage::OnTimer(nIDEvent);
	}

	LRESULT CallPage::onRemoveCall(WPARAM wParam, LPARAM lParam)
	{

		//
		// Find the item with the correct id.
		//
		LVFINDINFO findInfo;
		findInfo.flags       = LVFI_PARAM;
		findInfo.lParam      = lParam;

		int index = m_CallListCtrl.FindItem(&findInfo);

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
			"Removing call with index '%d'", index);

		//
		// If the item is in the list, delete it.
		// (It won't be in the list if the user doesn't have access control
		//  or that call type, or if the call type is filtered out)
		//
		if(index != -1)
		{            
			m_CallListCtrl.DeleteColourAndItem(index);
		}


		//
		// Refresh the text of the buttons so that the number of calls of
		// that type is kept up to date.
		//
		refreshAllButtonsText();

		refreshButtonFlashingState();

		refreshPECButtonFlashingState ();

		return 0;
	}

	LRESULT CallPage::onRefreshCall(WPARAM wParam, LPARAM lParam)
	{
		try
		{
			//
			// Find the item with the correct id.
			//
			LVFINDINFO findInfo;
			findInfo.flags       = LVFI_PARAM;
			findInfo.lParam      = lParam;

			int index = m_CallListCtrl.FindItem(&findInfo);

			//
			// If the item is in the list, update it.
			//
			if(index != -1)
			{
				ICallEntry& call = m_callEntryManager->getCallEntry(lParam);
				refreshItemDetails(index, call);
			}
		}
		catch(const TA_IRS_Bus::CallEntryDoesNotExistException&)
		{
			// Call no longer exists
		}

		return 0;
	}

	LRESULT CallPage::onRefreshCalls(WPARAM wParam, LPARAM lParam)
	{
		TA_ASSERT(0 != m_callEntryManager, "Call entry manager null");

		//
		// Delete all items from the list.
		//
		m_CallListCtrl.DeleteAllItems();
		m_CallListCtrl.clearItemColourSettings();        

		//
		// For each call type, if it isn't filtered out, and the user has
		// access rights to see it, add it to the list of types to retrieve
		// from the CallEntryManager
		//
		std::vector<std::string> callTypes;

		for(unsigned int i = 0; i < m_callTypeFilterInfo.size(); i ++)
		{
			if( m_callTypeFilterInfo[i].hasRights )
			{
				// TD #444: Need to determine ho wman y
				//if (m_callTypeFilterInfo[i].isVisible)
				{
					callTypes.push_back(m_callTypeFilterInfo[i].callType);
				}                
			}
		}


		//
		// Get the call entries and put them into the list control.
		//
		std::vector<TA_IRS_Bus::ICallEntry*> entries =
			m_callEntryManager->getCallEntriesOfType(callTypes);

		for(unsigned int j = 0; j < entries.size(); j ++)
		{
			addCall( *entries[j] );
		}    
	}


} // TA_IRS_App


