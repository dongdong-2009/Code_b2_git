//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/ApplicationFont.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#ifndef FONTHELPER_H__INCLUDED_
#define FONTHELPER_H__INCLUDED_

/////////////////////////////////////////////////////////////////////////////
// FontHelper

class FontHelper
{
// Operations
public:
    static void adjustChildrenFont( HWND hWnd );
    ~FontHelper();

private:
    FontHelper();
    FontHelper(const FontHelper& applicationFont);
    FontHelper& operator=(const FontHelper& applicationFont);
};

#endif // !defined(AFX_APPLICATIONFONT_H__04B9F2D3_2804_4B3F_8EAF_FDF66CFCADDA__INCLUDED_)
