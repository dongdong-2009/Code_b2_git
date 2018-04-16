#if !defined (Transactive_3002_GF_App_Bas_TimeTableManager_StationTimeTableManagerDlg)
#define Transactive_3002_GF_App_Bas_TimeTableManager_StationTimeTableManagerDlg
#if _MSC_VER > 1000
#pragma once
#endif

#include "afxcmn.h"
#include "app/bas/TimeTableManager/src/TimeTableManagerBaseDialog.h"
#include "app/bas/TimeTableManager/src/TimeTableManagerActionThread.h"

namespace TA_IRS_App
{
	// StationTimeTableManagerDlg dialog
	class StationTimeTableManagerDlg : public TimeTableManagerBaseDialog
										, public ActionThreadCallbackInterface
	{
		DECLARE_DYNAMIC(StationTimeTableManagerDlg)

	public:
		StationTimeTableManagerDlg(CWnd* pParent = NULL);   // standard constructor
		virtual ~StationTimeTableManagerDlg();

	// Dialog Data
		enum { IDD = IDD_STATION_TIMETABLEMANAGER_DIALOG };
	public:
		// the ActionThreadCallbackInterface interface implement
        virtual void ActionThreadFunction(TimeTableManagerAction action);
        virtual void CancelActionThreadFunction(TimeTableManagerAction action);
	public:
		// The interface in TimeTableManagerBaseDialog
		virtual TA_Base_Bus::ResizingProperties getResizingProperties();
		virtual DlgInitializeSize getDlgInitSize();
		virtual void init();
		virtual CString getApplicationName();


	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		virtual BOOL OnInitDialog();
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg LRESULT LoadTimeTableFromAgent(WPARAM wParam, LPARAM lParam);
        afx_msg void OnButtonTodayScheduleTimeTable();
        afx_msg void OnButtonTomorrowScheduleTimeTable();
		DECLARE_MESSAGE_MAP()
	private:
		void InitializeDlgData();
        bool loadTimetable( bool bToday );

		void setWindowCaption(bool busy);
	private:
		StationTimeTableListCtrl m_timeTableListCtrl;
        CButton m_btnCurrentTimetable;
        CButton m_btnNextTimetable;
		TimeTableManagerActionThread* m_initializeThread;
		TimeTableManagerActionThread* m_loadTimeTableThread; //get the timetable data from agent

        bool m_bLoadToday;
		bool m_bIsLoadModeTimes; // is load from agent now
	};

}

#endif //Transactive_3002_GF_App_Bas_TimeTableManager_StationTimeTableManagerDlg