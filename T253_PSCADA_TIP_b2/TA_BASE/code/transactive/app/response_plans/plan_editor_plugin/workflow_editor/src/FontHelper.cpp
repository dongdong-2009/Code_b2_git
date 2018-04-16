/**
  *
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/FontHelper.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Set the font of all windows created by the current thread, message boxes excepted.
  * Instantiate with the name of the font face, point size and italic/bold properties.
  *
  **/

#include "stdafx.h"

#include <string>

#include "FontHelper.h"

/////////////////////////////////////////////////////////////////////////////
// FontHelper

static BOOL __stdcall setChildFont(HWND hWnd, LPARAM lparam)
{
    if (::IsWindow(hWnd) && (NULL != lparam))
    {
        ::SendMessage(hWnd, WM_SETFONT, static_cast<WPARAM>(lparam), TRUE);
    }

    return TRUE;
}

FontHelper::FontHelper()
{
}

FontHelper::~FontHelper()
{
}

void FontHelper::adjustChildrenFont( HWND hWnd )
{
    if ( NULL == hWnd || !::IsWindow( hWnd ))
    {
        return;
    }

    static HGDIOBJ hFont = NULL;

    if ( NULL == hFont )
    {
        static const char* const DEFAULT_FONT_NAME = "Arial";
        static const int DefaultSize = 11;
        static const bool DefaultBold = true;
        static const bool DefaultItalic = false;

        const unsigned long ulFontSize = (static_cast<unsigned long>(DefaultSize) << (sizeof(unsigned long)* 4u)) | (DefaultItalic ? 1u : 0u) | ((DefaultBold ? 1u : 0u) << 1);

        // No font name match, need to create a new font
        CWindowDC dcWin(NULL);
        int nHeight = -MulDiv( DefaultSize, ::GetDeviceCaps(dcWin.GetSafeHdc(), LOGPIXELSY), 72 );
        hFont = ::CreateFont( nHeight, 0, 0, 0, (DefaultBold ? FW_BOLD : FW_NORMAL), DefaultItalic, 0, 0, ANSI_CHARSET, 
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FF_DONTCARE | FF_DONTCARE, DEFAULT_FONT_NAME );
    }

    ::SendMessage( hWnd, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE );
    ::EnumChildWindows( hWnd, setChildFont, reinterpret_cast<LPARAM>(hFont) );
}
