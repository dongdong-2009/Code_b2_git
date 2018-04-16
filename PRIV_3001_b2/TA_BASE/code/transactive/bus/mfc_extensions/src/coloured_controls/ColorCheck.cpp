/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/mfc_extensions/src/coloured_controls/ColorCheck.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// ColorCheck.cpp : implementation file
//
// Copyright (c) 2002 Radu Naiu
//
// Distribute and use freely, except:
// a) Don't alter or remove this notice.
// b) Mark the changes you make.
// This file is provided "as is" with no expressed or implied warranty.
// Use at your own risk. 
//
// History:
//
//      v1.0  24.01.2002  Initial release
//		v1.1  28.01.2002  BUG corrected: default checked state afeter initialization handled  

#include "bus/mfc_extensions/src/stdafx.h"
#include "bus/mfc_extensions/src/coloured_controls/ColorCheck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorCheck
IMPLEMENT_DYNAMIC(CColorCheck, CButton)

namespace
{
    const COLORREF CONST_COLOR_WINDOW     = GetSysColor( COLOR_WINDOW );
    const COLORREF CONST_COLOR_3DDKSHADOW = GetSysColor( COLOR_3DDKSHADOW );
    const COLORREF CONST_COLOR_3DSHADOW   = GetSysColor( COLOR_3DSHADOW );
    const COLORREF CONST_COLOR_3DHILIGHT  = GetSysColor( COLOR_3DHILIGHT );
    const COLORREF CONST_COLOR_3DLIGHT    = GetSysColor( COLOR_3DLIGHT );
    const COLORREF CONST_COLOR_BTNFACE    = GetSysColor( COLOR_BTNFACE );
}


CColorCheck::CColorCheck()
{
    checkFlag = FALSE;
    oldState = 0;
    oldAction = 0;
    drawFocus = FALSE;

    newColor = CONST_COLOR_WINDOW;
    newArrowColor = RGB(0,0,0);
    newTextColor = RGB(0,0,0);
}


CColorCheck::~CColorCheck()
{
}


BEGIN_MESSAGE_MAP(CColorCheck, CButton)
    //{{AFX_MSG_MAP(CColorCheck)
    //ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorCheck message handlers


void CColorCheck::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
    // CRect captionRect;
    CRect captionRect;
    captionRect.CopyRect(&lpDrawItemStruct->rcItem);
    CSize captionSize;

    // Retrieve the button's caption
    //
    const int bufSize = 512;
    TCHAR buffer[bufSize];
    GetWindowText(buffer, bufSize);

    //
    // Set the Text Position more to the left
    // Set the focus rectangle to just past the border decoration
    //
    captionSize = pDC->GetTextExtent(buffer, strlen(buffer));
    captionRect.SetRect( 17, 0, 17 + captionSize.cx + 3, captionSize.cy + 3 );

    UINT state = lpDrawItemStruct->itemState;
    UINT action = lpDrawItemStruct->itemAction;

    // Don't redraw whole control if only focus has changed
    if ( ODA_DRAWENTIRE & action )
    {
        drawBoder( pDC );

        if ( ODS_DISABLED & state )
        {
            pDC->FillSolidRect( 2, 2, 9, 9, CONST_COLOR_BTNFACE );

            if ( TRUE == checkFlag )
            {
                drawArrow( pDC, CONST_COLOR_3DSHADOW );
            }
            
            //draw the caption
            DrawCheckCaption( pDC, captionRect, buffer, CONST_COLOR_3DSHADOW, CONST_COLOR_3DHILIGHT );
        }
        else
        {
            pDC->FillSolidRect( 2, 2, 9, 9, newColor );

            if ( TRUE == checkFlag )
            {
                drawArrow( pDC, newArrowColor );
            }
            
            //draw the caption
            DrawCheckCaption( pDC, captionRect, buffer, newTextColor, CONST_COLOR_BTNFACE );

            if ( ODS_FOCUS & state && !( ODA_SELECT & oldAction ) )
            {
                DrawFocusRect(lpDrawItemStruct->hDC, (LPRECT)&captionRect);
            }
        }
    }
    else
    {
        if ( ODA_FOCUS == action )
        {
            if ( ( ODS_FOCUS & state ) ^ ( ODS_FOCUS & oldState ) )
            {
                DrawFocusRect(lpDrawItemStruct->hDC, (LPRECT)&captionRect);
            }
        }

        if ( ODA_SELECT & action )
        {
            //draw "checked" - arrow depending on state
            // mouse down / key down
            if ( !( ODS_SELECTED & oldState ) && ( ODS_SELECTED & state ) )
            {
                pDC->FillSolidRect( 2, 2, 9, 9, CONST_COLOR_WINDOW == newColor ? CONST_COLOR_BTNFACE : newColor );

                if ( TRUE == checkFlag )
                {
                    drawArrow( pDC, newArrowColor );
                }
            }
            // mouse up / key up
            else if ( ( ( ODA_SELECT & oldAction ) && ( ODS_SELECTED & oldState ) ) && !( ODS_SELECTED & state ) )
            {
                pDC->FillSolidRect(2, 2, 9, 9, newColor);

                if ( FALSE == checkFlag )
                {            
                    drawArrow( pDC, newArrowColor );
                }

                checkFlag = !checkFlag;
            }
        }
    }

    oldState = state;
    oldAction = action;
}


void CColorCheck::drawBoder(CDC* pDC )
{
    drawBoderLine( pDC, CONST_COLOR_3DDKSHADOW, 1, 10, 1, 1, 11, 1 );    // Line - UpRight - Inner
    drawBoderLine( pDC, CONST_COLOR_3DSHADOW, 0, 11, 0, 0, 12, 0 );      // Line - UpRight - Outter
    drawBoderLine( pDC, CONST_COLOR_3DHILIGHT, 12, 0, 12, 12, -1, 12 );  // Line - DownLeft - Outter
    drawBoderLine( pDC, CONST_COLOR_3DLIGHT, 11, 1, 11, 11, 0, 11 );     // Line - DownLeft - Inner
}


void CColorCheck::drawBoderLine(CDC* pDC, COLORREF color, int x1, int y1, int x2, int y2, int x3, int y3 )
{
    CPen normRect( PS_SOLID, 1, color );
    CPen* m_normRect = pDC->SelectObject( &normRect );
    
    CPoint startPt = pDC->MoveTo(x1, y1);
    pDC->LineTo( x2, y2 );
    pDC->LineTo( x3, y3 );

    normRect.DeleteObject();
}


void CColorCheck::drawArrow(CDC* pDC, COLORREF color )
{
    CPen normRect( PS_SOLID, 1, color );
    CPen* m_normRect = pDC->SelectObject( &normRect );

    drawArrowLine( pDC, 3, 5, 5, 7, 10, 2 );
    drawArrowLine( pDC, 3, 6, 5, 8, 10, 3 );
    drawArrowLine( pDC, 3, 7, 5, 9, 10, 4 );

    normRect.DeleteObject();
}


void CColorCheck::drawArrowLine(CDC* pDC, int x1, int y1, int x2, int y2, int x3, int y3 )
{
    CPoint startPt = pDC->MoveTo( x1, y1 );
    pDC->LineTo( x2, y2 );
    pDC->LineTo( x3, y3 );
}


void CColorCheck::DrawCheckCaption(CDC *pDC, CRect R, const char *Buf, COLORREF TextColor, COLORREF TextShadowColor )
{
    int prevBkMode = pDC->SetBkMode(TRANSPARENT);

    // draw shadow
    R.OffsetRect( 1, 1 );
    COLORREF prevColor = pDC->SetTextColor( TextShadowColor );
    pDC->DrawText(Buf, strlen(Buf), R, DT_CENTER|DT_VCENTER|DT_SINGLELINE);

    // draw text
    R.OffsetRect( -1, -1 );
    pDC->SetTextColor( TextColor );
    pDC->DrawText(Buf, strlen(Buf), R, DT_CENTER|DT_VCENTER|DT_SINGLELINE);

    // restore back
    pDC->SetBkMode(prevBkMode);
    pDC->SetTextColor(prevColor);
}


void CColorCheck::SetCheck(int nCheck)
{
    checkFlag = nCheck;
    Invalidate( TRUE );
}


BOOL CColorCheck::GetCheck()
{
    return ( TRUE == checkFlag );
}


COLORREF CColorCheck::SetBkColor(COLORREF boxColor)
{
    newColor = boxColor;
    return newColor;
}


COLORREF CColorCheck::SetArrowColor(COLORREF arrowColor)
{
    newArrowColor = arrowColor;
    return newArrowColor;
}


COLORREF CColorCheck::SetMyTextColor(COLORREF textColor)
{
    newTextColor = textColor;
    return newTextColor;
}


void CColorCheck::PreSubclassWindow()
{
    SetButtonStyle( GetStyle() | BS_OWNERDRAW );
    CButton::PreSubclassWindow();
}

