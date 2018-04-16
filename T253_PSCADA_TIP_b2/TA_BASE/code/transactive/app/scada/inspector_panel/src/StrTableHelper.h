//  Copyright (C) 2005 Serge Wautier - appTranslator
//  
//  appTranslator - The ultimate localization tool for your Visual C++ applications
//                  http://www.apptranslator.com
//
//  This source code is provided 'as-is', without any express or implied
//  warranty. In no event will the author be held liable for any damages
//  arising from the use of this software.
//
//  Permission is granted to anyone to use this software for any purpose,
//  including commercial applications, and to alter it and redistribute it
//  freely, subject to the following restrictions:
//
//  1. The origin of this source code must not be misrepresented; you must not
//    claim that you wrote the original source code. If you use this source code
//    in a product, an acknowledgment in the product documentation and in the
//    About box is required, mentioning appTranslator and http://www.apptranslator.com
//
//  2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original source code.
//
//  3. This notice may not be removed or altered from any source distribution
//

//
// Msg.h: Declaration of the StringTableHelper and StringTableFormatHelper classes
//

#if !defined(INSPECTOR_PANEL_RESOURCE_HELPER_INCLUDED)
#define INSPECTOR_PANEL_RESOURCE_HELPER_INCLUDED

#include "app/scada/Inspector_Panel/src/stdafx.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_App
{
    //
    //   StringTableHelper : Wrapper around LoadString().
    //                       The index in the string table is passed to the c'tor.
    //                       Since StringTableHelper derives from CString, it has an LPCTSTR operator, hence can replace a string literal.
    //
    //          e.g.: AfxMessageBox(StringTableHelper(IDS_ERROR), MB_ICONERROR); // Error. Login Failed.
    //

    class StringTableHelper : public CString
    {
    public:
        StringTableHelper(UINT nID); // c'tor loads a string from the resource string table
    };

    //
    //   StringTableFormatHelper: Creates a formatted message. Wrapper around the API FormatMessage(). Kind of super-printf().
    //          Since StringTableFormatHelper derives from CString, it has an LPCTSTR operator, hence can replace a string literal.
    //
    //          e.g.: AfxMessageBox( StringTableFormatHelper(IDS_AGE, szName, nAge), MB_ICONINFORMATION ); 
    //                                     // IDS_AGE : %1 is %2!d! years old.
    //                                     // Message : Jane is 27 years old.
    //

    class StringTableFormatHelper : public CString
    {
    public:
        StringTableFormatHelper(LPCTSTR pszFormat, ...); // Format string can be a string literal...
        StringTableFormatHelper(UINT nFormatID, ...);    // ... or a string stored in the string table.
    };
} // namespace TA_Base_App


#endif
