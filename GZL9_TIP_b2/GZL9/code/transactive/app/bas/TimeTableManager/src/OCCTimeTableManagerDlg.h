#if !defined (Transactive_3002_GF_App_Bas_TimeTableManager_OCCTimeTableManagerDlg)
#define Transactive_3002_GF_App_Bas_TimeTableManager_OCCTimeTableManagerDlg
#if _MSC_VER > 1000
#pragma once
#endif

#include "resource.h"
#include "app/bas/TimeTableManager/src/TimeTableManagerBaseDialog.h"
#include "app/bas/TimeTableManager/src/TimeTableManagerActionThread.h"
#include "app/bas/TimeTableManager/src/CommonDefine.h"
#include "app/bas/TimeTableManager/src/TimeTableManagerDataHelper.h"
#include "afxwin.h"

namespace TA_IRS_App
{
	// OCCTimeTableManagerDlg dialog
	class OCCTimeTableManagerDlg : public TimeTableManagerBaseDialog
									, public ActionThreadCallbackInterface
									, public ILocationFilter
	{
		DECLARE_DYNAMIC(OCCTimeTableManagerDlg)
	// Construction
	public:
		OCCTimeTableManagerDlg(CWnd* pParent = NULL);	// standard constructor

	// Dialog Data
		enum { IDD = IDD_OCC_TIMETABLEMANAGER_DIALOG };
	public:
		// the ActionThreadCallbackInterface interface implement
		virtual void ActionThreadFunction(TimeTableManagerAction action);
        virtual void CancelActionThreadFunction(TimeTableManagerAction action);
	public:
		// The interface in TimeTableManagerBaseDialog
		virtual TA_Base_Bus::ResizingProperties getResizingProperties();
		virtual DlgInitializeSize OCCTimeTableManagerDlg::getDlgInitSize();
		virtual void init();
		virtual CString getApplicationName();
		//The interface in ILocationFilter
		virtual unsigned long getLocationKey();
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Implementation
	protected:
		HICON m_hIcon;
		// Generated message map functions
		virtual BOOL OnInitDialog();
		afx_msg HCURSOR OnQueryDragIcon();
		afx_msg void OnPaint();
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnButtonRecoverTimeTableClick();
		afx_msg void OnButtonLocalStoreClick();
        afx_msg void OnButtonCancelClick();
        afx_msg void OnCbnComboStationsFocus();
		afx_msg void OnCbnSelchangeComboStations();
        afx_msg void OnBnClickedRadioWorkdays();
        afx_msg void OnBnClickedRadioFriday();
        afx_msg void OnBnClickedRadioSaturday();
        afx_msg void OnBnClickedRadioSunday();
        afx_msg void OnBnClickedRadioHoliday();
        afx_msg void OnBnClickedRadioSpecial1();
        afx_msg void OnBnClickedRadioSpecial2();
        afx_msg void OnBnClickedRadioSpecial3();
        afx_msg void OnBnClickedRadioSpecial4();
        afx_msg void OnBnClickedRadioSpecial5();
		afx_msg void OnBnClickedRadioCurrentSchedule();
		afx_msg void OnBnClickedRadioTomorrowSchedule();
		afx_msg void OnBnClickedSendTodaySchedule();
		afx_msg void OnBnClickedSendNextDaySchedule();
		// Fill the dialog data
		afx_msg LRESULT InitializeDialogData(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT LoadTimeTableFromAgent(WPARAM wParam, LPARAM lParam);
		DECLARE_MESSAGE_MAP()
	private:
        void refreshLocalTimeTable( const int nType );
        void loadServerTimeTable( const int nType );
		void filterTimeTable( const int nType );
		void downloadTimetable();
        void revertFocus();
		bool getSelectLocationAndType(const int nType, unsigned long& locationKey, std::string& typeName);
        void getTimeTableDisplayNameAndTime(const TimeTableScheduleInfo_SmartPtr& scheduleInfo, std::string& strName, std::string& strTime);
		void updateTimeTable();
		unsigned long getModeKeyByName();

		void CanEditModeList(BOOL bEditable);

		void setWindowCaption(bool busy);

		void sendTimeTable(unsigned long locationKey, TimeTableType timeTableType, bool isTodayTimeTable);
	private:
		OCCTimeTableListCtrl m_timeTableListCtrl;
		TimeTableManagerActionThread* m_initializeThread; //initialize data
		TimeTableManagerActionThread* m_loadTimeTableThread; //get the timetable data from agent
		CComboBox m_allLocations;
		CImageList m_OCCTimeTableImageList;

		CComboBox m_timeTableTodayName;
		CComboBox m_timeTableTommorowName;
        int m_nType;

		bool m_bIsFilterModeTimes;
		bool m_bIsLoadModeTimes;
	};
}

#endif //Transactive_3002_GF_App_Bas_TimeTableManager_OCCTimeTableManagerDlg
