// TimeTableManagerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "app/bas/TimeTableManager/src/TimeTableManager.h"
#include "app/bas/TimeTableManager/src/OCCTimeTableManagerDlg.h"
#include "app/bas/TimeTableManager/src/TimeTableDatapointPairManager.h"
#include "app/bas/TimeTableManager/src/AgentAccessFactory.h"
#include "bus/bas/bas_agent_library/src/BasAgentLibraryException.h"
#include "bus/bas/bas_agent_library/src/BasAgentProxyFactory.h"
#include "bus/bas/bas_agent_library/src/BasAgentProxy.h"
#include "core/data_access_interface/bas/src/BASTimeTableTimeTableAccessFactory.h"
#include "core/exceptions/idl/src/CommonExceptionsCorbaDef.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace TA_IRS_App
{
	// OCCTimeTableManagerDlg dialog
	IMPLEMENT_DYNAMIC(OCCTimeTableManagerDlg, TimeTableManagerBaseDialog)
	OCCTimeTableManagerDlg::OCCTimeTableManagerDlg(CWnd* pParent /*=NULL*/)
		: TimeTableManagerBaseDialog(OCCTimeTableManagerDlg::IDD, pParent)
	{
		m_timeTableListCtrl.setLocationFilter(this);
		m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

		m_bIsFilterModeTimes = false;
		m_bIsLoadModeTimes = false;
	}

	void OCCTimeTableManagerDlg::DoDataExchange(CDataExchange* pDX)
	{
		TimeTableManagerBaseDialog::DoDataExchange(pDX);
		DDX_Control(pDX, IDC_LIST_OCC_TIMETABLE, m_timeTableListCtrl);
        DDX_Control(pDX, IDC_COMBO_STATIONS, m_allLocations);
		DDX_Control(pDX, IDC_COMBO_CURRENT_SCHEDULE, m_timeTableTodayName);
        DDX_Control(pDX, IDC_COMBO_TOMORROW_SCHEDULE, m_timeTableTommorowName);
        DDX_Radio(pDX, IDC_RADIO_WORKDAYS, m_nType);
	}

	BEGIN_MESSAGE_MAP(OCCTimeTableManagerDlg, TimeTableManagerBaseDialog)
		ON_WM_PAINT()
		ON_WM_SIZE()
		ON_WM_QUERYDRAGICON()
		ON_MESSAGE(Msg_InitializeDialogData, InitializeDialogData)
		ON_BN_CLICKED(IDC_BUTTON_RECOVER_TIMETABLE, OnButtonRecoverTimeTableClick)
		ON_BN_CLICKED(IDC_BUTTON_LOCAL_STORE, OnButtonLocalStoreClick)
		ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnButtonCancelClick)
        ON_CBN_SETFOCUS(IDC_COMBO_STATIONS, OnCbnComboStationsFocus)
		ON_CBN_SELCHANGE(IDC_COMBO_STATIONS, OnCbnSelchangeComboStations)
		ON_BN_CLICKED(IDC_RADIO_WORKDAYS, OnBnClickedRadioWorkdays)
		ON_BN_CLICKED(IDC_RADIO_SATURDAY, OnBnClickedRadioSaturday)
		ON_BN_CLICKED(IDC_RADIO_FRIDAY, OnBnClickedRadioFriday)
		ON_BN_CLICKED(IDC_RADIO_SUNDAY, OnBnClickedRadioSunday)
		ON_BN_CLICKED(IDC_RADIO_HOLIDAY, OnBnClickedRadioHoliday)
		ON_BN_CLICKED(IDC_RADIO_SPECIAL_1, OnBnClickedRadioSpecial1)
		ON_BN_CLICKED(IDC_RADIO_SPECIAL_2, OnBnClickedRadioSpecial2)
		ON_BN_CLICKED(IDC_RADIO_SPECIAL_3, OnBnClickedRadioSpecial3)
		ON_BN_CLICKED(IDC_RADIO_SPECIAL_4, OnBnClickedRadioSpecial4)
		ON_BN_CLICKED(IDC_RADIO_SPECIAL_5, OnBnClickedRadioSpecial5)
		ON_BN_CLICKED(IDC_RADIO_CURRENT_SCHEDULE, OnBnClickedRadioCurrentSchedule)
		ON_BN_CLICKED(IDC_RADIO_TOMORROW_SCHEDULE, OnBnClickedRadioTomorrowSchedule)
		ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_TODAY, OnBnClickedSendTodaySchedule)
		ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_TOMORROW, OnBnClickedSendNextDaySchedule)
		//}}AFX_MSG_MAP	
	END_MESSAGE_MAP()


	// OCCTimeTableManagerDlg message handlers
	BOOL OCCTimeTableManagerDlg::OnInitDialog()
	{
		TimeTableManagerBaseDialog::OnInitDialog();

		// Set the icon for this dialog.  The framework does this automatically
		//  when the application's main window is not a dialog
		SetIcon(m_hIcon, TRUE);			// Set big icon
		SetIcon(m_hIcon, FALSE);		// Set small icon

		// TODO: Add extra initialization here
		m_OCCTimeTableImageList.Create(1,18,ILC_COLOR24,0,0);
		m_timeTableListCtrl.SetImageList(&m_OCCTimeTableImageList, LVSIL_SMALL);
		
		m_timeTableListCtrl.setupList();
		m_timeTableListCtrl.resizeHeaderCtrl();

		//create the initialize thread
		m_initializeThread = new TimeTableManagerActionThread(this);
		m_initializeThread->setAction(TimeTableManagerAction_Initialize);
		m_initializeThread->start();

		setWindowCaption(false);
		// start the timer

        m_nType = TTT_Unknown;
        UpdateData( FALSE );

		return TRUE;  // return TRUE  unless you set the focus to a control
	}

	void OCCTimeTableManagerDlg::init()
	{
		FUNCTION_ENTRY("init");
		FUNCTION_EXIT;
	}

	CString OCCTimeTableManagerDlg::getApplicationName()
	{
		FUNCTION_ENTRY("getApplicationName");
		FUNCTION_EXIT;
		return "中央时间表控制器";
	}

	unsigned long OCCTimeTableManagerDlg::getLocationKey()
	{
		FUNCTION_ENTRY("getLocationKey");	
		FUNCTION_EXIT;
		DWORD_PTR locationKey = m_allLocations.GetItemData(m_allLocations.GetCurSel());
		return static_cast<unsigned long>(locationKey);
	}

	// If you add a minimize button to your dialog, you will need the code below
	//  to draw the icon.  For MFC applications using the document/view model,
	//  this is automatically done for you by the framework.

	void OCCTimeTableManagerDlg::OnPaint()
	{
		if (IsIconic())
		{
			CPaintDC dc(this); // device context for painting

			SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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

	// The system calls this function to obtain the cursor to display while the user drags
	//  the minimized window.
	HCURSOR OCCTimeTableManagerDlg::OnQueryDragIcon()
	{
		return static_cast<HCURSOR>(m_hIcon);
	}

	TA_Base_Bus::ResizingProperties OCCTimeTableManagerDlg::getResizingProperties()
	{
		FUNCTION_ENTRY("getResizingProperties");
		TA_Base_Bus::ResizingProperties rp;
		rp.canMaximise = true;
		rp.maxWidth = gMFT_Screen_Width;
		rp.maxHeight = gMFT_Screen_Height;

		// The minimum allowed dimensions are initial (startup) dimensions
		CRect rc;
		GetClientRect(rc);
		rp.minWidth = rc.Width();
		rp.minHeight = rc.Height();
		FUNCTION_EXIT;
		return rp;
	}

	DlgInitializeSize OCCTimeTableManagerDlg::getDlgInitSize()
	{
		FUNCTION_ENTRY("getDlgInitSize");	
		const int StationInitDlgWidth = gMFT_Screen_Width;
		const int StationInitDlgHeight = gMFT_Screen_Height;
		FUNCTION_EXIT;
		return DlgInitializeSize(StationInitDlgWidth,StationInitDlgHeight);
	}

	void OCCTimeTableManagerDlg::OnSize(UINT nType, int cx, int cy)
	{
		FUNCTION_ENTRY("OnSize");
		If_Not_Valid_Window_Return_Void(this);
		If_Not_Valid_Window_Return_Void(GetDlgItem(IDC_STATIC_SCHEDULE_CONFIG));
		If_Not_Valid_Window_Return_Void(GetDlgItem(IDC_STATIC_CONFIGS));
		If_Not_Valid_Window_Return_Void(GetDlgItem(IDC_LIST_TIMETABLE));
		If_Not_Valid_Window_Return_Void(GetDlgItem(IDC_BUTTON_RECOVER_TIMETABLE));
		If_Not_Valid_Window_Return_Void(GetDlgItem(IDC_BUTTON_LOCAL_STORE));
		If_Not_Valid_Window_Return_Void(GetDlgItem(IDC_BUTTON_CANCEL));
		// get client rect
		CRect clientRect;
		GetClientRect(&clientRect);

		// move the top config static ctrl
		CRect cfgRect;
		((CStatic*)GetDlgItem(IDC_STATIC_SCHEDULE_CONFIG))->GetWindowRect(&cfgRect);
		ScreenToClient(&cfgRect);
		cfgRect.right = clientRect.right - Dlg_Right_Blank_Size;
		((CStatic*)GetDlgItem(IDC_STATIC_SCHEDULE_CONFIG))->MoveWindow(&cfgRect);

		// move the middle configs static ctrl
		CRect cfgsRect;
		((CStatic*)GetDlgItem(IDC_STATIC_CONFIGS))->GetWindowRect(&cfgsRect);
		ScreenToClient(&cfgsRect);
		cfgsRect.right = clientRect.right - Dlg_Right_Blank_Size;
		cfgsRect.bottom = clientRect.bottom - Dlg_Bottom_Blank_Size;
		((CStatic*)GetDlgItem(IDC_STATIC_CONFIGS))->MoveWindow(&cfgsRect);

		// move the three button in the bottom
		int buttonTop = 0;
		//move the recover time table button
		CRect btnRect;
		((CButton*)GetDlgItem(IDC_BUTTON_RECOVER_TIMETABLE))->GetWindowRect(&btnRect);
        buttonTop = clientRect.bottom - 2 * Dlg_Bottom_Blank_Size - btnRect.Height();
		ScreenToClient(&btnRect);
        btnRect.MoveToXY( clientRect.right - 4 * Dlg_Right_Blank_Size - 3 * btnRect.Width(), buttonTop );
		((CButton*)GetDlgItem(IDC_BUTTON_RECOVER_TIMETABLE))->MoveWindow(&btnRect);

		//move the local store button
		((CButton*)GetDlgItem(IDC_BUTTON_LOCAL_STORE))->GetWindowRect(&btnRect);
		ScreenToClient(&btnRect);
        btnRect.MoveToXY( clientRect.right - 3 * Dlg_Right_Blank_Size - 2 * btnRect.Width(), buttonTop );
		((CButton*)GetDlgItem(IDC_BUTTON_LOCAL_STORE))->MoveWindow(&btnRect);

		//move the cancel button
		((CButton*)GetDlgItem(IDC_BUTTON_CANCEL))->GetWindowRect(&btnRect);
		ScreenToClient(&btnRect);
        btnRect.MoveToXY( clientRect.right - 2 * Dlg_Right_Blank_Size - 1 * btnRect.Width(), buttonTop );
		((CButton*)GetDlgItem(IDC_BUTTON_CANCEL))->MoveWindow(&btnRect);

		// move the middle time table ctrl
		CRect listRect;
		((CListCtrl*)GetDlgItem(IDC_LIST_TIMETABLE))->GetWindowRect(&listRect);
		ScreenToClient(&listRect);
		listRect.right = clientRect.right - Dlg_Right_Blank_Size - Dlg_Right_Blank_Size;
		listRect.bottom = buttonTop - Dlg_Bottom_Blank_Size;
		((CListCtrl*)GetDlgItem(IDC_LIST_TIMETABLE))->MoveWindow(&listRect);

		// resize the list ctrl header column width
		m_timeTableListCtrl.resizeHeaderCtrl();
		FUNCTION_EXIT;
	}

	void OCCTimeTableManagerDlg::OnButtonRecoverTimeTableClick()
	{
		FUNCTION_ENTRY("OnButtonRecoverTimeTableClick");
		filterTimeTable( m_nType );
		FUNCTION_EXIT;
    }

    void OCCTimeTableManagerDlg::revertFocus()
    {
        switch ( m_nType )
        {
        case TTT_Workdays:
            GetDlgItem( IDC_RADIO_WORKDAYS )->SetFocus();
            break;
        case TTT_Friday:
            GetDlgItem( IDC_RADIO_FRIDAY )->SetFocus();
            break;
        case TTT_Saturday:
            GetDlgItem( IDC_RADIO_SATURDAY )->SetFocus();
            break;
        case TTT_Sunday:
            GetDlgItem( IDC_RADIO_SUNDAY )->SetFocus();
            break;
        case TTT_Holiday:
            GetDlgItem( IDC_RADIO_HOLIDAY )->SetFocus();
            break;
        case TTT_Special1:
            GetDlgItem( IDC_RADIO_SPECIAL_1 )->SetFocus();
            break;
        case TTT_Special2:
            GetDlgItem( IDC_RADIO_SPECIAL_2 )->SetFocus();
            break;
        case TTT_Special3:
            GetDlgItem( IDC_RADIO_SPECIAL_3 )->SetFocus();
            break;
        case TTT_Special4:
            GetDlgItem( IDC_RADIO_SPECIAL_4 )->SetFocus();
            break;
        case TTT_Special5:
            GetDlgItem( IDC_RADIO_SPECIAL_5 )->SetFocus();
            break;
        case TTT_ServerToday:
            GetDlgItem( IDC_RADIO_CURRENT_SCHEDULE )->SetFocus();
            break;
        case TTT_ServerTomorrow:
            GetDlgItem( IDC_RADIO_TOMORROW_SCHEDULE )->SetFocus();
            break;
        default:
            SetFocus();
            break;
        }
    }

	void OCCTimeTableManagerDlg::OnButtonLocalStoreClick()
	{
        FUNCTION_ENTRY("OnButtonLocalStoreClick");

        updateTimeTable();

		FUNCTION_EXIT;
	}

	void OCCTimeTableManagerDlg::OnButtonCancelClick()
	{
		FUNCTION_ENTRY("OnButtonCancelClick");	
		GetParent()->PostMessage(WM_CLOSE, 0, 0);
		FUNCTION_EXIT;
	}

	void OCCTimeTableManagerDlg::ActionThreadFunction(TimeTableManagerAction action)
	{
		FUNCTION_ENTRY("ActionThreadFunction");
		ApplicationBusyIndicator busyCursor;
		switch (action)
		{
		case TimeTableManagerAction_Initialize:
			InitializeDialogData(0, 0);
			break;
		case TimeTableManagerAction_LoadTimeTable:
			LoadTimeTableFromAgent(0, 0);
			break;
		}
		FUNCTION_EXIT;
    }

    void OCCTimeTableManagerDlg::CancelActionThreadFunction(TimeTableManagerAction action)
    {
        FUNCTION_ENTRY("CancelActionThreadFunction");
        switch(action)
        {
        case TimeTableManagerAction_Initialize:
            break;
        case TimeTableManagerAction_LoadTimeTable:

            break;
        }
        FUNCTION_EXIT;
    }

	LRESULT OCCTimeTableManagerDlg::InitializeDialogData(WPARAM wParam, LPARAM lParam)
	{
		FUNCTION_ENTRY("InitializeDialogData");

		// fill the location data to ComboBox
		std::vector<ILocation_SmartPtr> locations = TimeTableManagerDataHelperInstance::instance()->getAllLocations();
		m_allLocations.Clear();
		std::vector<ILocation_SmartPtr>::iterator it;

		for (it = locations.begin(); it != locations.end(); it++)
		{
			if ((*it)->getLocationType() == TA_Base_Core::ILocation::STATION)
			{
				int i = m_allLocations.InsertString(m_allLocations.GetCount(), (*it)->getDisplayName().c_str());
				m_allLocations.SetItemData(i, (*it)->getKey());
			}
		}
		//select the first item
		if (m_allLocations.GetCount() > 0)
		{
			m_allLocations.SetCurSel(0);
		}

		// fill the time table name to ComboBox
		TimeTableNameMap timetableNames;
		TimeTableManagerDataHelperInstance::instance()->getTimeTableNames(timetableNames);
		for (TimeTableNameMap::iterator it = timetableNames.begin(); it != timetableNames.end(); it++)
		{
			CString strTimeTableName = (*it).second.strChineseName.c_str();
			DWORD pkey = static_cast<DWORD>((*it).first);
			int todayIndex = m_timeTableTodayName.AddString(strTimeTableName);
			m_timeTableTodayName.SetItemData(todayIndex, pkey);
			int tomorrowIndex = m_timeTableTommorowName.AddString(strTimeTableName);
			m_timeTableTommorowName.SetItemData(tomorrowIndex, pkey);
		}
		
		FUNCTION_EXIT;
		return 0;
	}

    void OCCTimeTableManagerDlg::OnCbnComboStationsFocus()
    {
		if (m_bIsFilterModeTimes)
        {
            revertFocus();
            MessageBox("程序正忙，请稍候再试", MessageBoxCaption, MB_OK);
            return;
        }
    }

	void OCCTimeTableManagerDlg::OnCbnSelchangeComboStations()
	{
		// TODO: Add your control notification handler code here
		FUNCTION_ENTRY("OnCbnSelchangeComboStations");

        m_bIsFilterModeTimes = true;
        m_bIsLoadModeTimes = false;

        m_timeTableListCtrl.canEditModeList(true);
		filterTimeTable( m_nType );
		CanEditModeList(TRUE);

		m_bIsFilterModeTimes = false;

		downloadTimetable();

		FUNCTION_EXIT;
    }

    void OCCTimeTableManagerDlg::refreshLocalTimeTable( const int nType )
    {
	    if (m_bIsFilterModeTimes)
        {
            revertFocus();
            MessageBox("程序正忙，请稍候再试", MessageBoxCaption, MB_OK);
            return;
        }

        m_bIsFilterModeTimes = true;
        setWindowCaption(false);

        m_timeTableListCtrl.canEditModeList(true);
        filterTimeTable( nType );
        CanEditModeList(TRUE);

        m_bIsFilterModeTimes = false;

        m_nType = nType;
        UpdateData( FALSE );
    }

    void OCCTimeTableManagerDlg::OnBnClickedRadioFriday()
    {
        refreshLocalTimeTable( TTT_Friday );
    }

	void OCCTimeTableManagerDlg::OnBnClickedRadioWorkdays()
    {
        refreshLocalTimeTable( TTT_Workdays );
    }

    void OCCTimeTableManagerDlg::OnBnClickedRadioSaturday()
    {
        refreshLocalTimeTable( TTT_Saturday );
    }

    void OCCTimeTableManagerDlg::OnBnClickedRadioSunday()
    {
        refreshLocalTimeTable( TTT_Sunday );
    }

    void OCCTimeTableManagerDlg::OnBnClickedRadioHoliday()
    {
        refreshLocalTimeTable( TTT_Holiday );
    }

    void OCCTimeTableManagerDlg::OnBnClickedRadioSpecial1()
    {
        refreshLocalTimeTable( TTT_Special1 );
    }

    void OCCTimeTableManagerDlg::OnBnClickedRadioSpecial2()
    {
        refreshLocalTimeTable( TTT_Special2 );
    }

    void OCCTimeTableManagerDlg::OnBnClickedRadioSpecial3()
    {
        refreshLocalTimeTable( TTT_Special3 );
    }

    void OCCTimeTableManagerDlg::OnBnClickedRadioSpecial4()
    {
        refreshLocalTimeTable( TTT_Special4 );
    }

    void OCCTimeTableManagerDlg::OnBnClickedRadioSpecial5()
    {
        refreshLocalTimeTable( TTT_Special5 );
    }

	void OCCTimeTableManagerDlg::filterTimeTable( const int nType )
	{
		FUNCTION_ENTRY("filterTimeTable");
		unsigned long locationKey = 0u;
		std::string strType = "";
		if (!getSelectLocationAndType(nType, locationKey, strType))
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Current filter Invalid locationKey:%d Type:[%d - %s]", locationKey, nType, strType.c_str());
			return;
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Current filter Is locationKey:%d Type:[%d - %s]", locationKey, nType, strType.c_str());
		}

		m_timeTableListCtrl.DeleteAllItems();
		TimeTableScheduleInfoVct timeTableInfoVct;
		TimeTableManagerDataHelperInstance::instance()->getTimeTableScheduleInfoByLocationAndType(locationKey, strType, timeTableInfoVct);

		// fill with the select items	
		TimeTableScheduleInfoVct::iterator it;
		for (it = timeTableInfoVct.begin(); it != timeTableInfoVct.end(); it++)
		{
            std::string strDisplayName = "";
			std::string strTime = "";
			getTimeTableDisplayNameAndTime((*it), strDisplayName, strTime);
			int item = m_timeTableListCtrl.InsertItem(m_timeTableListCtrl.GetItemCount(), strDisplayName.c_str());
			m_timeTableListCtrl.SetItemText(item, TimeTable_Column_Index_BeginTime, strTime.c_str());
			m_timeTableListCtrl.SetItemData(item, (*it)->m_modeKey);
		}

        //add a blank item for add the new mode
        m_timeTableListCtrl.checkAndAppendInsertionItem();

		FUNCTION_EXIT;
	}

	bool OCCTimeTableManagerDlg::getSelectLocationAndType(const int nType, unsigned long& locationKey, std::string& typeName)
	{
		FUNCTION_ENTRY("getSelectLocationAndType");
		bool success = true;

		locationKey = static_cast<unsigned long>(m_allLocations.GetItemData(m_allLocations.GetCurSel()));

		TimeTableNameMap timetableNames;
		TimeTableManagerDataHelperInstance::instance()->getTimeTableNames(timetableNames);

        switch ( nType )
        {
        case TTT_Workdays:
            typeName = timetableNames[TTT_Workdays].strEnglishName;
            break;
        case TTT_Friday:
            typeName = timetableNames[TTT_Friday].strEnglishName;
            break;
        case TTT_Saturday:
            typeName = timetableNames[TTT_Saturday].strEnglishName;
            break;
        case TTT_Sunday:
            typeName = timetableNames[TTT_Sunday].strEnglishName;
            break;
        case TTT_Holiday:
            typeName = timetableNames[TTT_Holiday].strEnglishName;
            break;
        case TTT_Special1:
            typeName = timetableNames[TTT_Special1].strEnglishName;
            break;
        case TTT_Special2:
            typeName = timetableNames[TTT_Special2].strEnglishName;
            break;
        case TTT_Special3:
            typeName = timetableNames[TTT_Special3].strEnglishName;
            break;
        case TTT_Special4:
            typeName = timetableNames[TTT_Special4].strEnglishName;
            break;
        case TTT_Special5:
            typeName = timetableNames[TTT_Special5].strEnglishName;
            break;
        default:
            typeName = "";
            success = false;
            break;
        }
		
		FUNCTION_EXIT;
		return success;
	}

	void OCCTimeTableManagerDlg::getTimeTableDisplayNameAndTime(const TimeTableScheduleInfo_SmartPtr& scheduleInfo, std::string& strName, std::string& strTime)
	{
		FUNCTION_ENTRY("getTimeTableDisplayNameAndTime");

        strName = m_timeTableListCtrl.formatNameString(scheduleInfo->m_modeName, scheduleInfo->m_modeDescription);
        strTime = m_timeTableListCtrl.formatTimeString(scheduleInfo->m_tmBegin);

		FUNCTION_EXIT;
	}

	void OCCTimeTableManagerDlg::updateTimeTable()
	{
		FUNCTION_ENTRY("updateTimeTable");
		BAS_TRY
		{
			unsigned long locationKey = 0;
			std::string strType = "";
			if (!getSelectLocationAndType(m_nType, locationKey, strType))
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Current filter Invalid locationKey:%d Type:[%d - %s]", locationKey, m_nType, strType.c_str());
				return;
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Current filter Is locationKey:%d Type:[%d - %s]", locationKey, m_nType, strType.c_str());
			}

			//delete all about this location and time table type filter
			TimeTableManagerDataHelperInstance::instance()->deleteTimeTableScheduleInfoByLocationAndType(locationKey, strType);

			//add the new items;
            ScheduleInfoVct scheduleInfos;
			for (int i = 0; i < m_timeTableListCtrl.GetItemCount(); i++)
			{
                // Ignore last item for insertion
                if (m_timeTableListCtrl.isInsertionItem(i))
                {
                    continue;
                }

				ScheduleInfo scheduleInfo;
				scheduleInfo.modeKey = static_cast<unsigned long>(m_timeTableListCtrl.GetItemData(i));
				//scheduleInfo.modeName = m_timeTableListCtrl.GetItemText(i, Occ_TimeTable_Column_Index_mode).GetBuffer();
				scheduleInfo.modeTime = m_timeTableListCtrl.getOriginalModeBeginTime(m_timeTableListCtrl.GetItemText(i, TimeTable_Column_Index_BeginTime).GetBuffer());
				scheduleInfos.push_back(scheduleInfo);
			}

			TimeTableManagerDataHelperInstance::instance()->addTimeTableScheduleInfoByLocationAndType(locationKey, strType, scheduleInfos);

			//reload the data form DB.
			filterTimeTable( m_nType );
		}
		BAS_CATCH;
		if (BAS_WhetherHaveError)
		{
			//MessageBox(BAS_GetExceptionString, MessageBoxCaption, MB_OK+MB_ICONERROR);
		}
		FUNCTION_EXIT;
	}

	void OCCTimeTableManagerDlg::CanEditModeList(BOOL bEditable)
	{
		FUNCTION_ENTRY("CanEditModeList");
		GetDlgItem(IDC_BUTTON_RECOVER_TIMETABLE)->EnableWindow(bEditable);
		GetDlgItem(IDC_BUTTON_LOCAL_STORE)->EnableWindow(bEditable);
		GetDlgItem(IDC_BUTTON_CANCEL)->EnableWindow(bEditable);
		FUNCTION_EXIT;
	}

    void OCCTimeTableManagerDlg::loadServerTimeTable( const int nType )
    {
        FUNCTION_ENTRY("loadServerTimeTable");

        if (( TTT_ServerToday != nType ) && ( TTT_ServerTomorrow != nType ))
        {
            FUNCTION_EXIT;
            return;
        }

		if (m_bIsFilterModeTimes)
        {
            revertFocus();
            MessageBox("程序正忙，请稍候再试", MessageBoxCaption, MB_OK);
            FUNCTION_EXIT;
            return;
        }

		setWindowCaption(true);
		m_timeTableListCtrl.canEditModeList( false );
		m_timeTableListCtrl.DeleteAllItems();
		CanEditModeList(FALSE);
		m_nType = nType;
		UpdateData( FALSE );

		if (m_bIsLoadModeTimes)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "The time tables are loading");
			FUNCTION_EXIT;
			return;
		}

        m_bIsLoadModeTimes = true;
        
        m_loadTimeTableThread = new TimeTableManagerActionThread(this);
        m_loadTimeTableThread->setAction(TimeTableManagerAction_LoadTimeTable);
        m_loadTimeTableThread->start();

        FUNCTION_EXIT;
    }

	void OCCTimeTableManagerDlg::OnBnClickedRadioCurrentSchedule()
    {
		// TODO: Add your control notification handler code here
		FUNCTION_ENTRY("OnBnClickedRadioCurrentSchedule");

        loadServerTimeTable( TTT_ServerToday );

		FUNCTION_EXIT;
	}

	void OCCTimeTableManagerDlg::OnBnClickedRadioTomorrowSchedule()
    {
		// TODO: Add your control notification handler code here
        FUNCTION_ENTRY("OnBnClickedRadioTomorrowSchedule");

        loadServerTimeTable( TTT_ServerTomorrow );

		FUNCTION_EXIT;
	}

	LRESULT OCCTimeTableManagerDlg::LoadTimeTableFromAgent(WPARAM wParam, LPARAM lParam)
	{
		FUNCTION_ENTRY("LoadTimeTableFromAgent");
		CButton* button = (CButton*)GetDlgItem(IDC_RADIO_CURRENT_SCHEDULE);
		bool isLoadCurrentDayTimeTable = BST_CHECKED == button->GetCheck() ? true : false;

		unsigned long locationKey;
		locationKey = static_cast<unsigned long>(m_allLocations.GetItemData(m_allLocations.GetCurSel()));

		BAS_TRY
		{
			m_timeTableListCtrl.loadTimeTableFromAgent(locationKey, isLoadCurrentDayTimeTable);
		}
		BAS_CATCH;
		
		m_bIsLoadModeTimes = false;
		setWindowCaption(false);
		FUNCTION_EXIT;
		return 0;
	}

	void OCCTimeTableManagerDlg::setWindowCaption(bool busy)
	{
		CString strCaption = "中央时间表控制器";
		if (busy)
		{
			strCaption = strCaption + " - 正在下载时间表";
		}
		GetParent()->SetWindowText(strCaption);
	}

	void OCCTimeTableManagerDlg::OnBnClickedSendTodaySchedule()
	{
		FUNCTION_ENTRY("OnBnClickedSendTodaySchedule");
		if (m_timeTableTodayName.GetCurSel() == CB_ERR)
		{
			MessageBox("还没有选择要下发的时间表", MessageBoxCaption, MB_OK + MB_ICONWARNING);
			return;
		}

		unsigned long locationKey = static_cast<unsigned long>(m_allLocations.GetItemData(m_allLocations.GetCurSel()));
		TimeTableType timeTableType = static_cast<TimeTableType>(m_timeTableTodayName.GetItemData(m_timeTableTodayName.GetCurSel()));
		sendTimeTable(locationKey, timeTableType, true);
		FUNCTION_EXIT;
	}
	void OCCTimeTableManagerDlg::OnBnClickedSendNextDaySchedule()
	{
		FUNCTION_ENTRY("OnBnClickedSendNextDaySchedule");
		if (m_timeTableTommorowName.GetCurSel() == CB_ERR)
		{
			MessageBox("还没有选择要下发的时间表", MessageBoxCaption, MB_OK + MB_ICONWARNING);
			return;
		}

		unsigned long locationKey = static_cast<unsigned long>(m_allLocations.GetItemData(m_allLocations.GetCurSel()));
		TimeTableType timeTableType = static_cast<TimeTableType>(m_timeTableTommorowName.GetItemData(m_timeTableTommorowName.GetCurSel()));
		sendTimeTable(locationKey, timeTableType, false);
		FUNCTION_EXIT;
	}

	void OCCTimeTableManagerDlg::sendTimeTable(unsigned long locationKey, TimeTableType timeTableType, bool isTodayTimeTable)
	{
		FUNCTION_ENTRY("sentTimeTable");
		CString strExceptionString = "";
		try
		{
			TimeTableNameMap timetableNames;
			TimeTableManagerDataHelperInstance::instance()->getTimeTableNames(timetableNames);			
			std::string strTimeTableType = timetableNames[timeTableType].strEnglishName;

			unsigned long timeTableKey = TA_IRS_Core::BASTimeTableTimeTableAccessFactoryInstance::instance()->getTimeTableKeyByLocationkeyAndType(locationKey, strTimeTableType);
			std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
			//AgentAccessFactory::getInstance().executeTimeTable(locationKey, timeTableKey, session, isTodayTimeTable);
            TA_IRS_Bus::BasAgentProxyFactoryInstance::instance()->getBasAgentProxyByLocationKey(locationKey)->executeTimeTable(timeTableKey, session, isTodayTimeTable);
		}
        catch(const TA_IRS_Bus::BasNotDeployStationException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
			strExceptionString = "此站点还没有部署，请联系管理员！";
		}
        catch(const TA_IRS_Bus::BasAgentInvalidException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
			strExceptionString = "下发时间表过程中系统异常，无法连接到服务器！";
		}
		catch(const TA_Base_Core::TransactiveException& ex)
		{
			strExceptionString = ex.what();
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
			strExceptionString = "下发时间表过程中系统异常";
		}
        catch(const TA_Base_Core::AccessDeniedException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Access denied", ex.reason.in());
            strExceptionString = "无权限下发时间表";
        }
		catch(const TA_Base_Core::NotAvailableException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "NotAvailableException", ex.reason.in());
			strExceptionString = "下发时间表过程中数据配置异常";
		}
		catch(const std::exception& ex)
		{
			strExceptionString = ex.what();
			LOG_EXCEPTION_CATCH(SourceInfo, "std::exception", ex.what());
			strExceptionString = "下发时间表过程中数据异常";
		}
//  		catch(const TA_IRS_Bus::IStationBasAgentCorbaDefRef::StationModeInProgress& ex) //(compiler file 'msc1.cpp', line 1392) why?
//  		{
//   			strExceptionString = static_cast<const char*>(ex.what);
//   			LOG_EXCEPTION_CATCH(SourceInfo, "TA_IRS_Bus::IStationBasAgentCorbaDefRef::StationWriteDataPointError", strExceptionString.GetBuffer());
//   			strExceptionString = "下发时间表过程中与FEP连接异常";
//  		}
		catch(const CORBA::Exception& ex)
		{
			strExceptionString = static_cast< const char* >(ex._name());
			LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", strExceptionString.GetBuffer());
			strExceptionString = "下发时间表过程中通信异常";
		} 
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknown exception occur.");
			strExceptionString = "下发时间表过程中出现未知异常";
		}

		if(!strExceptionString.IsEmpty())
		{
			MessageBox(strExceptionString, MessageBoxCaption, MB_OK | MB_ICONERROR);
		}
        else
        {
            strExceptionString = isTodayTimeTable ? "当日时间表" : "明日时间表";
            strExceptionString += "下载成功";
            MessageBox(strExceptionString, MessageBoxCaption, MB_OK | MB_ICONINFORMATION);
        }

		FUNCTION_EXIT;
	}

	void OCCTimeTableManagerDlg::downloadTimetable()
	{
        loadServerTimeTable( m_nType );
	}
}

