#if !defined(AFX_VIDEODISPLAYMANAGERDLG_H__911C5097_68C5_11D7_B175_0050BAB094CE__INCLUDED_)
#define AFX_VIDEODISPLAYMANAGERDLG_H__911C5097_68C5_11D7_B175_0050BAB094CE__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "TrainBorne_DisplayManager/resource.h"

//#include "app/TrainBorneTest/Label.h"

#include "bus/generic_gui/src/TransActiveDialog.h"
#include "bus/mfc_extensions/src/multi_font_property_sheet/MultiFontPropertySheet.h"
#include "bus/trains/TrainCommonLibrary/src/CctvTypes.h"
#include "bus/trains/TrainCorbaProxy/src/ITrainCctvUpdateObserver.h"
#include "bus/trains/TrainCorbaProxy/src/TrainCctvCorbaProxy.h"

#include "core/utilities/src/RunParams.h"
//#include "VideoDisplayManagerPresenter.h"

#include "ITrainBorne.h"
#include <vector>
// VideoDisplayManagerDlg dialog

#define LOCATION_NAME_OCC     "OCC"
//#define LOCATION_NAME_KOCC	  "KOCC"  
#define LOCATION_NAME_DPT_1	  "KCD"
#define LOCATION_NAME_DPT_2	  "TSFB"
#define LOCATION_NAME_DPT_3	  "GDCC"

namespace TA_IRS_App
{
	#define _AFXEXT
class VideoDisplayPresenter;
class DlgAlarmEventTest;
class /*__declspec(dllexport)*/ VideoDisplayManagerDlg : public ITrainBorne
{
	DECLARE_DYNAMIC(VideoDisplayManagerDlg)

public:
	VideoDisplayManagerDlg(CWnd* pParent);   // standard constructor
	virtual ~VideoDisplayManagerDlg();
	virtual BOOL OnInitDialog();
	virtual void TB_TrainSelected(unsigned long trainID);
	virtual bool TB_DoApply();
	virtual void TB_PageActived(bool bSelected);
	virtual void TB_MouseAction(UINT action,UINT flag,CPoint pt);
// Dialog Data
	enum { IDD = IDD_VIDEODISPLAYMANAGER_DIALOG_OCC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestory();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnInitDisplayManager( WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTVSSCommand( WPARAM wParam, LPARAM lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void setupMonitors();
private:
	VideoDisplayPresenter* m_presenter;
	DlgAlarmEventTest*	m_dlgAlarmEventTest;
	bool m_bMsgFromParent;
	unsigned int m_locationKey; 
	std::string m_locationname;
	UINT m_groupCtrlID;
};
}
#endif
