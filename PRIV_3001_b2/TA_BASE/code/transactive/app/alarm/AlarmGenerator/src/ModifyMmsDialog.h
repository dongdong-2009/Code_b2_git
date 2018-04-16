#if !defined(AFX_MODIFYMMSDIALOG_H__34F6F106_96AD_4795_83E3_928136CFB7E8__INCLUDED_)
#define AFX_MODIFYMMSDIALOG_H__34F6F106_96AD_4795_83E3_928136CFB7E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModifyMmsDialog.h : header file
//

#include <string>
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"

/////////////////////////////////////////////////////////////////////////////
// ModifyMmsDialog dialog

class ModifyMmsDialog : public CDialog
{
// Construction
public:
	ModifyMmsDialog(const std::string alarmId, unsigned long locationKey, TA_Base_Core::MmsStateType curMms, std::string alarmDetail, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ModifyMmsDialog)
	enum { IDD = IDD_MMS_DIALOG };
	CButton	m_MmsCommit;
	CButton	m_MmsClose;
	CButton m_CurMms;
	CButton m_Mms1;
	CButton m_Mms2;
	CButton m_Mms3;
	CButton m_Mms4;
	CButton m_Mms5;
	CButton m_Mms6;
	CEdit	m_AlarmDetail;
	//CString	m_AlarmText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ModifyMmsDialog)
	public:
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ModifyMmsDialog)
	afx_msg void OnCommit();
	afx_msg void OnMms6();
	afx_msg void OnMms5();
	afx_msg void OnMms4();
	afx_msg void OnMms3();
	afx_msg void OnMms2();
	afx_msg void OnMms1();
	afx_msg void OnMmsCur();
	afx_msg void OnClose();
	afx_msg void OnRefresh();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	
	// The selected mms state type
	std::string m_alarmId;
	unsigned long m_locationKey;
	std::string m_mmsStateType;
	std::string m_alarmDetail;

	// This is to set the alarm detail into the edit box
	void setAlarmDetail (std::string alarmDetail);

	// This function reselects the submitted radio button number and set the mms state type to the one selected
	void disableAllRadioExcept(int radioNumber);

	// This function converts the submitted string to the mms state type
	TA_Base_Core::MmsStateType convertStringToMmsStateType(std::string mmsState);

	// This function converts the submitted mms state type into a corresponding string
	std::string convertMmsStateTypeToString(TA_Base_Core::MmsStateType mmsStateType);

	// This function set the radio buttons text based on the current mms state type to names of mms state type
	void setAllRadioTextExcept(std::string curStateType);

	// This function is to set the selected radio button's text
	void setRadioButtonText(std::string text, int radioNumber);

	// This function is to refresh the edit box upon updating the mms state type to refresh the latest
	// The data is to be taken out of the database to test that the database has actually been updated
	void refreshAlarmDetail();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODIFYMMSDIALOG_H__34F6F106_96AD_4795_83E3_928136CFB7E8__INCLUDED_)
