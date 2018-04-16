// src\StationTimeTableManagerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "app/bas/TimeTableManager/src/TimeTableManager.h"
#include "app/bas/TimeTableManager/src/StationTimeTableManagerDlg.h"
#include "app/bas/TimeTableManager/src/TimeTableManagerDataHelper.h"

namespace TA_IRS_App
{
	// StationTimeTableManagerDlg dialog
	IMPLEMENT_DYNAMIC(StationTimeTableManagerDlg, TimeTableManagerBaseDialog)
	StationTimeTableManagerDlg::StationTimeTableManagerDlg(CWnd* pParent /*=NULL*/) : 
    TimeTableManagerBaseDialog(StationTimeTableManagerDlg::IDD, pParent),
    m_bLoadToday( false ),
    m_bIsLoadModeTimes( false )
	{
		FUNCTION_ENTRY("StationTimeTableManagerDlg");
		FUNCTION_EXIT;
	}

	StationTimeTableManagerDlg::~StationTimeTableManagerDlg()
	{
		FUNCTION_ENTRY("~StationTimeTableManagerDlg");

		FUNCTION_EXIT;
	}

	void StationTimeTableManagerDlg::DoDataExchange(CDataExchange* pDX)
	{
		TimeTableManagerBaseDialog::DoDataExchange(pDX);

        DDX_Control(pDX, IDC_LIST_STATION_TIMETABLE, m_timeTableListCtrl);
        DDX_Control(pDX, IDC_RADIO_STATION_CURRENT_SCHEDULE, m_btnCurrentTimetable);
        DDX_Control(pDX, IDC_RADIO_STATION_TOMORROW_SCHEDULE, m_btnNextTimetable);
	}

	BEGIN_MESSAGE_MAP(StationTimeTableManagerDlg, TimeTableManagerBaseDialog)
		ON_BN_CLICKED(IDC_RADIO_STATION_CURRENT_SCHEDULE, OnButtonTodayScheduleTimeTable)
		ON_BN_CLICKED(IDC_RADIO_STATION_TOMORROW_SCHEDULE, OnButtonTomorrowScheduleTimeTable)
		ON_WM_SIZE()
	END_MESSAGE_MAP()

	// StationTimeTableManagerDlg message handlers
	BOOL StationTimeTableManagerDlg::OnInitDialog()
	{
		TimeTableManagerBaseDialog::OnInitDialog();
		m_timeTableListCtrl.setupList();
		m_timeTableListCtrl.resizeHeaderCtrl();

		//create the initialize thread
		m_initializeThread = new TimeTableManagerActionThread(this);
		m_initializeThread->setAction(TimeTableManagerAction_Initialize);
		m_initializeThread->start();

		setWindowCaption(false);

		return TRUE;
	}

	void StationTimeTableManagerDlg::init()
	{
		
	}

	void StationTimeTableManagerDlg::OnSize(UINT nType, int cx, int cy)
	{
		FUNCTION_ENTRY("OnSize");

        if (::IsWindow( m_timeTableListCtrl.GetSafeHwnd()))
        {
			CRect clientRect;
			GetClientRect(&clientRect);
			CRect rect;
			m_timeTableListCtrl.GetWindowRect(&rect);
			ScreenToClient(&rect);
			rect.right = clientRect.right - Dlg_Right_Blank_Size;
			rect.bottom = clientRect.bottom - Dlg_Bottom_Blank_Size;
			m_timeTableListCtrl.MoveWindow(&rect);

			// resize the list ctrl header column width
			m_timeTableListCtrl.resizeHeaderCtrl();
        }

		FUNCTION_EXIT;
	}

	TA_Base_Bus::ResizingProperties StationTimeTableManagerDlg::getResizingProperties()
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

	DlgInitializeSize StationTimeTableManagerDlg::getDlgInitSize()
	{
		FUNCTION_ENTRY("getDlgInitSize");	
		const int StationInitDlgWidth = gMFT_Screen_Width;
		const int StationInitDlgHeight = gMFT_Screen_Height;
		FUNCTION_EXIT;
		return DlgInitializeSize(StationInitDlgWidth,StationInitDlgHeight);
	}

	CString StationTimeTableManagerDlg::getApplicationName()
	{
		FUNCTION_ENTRY("getApplicationName");
		FUNCTION_EXIT;
		return "时间表管理器";
	}

	void StationTimeTableManagerDlg::ActionThreadFunction(TimeTableManagerAction action)
	{
		FUNCTION_ENTRY("ActionThreadFunction");
		switch(action)
		{
		case TimeTableManagerAction_Initialize:
				InitializeDlgData();
				break;
		case TimeTableManagerAction_LoadTimeTable:
				LoadTimeTableFromAgent(0 , 0);
				break;
		}
		FUNCTION_EXIT;
	}

    void StationTimeTableManagerDlg::CancelActionThreadFunction(TimeTableManagerAction action)
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

	void StationTimeTableManagerDlg::InitializeDlgData()
	{
		FUNCTION_ENTRY("InitializeDlgData");
		BAS_TRY
		{
			ILocation_SmartPtr location = TimeTableManagerDataHelperInstance::instance()->getLocalLocation();
			if (NULL == location.get())
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Cannot get local location information");
				TA_THROW(TA_Base_Core::TransactiveException("Cannot get local location information"));
			}
			GetDlgItem(IDC_STATION_NAME)->SetWindowText(location->getDisplayName().c_str());
		}
		BAS_CATCH;
		if (BAS_WhetherHaveError)
		{
			MessageBox("初始化数据失败，请重新启动，\n如果重新启动仍有问题请联系管理员", MessageBoxCaption, MB_OK + MB_ICONERROR);
		}
		FUNCTION_EXIT;
	}

	LRESULT StationTimeTableManagerDlg::LoadTimeTableFromAgent(WPARAM wParam, LPARAM lParam)
	{
		FUNCTION_ENTRY("LoadTimeTableFromAgent");
        
		ILocation_SmartPtr location = TimeTableManagerDataHelperInstance::instance()->getLocalLocation();
		if (NULL == location.get())
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Cannot get local location information");
		}
		else
		{
			unsigned long locationKey = location->getKey();
			BAS_TRY
			{
				m_timeTableListCtrl.loadTimeTableFromAgent(locationKey, m_bLoadToday);
			}
			BAS_CATCH;
		}

		m_bIsLoadModeTimes = false;
		setWindowCaption(false);
		FUNCTION_EXIT;
		return 0;
	}

	void StationTimeTableManagerDlg::OnButtonTodayScheduleTimeTable()
    {
        if ( m_bLoadToday && m_bIsLoadModeTimes )
        {
            return;
        }

        m_btnCurrentTimetable.SetCheck( BST_UNCHECKED );
        m_btnNextTimetable.SetCheck( BST_CHECKED );
        m_btnNextTimetable.SetFocus();

        if ( loadTimetable( true ) )
        {
			m_btnNextTimetable.SetCheck( BST_UNCHECKED );
			m_btnCurrentTimetable.SetCheck( BST_CHECKED );
			m_btnCurrentTimetable.SetFocus();
        }
    }

    void StationTimeTableManagerDlg::OnButtonTomorrowScheduleTimeTable()
    {
        if ( !m_bLoadToday && m_bIsLoadModeTimes )
        {
            return;
        }

        m_btnNextTimetable.SetCheck( BST_UNCHECKED );
        m_btnCurrentTimetable.SetCheck( BST_CHECKED );
        m_btnCurrentTimetable.SetFocus();

        if ( loadTimetable( false ) )
        {
			m_btnCurrentTimetable.SetCheck( BST_UNCHECKED );
			m_btnNextTimetable.SetCheck( BST_CHECKED );
			m_btnNextTimetable.SetFocus();
        }
    }

	void StationTimeTableManagerDlg::setWindowCaption(bool busy)
	{
        std::string strCaption = "时间表管理器";
		if (busy)
		{
			strCaption += " - 正在下载时间表";
		}
		GetParent()->SetWindowText(strCaption.c_str());
	}

    bool StationTimeTableManagerDlg::loadTimetable( bool bToday )
    {
		setWindowCaption(true);
		m_bLoadToday = bToday;
		if (m_bIsLoadModeTimes)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "The time tables are loading");
		    return true;
		}
        m_bIsLoadModeTimes = true;
        
        m_loadTimeTableThread = new TimeTableManagerActionThread(this);
        m_loadTimeTableThread->setAction(TimeTableManagerAction_LoadTimeTable);
        m_loadTimeTableThread->start();

        return true;
    }
}
