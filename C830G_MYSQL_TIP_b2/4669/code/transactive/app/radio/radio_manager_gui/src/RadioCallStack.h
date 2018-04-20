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
#if !defined(AFX_RADIOCALLSTACK_H__9D2F89A4_688E_11D7_B168_0050BAB1D931__INCLUDED_)
#define AFX_RADIOCALLSTACK_H__9D2F89A4_688E_11D7_B168_0050BAB1D931__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "bus\mfc_extensions\src\mfc_extensions.h"
#include "CallStackCurrentPage.h"
#include "CallStackOtherPage.h"
#include "CallStackHistoryPage.h"
#include "DynamicGroupPage.h"

namespace TA_Base_Core
{
	class RightsLibrary;
}

// RadioCallStack.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRadioCallStack window

class CRadioCallStack : public CTabDialogCtrl
{
// Construction
public:
	CRadioCallStack();

// Attributes
public:
	void setRadioSessionReference(long sessionRef);
	void setRadioEntityName(std::string radioEntityName);
	void onUpdateRights();

	void Init();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRadioCallStack)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRadioCallStack();

	CCallStackCurrentPage*		GetCurrentCallStack() { return &m_CurrentPage; };
	CCallStackOtherPage*		GetOtherCallStack()   { return &m_OtherPage;};
	CCallStackHistoryPage*		GetHistoryCallStack() { return &m_HistoryPage;};
	CDynamicGroupPage*			GetDynamicGroup()     { return &m_DynamicGroupPage;};
    
    /**
     * addDynamicGroupMembers
     *
     * Adds one or more members to the dynamic group box on the Dynamic Group Page
     *
     * @param member The members to add to the dynamic group page.
     */
    void addDynamicGroupMembers(std::vector<std::vector<CString> >* newMembers);

    /**
     * updateExistingDynamicGroupMembers
     *
     * Used to update trains in the dynamic group when details change
     */
    void updateExistingDynamicGroupMembers( CString& oldTsiNumber,
                                            CString& oldServiceNumber,
                                            CString& oldLocation,
                                            CString& newTsiNumber,
                                            CString& newServiceNumber,
                                            CString& newLocation);

	// Generated message map functions
protected:
	//{{AFX_MSG(CRadioCallStack)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	CCallStackCurrentPage		m_CurrentPage;
	CCallStackOtherPage			m_OtherPage;
	CCallStackHistoryPage		m_HistoryPage;
	CDynamicGroupPage           m_DynamicGroupPage;
    int                         m_DynamicGroupPageIndex;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RADIOCALLSTACK_H__9D2F89A4_688E_11D7_B168_0050BAB1D931__INCLUDED_)
