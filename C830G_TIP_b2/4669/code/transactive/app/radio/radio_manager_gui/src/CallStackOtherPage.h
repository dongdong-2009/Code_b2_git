/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
// CallStackOtherPage.h : header file
//

#ifndef __CALLSTACKOTHERPAGE_H__
#define __CALLSTACKOTHERPAGE_H__

//#include "app\radio\radio_agent_stub\src\RadioSession.h"
#include "app/radio/radio_manager_gui/src/RadioCallStackListCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// CCallStackCurrentPage dialog


/////////////////////////////////////////////////////////////////////////////
// CCallStackOtherPage dialog

class CCallStackOtherPage : public CDialog
{
	DECLARE_DYNCREATE(CCallStackOtherPage)

// Construction
public:
	CCallStackOtherPage();
	~CCallStackOtherPage();

	void setRadioSessionReference(long radioSessionRef);
	void setRadioEntityName(std::string radioEntityName);
	void onUpdateRights();
	
	void Init();
	
// Dialog Data
    //{{AFX_DATA(CCallStackOtherPage)
    enum { IDD = IDD_CALLSTACK_OTHER };
    //CListCtrl    m_OtherCallStack;
    //CRadioCallStackListCtrl    m_OtherCallStack(CRadioCallStackListCtrl::EStackType::OthersStack);
    TA_IRS_App::CRadioCallStackListCtrl    m_OtherCallStack;

	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCallStackOtherPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnCancel();
	afx_msg void OnOK();
    afx_msg void onItemChangedOtherCalllist(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCallStackOtherPage)
	virtual BOOL OnInitDialog();
	afx_msg void onKillCallButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};




#endif // __CALLSTACKOTHERPAGE_H__
