/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/RadioFacilities.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_RADIOFACILITIES_H__E4EB6A90_6958_11D7_B169_0050BAB1D931__INCLUDED_)
#define AFX_RADIOFACILITIES_H__E4EB6A90_6958_11D7_B169_0050BAB1D931__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "DirectLine.h"
#include "DirectLineConfigurePage.h"
#include "bus\mfc_extensions\src\mfc_extensions.h"
#include "bus\radio\radiosearchpage\src\searchpage.h"
#include "GroupPatchPage.h"

// RadioFacilities.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRadioFacilities window

class CRadioFacilities : public CTabDialogCtrl
{
// Construction
public:
	CRadioFacilities();

// Attributes
public:

// Operations
public:
	void Init();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRadioFacilities)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRadioFacilities();

	CDirectLinePage* getDirectLinePage() { return &m_DirectLinePage; }
    CDirectLineConfigurePage* getDirectLineConfigurePage() { return &m_ConfigurePage; }

	void onUpdateRights();
    
	// Generated message map functions
protected:
	//{{AFX_MSG(CRadioFacilities)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

	TA_IRS_Bus::CSearchPage			m_SearchPage;
	CDirectLinePage				m_DirectLinePage;
	CDirectLineConfigurePage	m_ConfigurePage;
	CGroupPatchPage             m_groupPatchPage;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RADIOFACILITIES_H__E4EB6A90_6958_11D7_B169_0050BAB1D931__INCLUDED_)
