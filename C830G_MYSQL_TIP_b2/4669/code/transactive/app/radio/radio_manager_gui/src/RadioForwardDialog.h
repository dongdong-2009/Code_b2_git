/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/RadioForwardDialog.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_RADIOFORWARDDIALOG_H__CB5A00F6_4F70_11D7_B14D_0050BAB1D931__INCLUDED_)
#define AFX_RADIOFORWARDDIALOG_H__CB5A00F6_4F70_11D7_B14D_0050BAB1D931__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus\radio\RadioAgentCorbaDef_4669\idl\src\IRadioSessionCorbaDef.h"
#include "core\naming\src\NamedObject.h"

// RadioForwardDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRadioForwardDialog dialog

class CRadioForwardDialog : public CDialog
{
// Construction
public:
	CRadioForwardDialog(CWnd* pParent = NULL);   // standard constructor

	// Indicate if forwarding was set
	bool IsForward() { return m_forwardEnabled; }
    CString getForwardAddress() { return m_forwardAddress; }

// Dialog Data
	//{{AFX_DATA(CRadioForwardDialog)
	enum { IDD = IDD_RADIOFORWARD };
	CComboBox	m_IDCombo;
	CComboBox	m_TSICombo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRadioForwardDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRadioForwardDialog)
	virtual BOOL OnInitDialog();
	afx_msg void onOK();
	afx_msg void onRadio1();
	afx_msg void onRadio2();
	afx_msg void onRadio3();
	afx_msg void onSelchangeTsiCombo();
	afx_msg void onEditchangeTsiCombo();
	afx_msg void onEditchangeIdCombo();
	afx_msg void onSelchangeIdCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
private:

	void loadSubscriberList();
    void SynchroniseIDwithTSI(const char* selectedTSI);
	void SynchroniseTSIwithID(const char* selectedID);
	void EnableOKButton();

	bool	m_forwardEnabled;
    CString m_forwardAddress;

	typedef std::pair<std::string,std::string>  subscriberEntry;
	std::vector<subscriberEntry >    m_subscribers;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RADIOFORWARDDIALOG_H__CB5A00F6_4F70_11D7_B14D_0050BAB1D931__INCLUDED_)
