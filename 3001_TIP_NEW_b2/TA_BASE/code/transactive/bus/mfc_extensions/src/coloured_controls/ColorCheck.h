/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/mfc_extensions/src/coloured_controls/ColorCheck.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
#if !defined(AFX_COLORCHECK_H__A897D857_09A3_11D6_8CAD_00500439B545__INCLUDED_)
#define AFX_COLORCHECK_H__A897D857_09A3_11D6_8CAD_00500439B545__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorCheck.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorCheck window

// Make this work in a DLL
#ifdef IMPORT_EXTENSIONS
class __declspec(dllimport) CColorCheck : public CButton
#else
class AFX_EXT_CLASS CColorCheck : public CButton
#endif
{
DECLARE_DYNAMIC(CColorCheck)

// Construction
public:

	CColorCheck();
	virtual ~CColorCheck();
	
    // BOOL Attach(const UINT nID, CWnd* pParent);	
	virtual void SetCheck(int nCheck);
	virtual int GetCheck();
	COLORREF SetBkColor(COLORREF color);
	COLORREF SetArrowColor(COLORREF newColor);
	COLORREF SetMyTextColor(COLORREF txtColor);

// Implementation
public:
	BOOL checkFlag;
	UINT oldAction;
	UINT oldState;
	BOOL drawFocus;
	COLORREF newColor;
    COLORREF newArrowColor;
    COLORREF newTextColor;

	// Generated message map functions
protected:
    virtual void PreSubclassWindow();
	void DrawCheckCaption(CDC *pDC, CRect R, const char *Buf, COLORREF TextColor, COLORREF TextShadowColor );
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItem);	//override the standard function (no WM_DRAWITEM !!!)
	//{{AFX_MSG(CColorCheck)
	//}}AFX_MSG
	
private:
    void drawBoder(CDC* pDC );
    void drawBoderLine(CDC* pDC, COLORREF color, int x1, int y1, int x2, int y2, int x3, int y3 );
    void drawArrow(CDC* pDC, COLORREF color );
    void drawArrowLine(CDC* pDC, int x1, int y1, int x2, int y2, int x3, int y3 );

private:

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORCHECK_H__A897D857_09A3_11D6_8CAD_00500439B545__INCLUDED_)
