/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/scada/DataNodeController/src/SummaryListCtrl.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#if !defined(AFX_SUMMARYLISTCTRL_H__87B9733C_E28E_43F5_BF38_50DDCB643B53__INCLUDED_)
#define AFX_SUMMARYLISTCTRL_H__87B9733C_E28E_43F5_BF38_50DDCB643B53__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SummaryListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSummaryListCtrl window

class CSummaryListCtrl : public CListCtrl
{
// Construction
public:
	CSummaryListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSummaryListCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	void InitialiseList();
	virtual ~CSummaryListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSummaryListCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUMMARYLISTCTRL_H__87B9733C_E28E_43F5_BF38_50DDCB643B53__INCLUDED_)
