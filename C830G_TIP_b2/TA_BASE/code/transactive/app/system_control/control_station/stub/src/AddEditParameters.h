/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/system_control/control_station/stub/src/AddEditParameters.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_ADDEDITPARAMETERS_H__BB103951_9330_4F58_B502_550AD35ED698__INCLUDED_)
#define AFX_ADDEDITPARAMETERS_H__BB103951_9330_4F58_B502_550AD35ED698__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddEditParameters.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AddEditParameters dialog

class AddEditParameters : public CDialog
{
// Construction
public:
	AddEditParameters(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AddEditParameters)
	enum { IDD = IDD_DIALOG1 };
	CString	m_name;
	CString	m_value;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AddEditParameters)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AddEditParameters)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDEDITPARAMETERS_H__BB103951_9330_4F58_B502_550AD35ED698__INCLUDED_)
