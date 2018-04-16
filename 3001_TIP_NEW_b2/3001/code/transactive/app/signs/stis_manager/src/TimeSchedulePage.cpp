
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_TIP/4669/transactive/app/signs/stis_manager/src/TimeSchedulePage.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2008/11/21 21:00:21 $
  * Last modified by:  $Author: bjteamleads $
  *
  * This page shows the library versions at each location.
  */

#include "stdafx.h"
#include "TimeSchedulePage.h"
#include "RightsManager.h"
#include "UserMessages.h"
#include "MainTab.h"
#include "RunThreadedFunc.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"
#include <iomanip>
#include <algorithm>
#include "boost/bind.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "MainTab.h"


using TA_Base_Core::DebugUtil;
using TA_Base_Core::TransactiveException;
using TA_Base_Bus::ListCtrlSelNoFocus;

namespace TA_IRS_App
{
	const TA_Base_Core::EDisplayMode defaultDisplayMode = TA_Base_Core::INSTANT_ON_STIS;
	const TA_Base_Core::EScrollSpeed defaultScrollSpeed = TA_Base_Core::SCROLL_MEDIUM;
	const unsigned int defaultDisplayTime = 1;
	const TA_Base_Core::EJustification defaultJustification = TA_Base_Core::CENTRED;
	const TA_Base_Core::ELcdFontColour defaultLcdFontColour = TA_Base_Core::WHITE;
	const TA_Base_Core::ELcdFontSize defaultLcdFontSize = TA_Base_Core::LCD_MEDIUM_FONT;
	const TA_Base_Core::ELEDColour defaultLedColour = TA_Base_Core::LED_RED;
	const TA_Base_Core::ELEDFontSize defaultLedFontSize = TA_Base_Core::FONT_SIZE_MEDIUM;

	const int TimeSchedulePage::MAX_FREE_TEXT_CHARACTERS = 255; 
	const CString TimeSchedulePage::VALID_CHARACTERS = 
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        " !\"#$%&'()*+,-./" //ASCII code 32 to 47
        ":;<=>?\x03\x08\x16\x18"; //ASCII code 58 to 63

    TimeSchedulePage::TimeSchedulePage(CWnd* pParent /*=NULL*/)
        : CDialog(TimeSchedulePage::IDD, pParent)
    {
        FUNCTION_ENTRY( "TimeSchedulePage" );

        // {{AFX_DATA_INIT(TimeSchedulePage)
            // NOTE: the ClassWizard will add member initialization here
        // }}AFX_DATA_INIT

        FUNCTION_EXIT;
    }


    void TimeSchedulePage::DoDataExchange(CDataExchange* pDX)
    {
        FUNCTION_ENTRY( "DoDataExchange" );

        CDialog::DoDataExchange(pDX);
        // {{AFX_DATA_MAP(TimeSchedulePage)
        DDX_Control(pDX, IDC_PRIORITY, m_priorityCombo);
        DDX_Control(pDX, IDC_START_DATE, m_startDate);
        DDX_Control(pDX, IDC_START_TIME, m_startTime);
        DDX_Control(pDX, IDC_END_DATE, m_endDate);
        DDX_Control(pDX, IDC_END_TIME, m_endTime);
		DDX_Control(pDX, IDC_EDIT_REPEAT_INTERVAL, m_repeatInterval);
		DDX_Control(pDX, IDC_SPIN_REPEAT_INTERVAL, m_repeatIntervalSpin);
        DDX_Control(pDX, IDC_DISPLAY_MODE, m_displayMode);
		DDX_Control(pDX, IDC_DISPLAY_TIME, m_displayTime);
		DDX_Control(pDX, IDC_SCROLL_SPEED, m_scrollSpeed);
		DDX_Control(pDX, IDC_JUSTIFICATION, m_justification);
		DDX_Control(pDX, IDC_LCD_FONT_COLOUR, m_lcdFontColour);
		DDX_Control(pDX, IDC_LCD_FONT_SIZE, m_lcdFontSize);
		DDX_Control(pDX, IDC_LED_FONT_COLOUR, m_ledFontColour);
		DDX_Control(pDX, IDC_LED_FONT_SIZE, m_ledFontSize);
		DDX_Control(pDX, IDC_DISPLAY_TIME_LABEL, m_displayTimeLabel);
		DDX_Control(pDX, IDC_EDIT_TIME_SCHEDULE, m_textEdit);
		DDX_Control(pDX, IDC_LIST_TIME_SCHEDULE, m_messageList);
		DDX_Control(pDX, IDC_BTN_SAVE_TIME_SCHEDULE, m_saveBtn);
		DDX_Control(pDX, IDC_BTN_EDIT_TIME_SCHEDULE, m_editBtn);
		DDX_Control(pDX, IDC_BTN_DOWNLOAD_TIME_SCHEDULE, m_downLoadBtn);
		DDX_Control(pDX, IDC_JUSTIFICATION_LABEL, m_justificationLabel);
		DDX_Control(pDX, IDC_SCROLLSPEED_LABEL, m_scrollSpeedLabel);
        // }}AFX_DATA_MAP

        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(TimeSchedulePage, CDialog)
        // {{AFX_MSG_MAP(TimeSchedulePage)
        ON_WM_DESTROY()
        ON_BN_CLICKED(IDC_BTN_SAVE_TIME_SCHEDULE, OnSave)
        ON_BN_CLICKED(IDC_BTN_EDIT_TIME_SCHEDULE, OnEdit)
        ON_BN_CLICKED(IDC_BTN_DOWNLOAD_TIME_SCHEDULE, OnDownLoad)
		ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_TIME_SCHEDULE, OnMessageSelChanged)
		ON_MESSAGE(WM_TIMESCHEDULEDATA_COMPLETE_LOAD, OnTimeScheduleDataCompleteLoad)
		ON_EN_KILLFOCUS(IDC_EDIT_REPEAT_INTERVAL, OnKillFocusRepeatInterval)
    // }}AFX_MSG_MAP
    END_MESSAGE_MAP()

    BOOL TimeSchedulePage::OnInitDialog()
    {
        FUNCTION_ENTRY( "OnInitDialog" );

        CDialog::OnInitDialog();
		m_repeatInterval.setBunndySpinCtrl(&m_repeatIntervalSpin);
		//the edit control should be disabled default
		m_textEdit.EnableWindow(FALSE);
		//set message list ctrl attributes
		m_messageList.SetExtendedStyle(m_messageList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
		m_displayOrder[0] = 1;
		m_displayOrder[1] = 0;
		RECT rect;
		const int imageColumnSize = 30;
		m_messageList.GetClientRect(&rect);
		m_messageList.InsertColumn(0, "", LVCFMT_LEFT, imageColumnSize);
		m_messageList.InsertColumn(1, "", LVCFMT_LEFT, rect.right - imageColumnSize);
		m_messageList.SetColumnOrderArray(2, m_displayOrder);
		//init imagelist
		m_imageList.Create(16, 16, ILC_COLOR32 | ILC_MASK, 2, 1);
		HICON hicon = AfxGetApp()->LoadIcon(IDI_BLANK);
		m_imageList.Add(hicon);
		hicon = AfxGetApp()->LoadIcon(IDI_CHECK);
		m_imageList.Add(hicon);
		m_messageList.SetImageList( &m_imageList, LVSIL_SMALL );

		m_displayAttributesControl = new TSDisplayAttributesManager(m_displayMode, 
			m_displayTimeLabel,
			m_displayTime, 
			m_scrollSpeed,
			m_justification,
			m_lcdFontColour,
			m_lcdFontSize,
			m_ledFontColour,
			m_ledFontSize,
			m_justificationLabel,
			m_scrollSpeedLabel);

		// set time format
		m_startDate.SetFormat(" dd-MMM-yy");
		m_endDate.SetFormat(" dd-MMM-yy");
		m_startTime.SetFormat(" HH:mm:ss");
		m_endTime.SetFormat(" HH:mm:ss");

		m_timeAttributesControl = new TimeScheduleTimeControl(m_priorityCombo,
			m_startDate,
			m_startTime,
			m_endDate,
			m_endTime,
			m_repeatInterval);
        FUNCTION_EXIT;
        return TRUE;  // return TRUE unless you set the focus to a control
                      // EXCEPTION: OCX Property Pages should return FALSE
    }


	void TimeSchedulePage::OnMessageSelChanged(NMHDR* pNMHDR, LRESULT* pResult)
	{
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
		(*pResult) = TRUE;
		if ((pNMListView->uChanged & LVIF_STATE) 
			&& (pNMListView->uNewState & LVNI_SELECTED))
		{
			// do stuff...
	        TA_IRS_Core::StisTimeScheduleData* message = NULL;
	        int selected = -1;
	        POSITION currSel = m_messageList.GetFirstSelectedItemPosition();

	        if( currSel != NULL )
	        {
	            selected = m_messageList.GetNextSelectedItem( currSel );
	        }
			if(selected < 0)
			{				
				return;
			}	        
            message = reinterpret_cast<TA_IRS_Core::StisTimeScheduleData*> ( m_messageList.GetItemData( selected ) );  
			resetAllcontrolValue(message);
			resetItemImage(selected);
			m_textEdit.EnableWindow(FALSE);
			(*pResult) = FALSE;
		}	
	}

	bool TimeSchedulePage::saveAllControlValue(TA_IRS_Core::StisTimeScheduleData* data)
	{
		FUNCTION_ENTRY("saveAllControlValue");
		TA_ASSERT(data != NULL, "The message is not valid");
		bool isSuccess = false;
		// Get all control value and save it
		// Message content
		CString messageContent;
		m_textEdit.GetWindowText(messageContent);
		data->setTextContent(LPCTSTR(messageContent));
		// Priority
		data->setPriority(m_timeAttributesControl->getPriority());
		// Start time and end time
		data->setStartTime(m_timeAttributesControl->convertCTimeToString(m_timeAttributesControl->getStartTime()));
		data->setEndTime(m_timeAttributesControl->convertCTimeToString(m_timeAttributesControl->getEndTime()));
		// Repeatinterval
		data->setRepeatInterval(m_timeAttributesControl->getRepeatInterval());
		// Displayattributes
		data->setDisplayMode(m_displayAttributesControl->getDisplayMode());
		data->setScrollSpeed(m_displayAttributesControl->getScrollSpeed());
		data->setDisplayTime(m_displayAttributesControl->getDisplayTime());
		data->setJustification(m_displayAttributesControl->getJustification());
		// Lcd attributes
		data->setLcdFontColour(m_displayAttributesControl->getLcdFontColour());
		data->setLcdFontSize(m_displayAttributesControl->getLcdFontSize());
		// Led attributes
		data->setLedFontColour(m_displayAttributesControl->getLedFontColour());
		data->setLedFontSize(m_displayAttributesControl->getLedFontSize());
		try
		{
			data->updateExistingStisTimeSchedule();
			isSuccess = true;
		}
		catch(const TA_Base_Core::DatabaseException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
			UserMessages::getInstance().displayErrorOnce(UserMessages::UNABLE_TO_CONNECT_DATABASE, UserMessages::ERROR_CONNECTION_DATABASE);
		}
		catch( ... )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknow exception", "");
		}
		return isSuccess;
	}

	void TimeSchedulePage::setAllDefaultValue()
	{
		FUNCTION_ENTRY("setAllDefaultValue");
		// set priority and repeat interval default value
		m_timeAttributesControl->setDefaultValue();
		// set start time and end time to current time
		CTime currentTime = CTime::GetCurrentTime();
		m_timeAttributesControl->setStartTime(currentTime);
		m_timeAttributesControl->setEndTime(currentTime);
		// set display attribute default attributes
		m_displayAttributesControl->setDisplayMode(defaultDisplayMode);
		m_displayAttributesControl->setScrollSpeed(defaultScrollSpeed);
		m_displayAttributesControl->setDisplayTime(defaultDisplayTime);
		m_displayAttributesControl->setJustification(defaultJustification);
		m_displayAttributesControl->setLcdFontColour(defaultLcdFontColour);
		m_displayAttributesControl->setLcdFontSize(defaultLcdFontSize);
		m_displayAttributesControl->setLedFontColour(defaultLedColour);
		m_displayAttributesControl->setLedFontSize(defaultLedFontSize);

		FUNCTION_EXIT;
	}

	void TimeSchedulePage::resetAllcontrolValue(TA_IRS_Core::StisTimeScheduleData* data)
	{
		FUNCTION_ENTRY("resetAllcontrolValue");
	    TA_ASSERT(data != NULL, "The message is not a valid");
		try
		{
			//set text edit content
			m_textEdit.SetWindowText(data->getTextContent().c_str());

			// if text is empty, set to default
			if(data->isRecordEmtpy())
			{
				setAllDefaultValue();
			}
			else
			{				
				//set priority
				m_timeAttributesControl->setPriority(data->getPriority());
				//set start and end time
				CTime time = m_timeAttributesControl->convertStringToCTime(data->getStartTime());
				m_timeAttributesControl->setStartTime(time);
				time = m_timeAttributesControl->convertStringToCTime(data->getEndTime());
				m_timeAttributesControl->setEndTime(time);
				//set repeat interval
				m_timeAttributesControl->setRepeatInterval(data->getRepeatInterval());
				//set display attributes
				m_displayAttributesControl->setDisplayMode(data->getDisplayMode());
				m_displayAttributesControl->setDisplayTime(data->getDisplayTime());
				m_displayAttributesControl->setScrollSpeed(data->getScrollSpeed());
				m_displayAttributesControl->setJustification(data->getJustification());
				//set lcd attributes
				m_displayAttributesControl->setLcdFontColour(data->getLcdFontColour());
				m_displayAttributesControl->setLcdFontSize(data->getLcdFontSize());
				//set led attributes
				m_displayAttributesControl->setLedFontColour(data->getLedFontColour());
				m_displayAttributesControl->setLedFontSize(data->getLedFontSize());
			}
			
		}
		catch(const TA_Base_Core::DatabaseException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, ex.what(), "");
			UserMessages::getInstance().displayErrorOnce(UserMessages::UNABLE_TO_CONNECT_DATABASE, UserMessages::ERROR_CONNECTION_DATABASE);
		}
		FUNCTION_EXIT;
	}

    void TimeSchedulePage::Init()
    {
        FUNCTION_ENTRY( "Init" );
		m_textEdit.SetLimitText(MAX_FREE_TEXT_CHARACTERS);
		m_textEdit.setAllowedCharacters(VALID_CHARACTERS);
		m_timeAttributesControl->Init();
		m_displayAttributesControl->init();
		setAllDefaultValue();
		runThreadedFunc(boost::bind(&TimeSchedulePage::loadAllMessages, this));
        FUNCTION_EXIT;
    }


	void TimeSchedulePage::OnEdit()
	{
        int selected = -1;
        POSITION currSel = m_messageList.GetFirstSelectedItemPosition();
        if( currSel != NULL )
        {
            selected = m_messageList.GetNextSelectedItem( currSel );
        }
		if( selected < 0 )
		{
			return;
		}
		m_textEdit.EnableWindow(TRUE);
	}

	void TimeSchedulePage::OnSave()
	{
        int selected = -1;
        POSITION currSel = m_messageList.GetFirstSelectedItemPosition();
        if( currSel != NULL )
        {
            selected = m_messageList.GetNextSelectedItem( currSel );
        }
		if( selected < 0 )
		{
			return;
		}
		// check if input valid
		// start time & end time
        CTime tempStartTime = m_timeAttributesControl->getStartTime();
        CTime tempEndTime = m_timeAttributesControl->getEndTime();
        // If the message is not an emergency message (ie continuous)
        // ensure that the start and end time are valid
        if( tempStartTime > tempEndTime )
        {
            UserMessages::getInstance().displayError(UserMessages::ERROR_START_TIME_AFTER_END);
            return;
        }
        else if( tempStartTime == tempEndTime )
        {
            UserMessages::getInstance().displayError(UserMessages::ERROR_START_TIME_EQUALS_END);
            return;
        }

        CTime currentTime = CTime::GetCurrentTime();
        if ( currentTime > tempEndTime )
        {
            UserMessages::getInstance().displayError(UserMessages::ERROR_END_TIME_PASSED);
            return;
        }
        else if (currentTime > tempStartTime) {
            UserMessages::getInstance().displayError(UserMessages::ERROR_START_TIME_BEFORE_CURRENT_TIME);
            return;
        }

		// if repeat interval valid
		unsigned short repeatInterval = m_timeAttributesControl->getRepeatInterval();
		if(m_timeAttributesControl->getValidMinRepeatInterval() > repeatInterval
			|| m_timeAttributesControl->getValidMaxRepeatInterval() < repeatInterval)
		{
            UserMessages::getInstance().displayError(UserMessages::ERROR_REPEAT_INTERVAL_INVALID);
            return;
		}

		TA_IRS_Core::StisTimeScheduleData *data =
			reinterpret_cast<TA_IRS_Core::StisTimeScheduleData*>(m_messageList.GetItemData(selected));
		if(saveAllControlValue(data))
		{
			//invalidate values then reload from database
			invalidateMessageItem(selected);
			//resetAllcontrolValue(data);
			resetItemImage(selected);
			m_textEdit.EnableWindow(FALSE);
		}
	}

	void TimeSchedulePage::resetItemImage(int nItem)
	{

		CString cstr;
		m_textEdit.GetWindowText(cstr);
		LVITEM itm;
		ZeroMemory(&itm, sizeof(LVITEM));
		itm.mask = LVIF_IMAGE | LVIF_TEXT;
		itm.iImage = cstr.IsEmpty() ? 0 : 1;
		itm.iItem = nItem;
		itm.iSubItem = 0;
		itm.pszText = "";
		m_messageList.SetItem(&itm);
	}

	void TimeSchedulePage::OnDownLoad()
	{
		FUNCTION_ENTRY("OnDownLoad");
		if( UserMessages::getInstance().askQuestion(UserMessages::QUESTION_DOWNLOAD_TIME_SCHEDULE) != IDYES )
	    {
	        FUNCTION_EXIT;
	        return;
	    }

        std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
        try
        {
			try
			{
				CORBA_CALL(TISAgentAccessFactory::getInstance().getSTISAgent(),submitTimeScheduleDownloadRequest,(sessionId.c_str()));
			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				CORBA_CALL(TISAgentAccessFactory::getInstance().getSTISAgent(),submitTimeScheduleDownloadRequest,(sessionId.c_str()));
			}
            UserMessages::getInstance().displayInfo(
                str( format(UserMessages::INFO_REQUEST_SUCCESSFUL) % "Time Schedule Download" ).c_str());
        }
		CATCH_ALL_EXCEPTIONS("While submitting Time Schedule Download request");

	}

	void TimeSchedulePage::invalidateMessageItem(unsigned int itemIndex)
	{
		FUNCTION_ENTRY("invalidateMessageItem");
		DWORD_PTR data = m_messageList.GetItemData(itemIndex);
		if(data != NULL)
		{
			TA_IRS_Core::StisTimeScheduleData *scheduleData = reinterpret_cast<TA_IRS_Core::StisTimeScheduleData*>(data);
			try
			{
				scheduleData->invalidate();
			}
			catch(const TA_Base_Core::DatabaseException& ex)
			{
				UserMessages::getInstance().displayErrorOnce(UserMessages::UNABLE_TO_CONNECT_DATABASE, UserMessages::ERROR_CONNECTION_DATABASE);
			}
		}
		FUNCTION_EXIT;
	}

    void TimeSchedulePage::OnDestroy()
    {
        FUNCTION_ENTRY( "OnDestroy" );

		delete m_timeAttributesControl;
		delete m_displayAttributesControl;
		//clear message data map
		for(std::vector<TA_IRS_Core::StisTimeScheduleData*>::iterator ite = m_timeScheduleMessags.begin();
			ite != m_timeScheduleMessags.end();
			++ite)
		{
			delete *ite;
		}
		TA_IRS_Core::StisTimeScheduleAccessFactory::removeInstance();
        CDialog::OnDestroy();
        FUNCTION_EXIT;
    }

	LRESULT TimeSchedulePage::OnTimeScheduleDataCompleteLoad(WPARAM, LPARAM)
	{
        initMessageList();
		return 0;
	}

	void TimeSchedulePage::loadAllMessages()
	{
		FUNCTION_ENTRY("loadAllMessages");

		try
		{
		    m_timeScheduleMessags = TA_IRS_Core::StisTimeScheduleAccessFactory::getInstance().getStisTimeSchedules();
			// Force load every item data
			for(std::vector<TA_IRS_Core::StisTimeScheduleData*>::iterator it = m_timeScheduleMessags.begin();
				it != m_timeScheduleMessags.end(); ++it)
			{
				(*it)->getPriority();
			}
		}
		catch( const TA_Base_Core::DatabaseException& ex)
		{
			UserMessages::getInstance().displayErrorOnce(UserMessages::UNABLE_TO_CONNECT_DATABASE, UserMessages::ERROR_CONNECTION_DATABASE);
		}
		catch( const TA_Base_Core::DataException& ex)
		{

		}
		catch( ... )
		{

		}
		this->PostMessage(WM_TIMESCHEDULEDATA_COMPLETE_LOAD);
		FUNCTION_EXIT;
	}

	void TimeSchedulePage::initMessageList()
	{
		FUNCTION_ENTRY("initMessageList");
		int nItem = -1;
		for(std::vector<TA_IRS_Core::StisTimeScheduleData*>::iterator ite = m_timeScheduleMessags.begin();
			ite != m_timeScheduleMessags.end();
			++ite)
		{
			std::stringstream ss;
			ss << "Message ";
			//if message text content empty used image 0,else use image 1
			try
			{
				nItem = m_messageList.InsertItem(++nItem, "", (*ite)->getTextContent().empty() ? 0 : 1);
				if( -1 != nItem)
				{
					ss << (nItem + 1);
					m_messageList.SetItemText(nItem, 1, ss.str().c_str());
					m_messageList.SetItemData(nItem, reinterpret_cast<DWORD_PTR>(*ite));
				}
			}
			catch (...)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "Unkown exception", "");
			}
		}
		FUNCTION_EXIT;
	}

    void TimeSchedulePage::OnOK()
    {
        FUNCTION_ENTRY( "OnOK" );
        FUNCTION_EXIT;
    }

    void TimeSchedulePage::OnCancel()
    {
        FUNCTION_ENTRY( "OnCancel" );
        FUNCTION_EXIT;
    }

    BOOL TimeSchedulePage::PreTranslateMessage(MSG* pMsg)
    {
		if(WM_KEYDOWN == pMsg->message)
        {
            switch ( pMsg->wParam )
            {
	            case VK_ESCAPE: return TRUE;
				case VK_RETURN: return TRUE;
	            default: break;
            }
        }
        return CDialog::PreTranslateMessage(pMsg);
    }

	void TimeSchedulePage::OnKillFocusRepeatInterval()
	{
		m_repeatInterval.focusKilled();
	}

} // TA_IRS_App


