/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/wild/wild_agent/stub/src/WILDSimulatorDlg.h $
  * @author:  Andy Siow
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  *
  */

#if !defined(AFX_WILDSimulatorDLG_H__B1FC56F1_91F3_4680_9193_F5B9A34FE350__INCLUDED_)
#define AFX_WILDSimulatorDLG_H__B1FC56F1_91F3_4680_9193_F5B9A34FE350__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "bus/generic_gui/src/TransActiveDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CWILDSimulatorDlg dialog

namespace TA_Base_Bus
{
    class IGUIAccess;
}

namespace TA_Base_Core
{
	class CommsMessageSender;
}

class CWILDSimulatorDlg : public TA_Base_Bus::TransActiveDialog
{
// Construction
public:
	CWILDSimulatorDlg(TA_Base_Bus::IGUIAccess* genericGUICallback);	// standard constructor


// Dialog Data
	//{{AFX_DATA(CECSManagerDlg)
	enum { IDD = IDD_WILDSimulator_DIALOG };
	CButton m_sendWILDUpdateButton;
	CButton m_atsButton;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWILDSimulatorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWILDSimulatorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSendWILDUpdateButton();
	afx_msg void OnATSUpdateButton();
	afx_msg void OnResetWILDButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// WILD
	CDateTimeCtrl m_datePicker;
	CDateTimeCtrl m_timePicker;
	//ATS
	CDateTimeCtrl m_atsDatePicker;
	CDateTimeCtrl m_atsTimePicker;

	CEdit m_PVNumber;
	CEdit m_ATCCarNumber;

	CButton m_checkA1;
	CButton m_checkB1;
	CButton m_checkA2;
	CButton m_checkB2;
	CButton m_checkA3;
	CButton m_checkB3;
	CButton m_checkA4;
	CButton m_checkB4;
	CButton m_checkA5;
	CButton m_checkB5;
	CButton m_checkA6;
	CButton m_checkB6;
	CButton m_checkA7;
	CButton m_checkB7;
	CButton m_checkA8;
	CButton m_checkB8;
	CButton m_checkA9;
	CButton m_checkB9;
	CButton m_checkA10;
	CButton m_checkB10;
	CButton m_checkA11;
	CButton m_checkB11;
	CButton m_checkA12;
	CButton m_checkB12;
	CButton m_critCheck;
	CButton m_resetButton;

	unsigned int decToBin(unsigned char decimal);
	int binToDec(const std::string& binary);
	unsigned long hexToDec(unsigned char hexValue);

	TA_Base_Core::CommsMessageSender*	m_commsMessageSender;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WILDSimulatorDLG_H__B1FC56F1_91F3_4680_9193_F5B9A34FE350__INCLUDED_)
