#if !defined (Transactive_3002_GF_App_Bas_TimeTableManager_TimeTableManagerBaseDialog)
#define Transactive_3002_GF_App_Bas_TimeTableManager_TimeTableManagerBaseDialog
#if _MSC_VER > 1000
#pragma once
#endif

#include "bus/generic_gui/src/ResizingProperties.h"
#include "app/bas/TimeTableManager/src/CommonDefine.h"
#include "app/bas/TimeTableManager/src/OCCTimeTableListCtrl.h"
#include "app/bas/TimeTableManager/src/StationTimeTableListCtrl.h"

namespace TA_IRS_App
{
	// TimeTableManagerBaseDialog dialog
	class TimeTableManagerBaseDialog : public CDialog
	{
		DECLARE_DYNAMIC(TimeTableManagerBaseDialog)

	public:
		TimeTableManagerBaseDialog(UINT nIDTemplate, CWnd* pParent = NULL);   // standard constructor
		virtual ~TimeTableManagerBaseDialog();
	public:
		virtual BOOL OnInitDialog();
		virtual void OnCancel();
	public:
		// The TimeTableManagerBaseDialog Interface for TimeTableManagerMainDlg
		virtual TA_Base_Bus::ResizingProperties getResizingProperties();
		virtual DlgInitializeSize getDlgInitSize();
		virtual void init();
		virtual CString getApplicationName();
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		DECLARE_MESSAGE_MAP()
	};
}

#endif //Transactive_3002_GF_App_Bas_TimeTableManager_TimeTableManagerBaseDialog