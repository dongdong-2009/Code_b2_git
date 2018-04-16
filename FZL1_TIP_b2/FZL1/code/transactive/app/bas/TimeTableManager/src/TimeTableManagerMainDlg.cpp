// src\TimeTableManagerMainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "app/bas/TimeTableManager/src/TimeTableManager.h"
#include "app/bas/TimeTableManager/src/TimeTableManagerMainDlg.h"
#include "app/bas/TimeTableManager/src/OCCTimeTableManagerDlg.h"
#include "app/bas/TimeTableManager/src/StationTimeTableManagerDlg.h"
#include "app/bas/TimeTableManager/src/CommonDefine.h"
#include "app/bas/TimeTableManager/src/TimeTableManagerUtility.h"

namespace TA_IRS_App
{
	// TimeTableManagerMainDlg dialog
	IMPLEMENT_DYNAMIC(TimeTableManagerMainDlg, TA_Base_Bus::TransActiveDialog)
	TimeTableManagerMainDlg::TimeTableManagerMainDlg(TA_Base_Bus::IGUIAccess& controlClass, CWnd* pParent /*=NULL*/)
		: TA_Base_Bus::TransActiveDialog(controlClass, TimeTableManagerMainDlg::IDD, pParent)
		,m_timeTableManagerImp(NULL)
	{
		FUNCTION_ENTRY("TimeTableManagerMainDlg");
		m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
		FUNCTION_EXIT;
	}

	TimeTableManagerMainDlg::~TimeTableManagerMainDlg()
	{
		FUNCTION_ENTRY("~TimeTableManagerMainDlg");
		FUNCTION_EXIT;
	}

	void TimeTableManagerMainDlg::DoDataExchange(CDataExchange* pDX)
	{
		using namespace TA_Base_Bus;
		FUNCTION_ENTRY("DoDataExchange");
		TransActiveDialog::DoDataExchange(pDX);
		FUNCTION_EXIT;
	}


	BEGIN_MESSAGE_MAP(TimeTableManagerMainDlg, TA_Base_Bus::TransActiveDialog)
		ON_WM_SIZE()
		ON_WM_PAINT()
	END_MESSAGE_MAP()

	BOOL TimeTableManagerMainDlg::OnInitDialog()
	{
		FUNCTION_ENTRY("OnInitDialog");
		initializeTimeTableManagerImp();
		setResizingInfo();	// this function must before TA_Base_Bus::TransActiveDialog::OnInitDialog()
		TA_Base_Bus::TransActiveDialog::OnInitDialog();	

		SetIcon(m_hIcon, TRUE);			// 设置大图标
		SetIcon(m_hIcon, FALSE);		// 设置小图标
		// set the Initialize size
  		CRect rect;
  		GetWindowRect(&rect);
  		DlgInitializeSize dlgSize = m_timeTableManagerImp->getDlgInitSize();
  		MoveWindow(rect.left, rect.top, dlgSize.nWidth, dlgSize.nHeight, TRUE);
		ShowWindow(SW_SHOW);
		m_timeTableManagerImp->ShowWindow(SW_SHOW);
		SetWindowText(m_timeTableManagerImp->getApplicationName());
		
		Invalidate(TRUE);
		FUNCTION_EXIT;
		return TRUE;
	}

	// TimeTableManagerMainDlg message handlers
	void TimeTableManagerMainDlg::OnSize(UINT nType, int cx, int cy)
	{
		FUNCTION_ENTRY("OnSize");
		using namespace TA_Base_Bus;
		TransActiveDialog::OnSize(nType,cx,cy);
		moveTimeTableManagerImp();
		FUNCTION_EXIT;
	}

	void TimeTableManagerMainDlg::init()
	 {
		 FUNCTION_ENTRY("init");
		// Do some init action.
		 FUNCTION_EXIT;
	 }

	void TimeTableManagerMainDlg::initializeTimeTableManagerImp()
	{
		FUNCTION_ENTRY("initializeTimeTableManagerImp");
		if (isOccTimeTableManager())
		{
			m_timeTableManagerImp = new OCCTimeTableManagerDlg(this);
			m_timeTableManagerImp->Create(OCCTimeTableManagerDlg::IDD, this);
		}
		else
		{
			m_timeTableManagerImp = new StationTimeTableManagerDlg(this);
			m_timeTableManagerImp->Create(StationTimeTableManagerDlg::IDD, this);
		}
		m_timeTableManagerImp->ShowWindow(SW_HIDE);
		
		FUNCTION_EXIT;
	}

	void TimeTableManagerMainDlg::moveTimeTableManagerImp()
	{
		FUNCTION_ENTRY("moveTimeTableManagerImp");
		if (NULL != m_timeTableManagerImp &&
			IsWindow(m_timeTableManagerImp->GetSafeHwnd()))
		{
			CRect clientRect;
			GetClientRect(&clientRect);
			m_timeTableManagerImp->MoveWindow(clientRect, TRUE);
			m_timeTableManagerImp->Invalidate(TRUE);
		}
		FUNCTION_EXIT;
	}

	void TimeTableManagerMainDlg::setResizingInfo()
	{
		FUNCTION_ENTRY("setResizingInfo");
		TA_Base_Bus::ResizingProperties rp;
		rp = m_timeTableManagerImp->getResizingProperties();
		setResizingProperties(rp);
		FUNCTION_EXIT;
	}

	bool TimeTableManagerMainDlg::isOccTimeTableManager()
	{
		FUNCTION_ENTRY("isOccTimeTableManager");
		using namespace TA_Base_Core;

		bool bIsOcc = true;
		
		// set the value
		// if not config the type or type is OccTimeTableManager
		// return true 
		if (RunParams::getInstance().isSet(RPARAM_TIMETABLE_TYPE))
		{
			std::string strTimeTableTypeValue = RunParams::getInstance().get(RPARAM_TIMETABLE_TYPE);
			bIsOcc = (0 == strTimeTableTypeValue.compare(RPARAM_TIMETABLE_TYPE_VALUE_OCC));
		}
		else
		{
			ILocation_SmartPtr locationPtr = TimeTableManagerDataHelperInstance::instance()->getLocalLocation();
			if (NULL != locationPtr.get())
			{
				bIsOcc = ( locationPtr->getLocationType() == TA_Base_Core::ILocation::OCC );
			}
		}

		FUNCTION_EXIT;
		return bIsOcc;
	}

	void TimeTableManagerMainDlg::OnPaint()
	{
		if (IsIconic())
		{
			CPaintDC dc(this); // 用于绘制的设备上下文

			SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

			// 使图标在工作矩形中居中
			int cxIcon = GetSystemMetrics(SM_CXICON);
			int cyIcon = GetSystemMetrics(SM_CYICON);
			CRect rect;
			GetClientRect(&rect);
			int x = (rect.Width() - cxIcon + 1) / 2;
			int y = (rect.Height() - cyIcon + 1) / 2;

			// 绘制图标
			dc.DrawIcon(x, y, m_hIcon);
		}
		else
		{
			TA_Base_Bus::TransActiveDialog::OnPaint();
		}
	}
}
