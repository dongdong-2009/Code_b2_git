#pragma once
#include "StdAfx.h"
#include "app\cctv\video_display_manager\src\Resource.h"
#include "core/utilities/src/RunParams.h"

// CTestDlg dialog

class CTestDlg : public CDialog
{
	DECLARE_DYNAMIC(CTestDlg)

public:
	CTestDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTestDlg();
	void setRunParamUser(TA_Base_Core::RunParamUser* runParamUser);

// Dialog Data
	enum { IDD = IDD_TEST_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
private:
	CEdit m_runParam;
	CEdit m_runParamValue;
	TA_Base_Core::RunParamUser* m_paramUser;
};
