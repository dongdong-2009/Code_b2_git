/**
  * ModifyGraph.cpp
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/history/history_viewer/src/ModifyGraph.cpp $
  * @author:  A. Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This dialog is launched from the main dialog. It allows modification to the mode of the 
  * graph (Live, Non-live), the representation (data type displayed), the start time and the
  * x-axis range.
  *
  */

#include "app/history/history_viewer/src/stdafx.h"
#include "app/history/history_viewer/src/historyviewer.h"
#include "app/history/history_viewer/src/ModifyGraph.h"
#include "app/history/history_viewer/src/HistoryViewerDlg.h"
#include "app/history/history_viewer/src/globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_App
{

    /////////////////////////////////////////////////////////////////////////////
    // CModifyGraph dialog

    CModifyGraph::CModifyGraph(BOOL nowCheck, int historyType, COleDateTime startDateTime,
        int rangeDays, COleDateTime rangeTime, const CComboBox& dataTypeCtrl, CWnd* pParent)

        : CDialog(CModifyGraph::IDD, pParent)
    {
        //{{AFX_DATA_INIT(CModifyGraph)
        m_Start = startDateTime;
        m_Now = nowCheck;
        m_DataType = historyType;
        m_rangeDays = rangeDays;
        m_rangeTime = rangeTime;

        //}}AFX_DATA_INIT

        calculateRangeSeconds();

        //
        // Store the list data away so it can be used to initialise
        // the pull down list during OnInitDialog
        //
        DATATYPE dataType;
        for( int i=0; i<dataTypeCtrl.GetCount(); i++ )
        {
	        dataTypeCtrl.GetLBText( i, dataType.itemText );
	        dataType.itemData = dataTypeCtrl.GetItemData( i );
	        m_DataTypes.push_back( dataType );
        }
    }


    void CModifyGraph::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CModifyGraph)
        DDX_Control(pDX, IDC_RANGETIME, m_rangeTimeCtrl);
        DDX_Control(pDX, IDC_RANGE_DAYS, m_rangeDaysCtrl);
        DDX_Control(pDX, IDC_NOWCHECK, m_NowCtrl);
        DDX_Control(pDX, IDC_STARTDATETIME, m_StartCtrl);
        DDX_Control(pDX, IDC_DATAPOINT_LIST, m_DataTypeListCtrl);
        //DDX_Control(pDX, IDC_RANGECTRL, m_RangeCtrl);
        DDX_DateTimeCtrl(pDX, IDC_STARTDATETIME, m_Start);
        DDX_Check(pDX, IDC_NOWCHECK, m_Now);
        DDX_CBIndex(pDX, IDC_DATAPOINT_LIST, m_DataType);
        //DDX_OCInt(pDX, IDC_RANGECTRL, DISPID(7), m_RangeSeconds);
        //DDX_Text(pDX, IDC_RANGEDAYS, m_rangeDays);
        DDX_DateTimeCtrl(pDX, IDC_RANGETIME, m_rangeTime);
        //}}AFX_DATA_MAP

        //
        // Manually calculate m_RangeSeconds
        //
        calculateRangeSeconds();

        //
        // Note: History type must be processed before range verification
        //
        CHistoryViewerDlg::DDV_NoRange(pDX, m_RangeSeconds);		            // Check for no range
        int histConfig = m_DataTypeListCtrl.GetItemData( m_DataType );
        CHistoryViewerDlg::DDV_LargeRange( pDX, m_RangeSeconds, histConfig);	// Check for range too big
    }


    BEGIN_MESSAGE_MAP(CModifyGraph, CDialog)
        //{{AFX_MSG_MAP(CModifyGraph)
        ON_BN_CLICKED(IDC_NOWCHECK, OnNowCheck)
        //ON_EN_UPDATE(IDC_RANGEDAYS, OnUpdateRangedays)
        ON_NOTIFY(DTN_DATETIMECHANGE, IDC_STARTDATETIME, OnStartDateTimeChange)
        ON_NOTIFY(DTN_DATETIMECHANGE, IDC_RANGE_DAYS, OnRangeDaysChange)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CModifyGraph message handlers

    //
    // OnInitDialog
    //
    BOOL CModifyGraph::OnInitDialog() 
    {
        CDialog::OnInitDialog();

        //
        // Set format for start date/time
        //
        m_StartCtrl.SetFormat( "dd/MM/yyy HH:mm" );
        m_StartCtrl.EnableWindow( !m_Now );

        m_rangeTimeCtrl.SetFormat( "HH:mm" );
        //m_rangeDaysCtrl.SetLimitText(2);
        m_rangeDaysCtrl.SetFormat("mm");

        COleDateTime temp;
        temp.SetTime(0, m_rangeDays, 0);
        m_rangeDaysCtrl.SetTime(temp);

        //
        // Set up the pull down list
        //
        int lastIndex=0;
        for(unsigned int i=0; i<m_DataTypes.size(); i++ )
        {
	        lastIndex = m_DataTypeListCtrl.AddString( m_DataTypes[i].itemText );
	        m_DataTypeListCtrl.SetItemData( i, m_DataTypes[i].itemData );
        }
        m_DataTypeListCtrl.SetCurSel( m_DataType );

        return TRUE;  // return TRUE unless you set the focus to a control
                      // EXCEPTION: OCX Property Pages should return FALSE
    }


    //
    // OnNowCheck
    //
    void CModifyGraph::OnNowCheck() 
    {
        m_Now = m_NowCtrl.GetCheck();
        m_StartCtrl.EnableWindow( !m_Now );
    }


    //
    // OnUpdateRangedays
    //
    //void CModifyGraph::OnUpdateRangedays() 
    //{
        // TODO: If this is a RICHEDIT control, the control will not
        // send this notification unless you override the CDialog::OnInitDialog()
        // function to send the EM_SETEVENTMASK message to the control
        // with the ENM_UPDATE flag ORed into the lParam mask.
        
        //int temp = m_rangeDays;

        //UpdateData(TRUE);

        //if(m_rangeDays < 0 && m_rangeDays > 99)
        //{
        //    m_rangeDays = temp;
        //}

        //UpdateData(FALSE);
    //}


    //
    // calculateRangeSeconds
    //
    void CModifyGraph::calculateRangeSeconds()
    {
        int hourRange = m_rangeTime.GetHour();
        int minuteRange = m_rangeTime.GetMinute();

        int secondsInDay = 24*60*60;
        m_RangeSeconds = secondsInDay*m_rangeDays + 60*60*hourRange + 60*minuteRange;
    }


    void CModifyGraph::OnRangeDaysChange(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        CWnd* button = GetDlgItem(IDOK);

        COleDateTime temp;
        m_rangeDaysCtrl.GetTime(temp);
        m_rangeDays = temp.GetMinute();

        button->EnableWindow();

        *pResult = 0;
    }

    //
    // OnStartDateTimeChange
    //
    void CModifyGraph::OnStartDateTimeChange(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        int currentYear = COleDateTime::GetCurrentTime().GetYear();

        COleDateTime ctrlTime;
        m_StartCtrl.GetTime(ctrlTime);
        int year = ctrlTime.GetYear();

        CWnd* button = GetDlgItem(IDOK);

        if(year < currentYear - TREND_VIEWER_RANGE_YEAR || year > currentYear)
        {
            button->EnableWindow(false);
            return;
        }

        int currentMonth = COleDateTime::GetCurrentTime().GetMonth();
        int month = ctrlTime.GetMonth();
        if(year == currentYear - TREND_VIEWER_RANGE_YEAR && month < currentMonth ||
            year == currentYear && month > currentMonth)
        {
            button->EnableWindow(false);
            return;
        }
        
        int currentDay = COleDateTime::GetCurrentTime().GetDay();
        int day = ctrlTime.GetDay();
        if(year == currentYear - TREND_VIEWER_RANGE_YEAR && month == currentMonth && day < currentDay ||
            year == currentYear && month == currentMonth && day > currentDay)
        {
            button->EnableWindow(false);
            return;
        }
		        
        // Set apply icon to show that apply is required
        button->EnableWindow();
        
        *pResult = 0;
    }

} // TA_App

