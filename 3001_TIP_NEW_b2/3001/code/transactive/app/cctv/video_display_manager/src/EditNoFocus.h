/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_display_manager/src/EditNoFocus.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
#if !defined(AFX_EDITNOFOCUS_H__C4809188_0304_4112_828D_9EC774DD08D0__INCLUDED_)
#define AFX_EDITNOFOCUS_H__C4809188_0304_4112_828D_9EC774DD08D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditNoFocus.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditNoFocus window

class CEditNoFocus : public CEdit
{
// Construction
public:
    CEditNoFocus();

// Attributes
public:

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CEditNoFocus)
    // }}AFX_VIRTUAL

// Implementation
public:
    virtual ~CEditNoFocus();

    // Generated message map functions
protected:
    // {{AFX_MSG(CEditNoFocus)
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    // }}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITNOFOCUS_H__C4809188_0304_4112_828D_9EC774DD08D0__INCLUDED_)
