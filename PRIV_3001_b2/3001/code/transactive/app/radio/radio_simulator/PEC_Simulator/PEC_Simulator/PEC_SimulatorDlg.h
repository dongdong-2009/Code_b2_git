// PEC_SimulatorDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include <string>

// CPEC_SimulatorDlg dialog
class CPEC_SimulatorDlg : public CDialog
{
// Construction
public:
	CPEC_SimulatorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PEC_SIMULATOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	std::string convertToString(const CString& str);
public:
	CComboBox m_comboTrainNumber;
	CComboBox m_comboCarNumber;
	CComboBox m_PECNumber;
	CComboBox m_PECValue;
	CComboBox m_PECSource;
	afx_msg void OnBnClickedButtonSendCommand();

	void getPECInformation();

private:
	std::string m_strTrainNumber;
	std::string m_strCarNumber;
	std::string m_strPECNumber;
	std::string m_strPECValue;
	std::string m_strPECSource;

};
