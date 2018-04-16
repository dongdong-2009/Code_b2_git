#if !defined (Transactive_3002_GF_App_Bas_TimeTableManager_TimeTableManagerMainDlg)
#define Transactive_3002_GF_App_Bas_TimeTableManager_TimeTableManagerMainDlg
#if _MSC_VER > 1000
#pragma once
#endif

#include <memory>
#include "resource.h"
#include "bus/generic_gui/src/TransActiveDialog.h"
#include "app/bas/TimeTableManager/src/TimeTableManagerBaseDialog.h"

namespace TA_IRS_App
{
	// TimeTableManagerMainDlg dialog
	class TimeTableManagerMainDlg : public TA_Base_Bus::TransActiveDialog
	{
		DECLARE_DYNAMIC(TimeTableManagerMainDlg)

	public:
		TimeTableManagerMainDlg(TA_Base_Bus::IGUIAccess& controlClass, CWnd* pParent = NULL);   // standard constructor
		virtual ~TimeTableManagerMainDlg();

	// Dialog Data
		enum { IDD = IDD_TIMETABLEMANAGER_MAIN_DIALOG };
		HICON m_hIcon;
	public:

		bool isOccTimeTableManager();
		virtual void init();
	private:
		void setResizingInfo();
		void initializeTimeTableManagerImp();
		void moveTimeTableManagerImp();
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		virtual BOOL OnInitDialog();
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void TimeTableManagerMainDlg::OnPaint();
		DECLARE_MESSAGE_MAP()
	private:
		TimeTableManagerBaseDialog* m_timeTableManagerImp;
	};
}

#endif //Transactive_3002_GF_App_Bas_TimeTableManager_TimeTableManagerMainDlg