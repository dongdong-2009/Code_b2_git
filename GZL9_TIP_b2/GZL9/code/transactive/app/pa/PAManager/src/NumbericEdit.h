/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/app/pa/PAManager/src/NumbericEdit.h $
  * @author Huang Qi
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * Handle paste message to avoid non numberic input
  *
  */
#ifndef NUMBERICEDIT_H__INCLUDED_
#define NUMBERICEDIT_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NumbericEdit.h : header file
//

#include <string>

/////////////////////////////////////////////////////////////////////////////
// NumbericEdit window

class NumbericEdit : public CEdit
{
// Construction
public:
	NumbericEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(NumbericEdit)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~NumbericEdit();

private:
    bool PrePaste();
    bool numberStringValid( const std::string& strNumber );

	// Generated message map functions
protected:
	//{{AFX_MSG(NumbericEdit)
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(NUMBERICEDIT_H__INCLUDED_)
