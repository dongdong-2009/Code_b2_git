
// HusVideoPlayerDlg.h : header file
//

#pragma once
#include "resource.h"
#include "activexclientctrl1.h"
#include "afxwin.h"


// CHusVideoPlayerDlg dialog
class CHusVideoPlayerDlg : public CDialog
{
// Construction
public:
	CHusVideoPlayerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TESTACTIVEX_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
    afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnShowOrHideWindow(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	void fitWindow();
	CActivexclientctrl1 m_ax;
	UINT m_timerID;
};
