#pragma once
#include "StdAfx.h"
#include "app\cctv\video_display_manager\src\Resource.h"
#include "afxwin.h"

// CTestDlg dialog

class CTestDlg : public CDialog
{
	DECLARE_DYNAMIC(CTestDlg)

public:
	CTestDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTestDlg();

// Dialog Data
	enum { IDD = IDD_TEST_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
private:
	CEdit m_runParam;
	CEdit m_runParamValue;
	CEdit m_runPramAndValue;
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonAckAlarm();
	afx_msg void OnBnClickedButtonSubmitAlarm();
	CEdit m_alarmEntityKeyAck;
	CEdit m_alarmEntitykeySubmit;
};
