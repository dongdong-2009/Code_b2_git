/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/ttis_manager/src/ValidatingEdit.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This disallows specified characters.
  * Other than that it is a regular CEdit
  *
  */

#include "stdafx.h"
#include "ValidatingEdit.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{
    const int ValidatingEdit::MAX_FREE_TEXT_CHARACTERS = 124;
    const char* ValidatingEdit::FREE_TEXT_INVALID_CHARACTERS = "@[]^`~";

    BEGIN_MESSAGE_MAP(ValidatingEdit, CEdit)
    // {{AFX_MSG_MAP(ValidatingEdit)
    ON_WM_CHAR()
    // }}AFX_MSG_MAP
    END_MESSAGE_MAP()

    void ValidatingEdit::setDisallowedCharacters(CString disallowed)
    {
        FUNCTION_ENTRY( "setDisallowedCharacters" );

        m_disallowed = disallowed;

        FUNCTION_EXIT;
    }


    void ValidatingEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
    {
        FUNCTION_ENTRY( "OnChar" );

        // if the character is not in the string
        if( m_disallowed.Find(nChar) == -1 )
        {
            // allow it
            CEdit::OnChar(nChar, nRepCnt, nFlags);
        }
        // otherwise do nothing

        FUNCTION_EXIT;
    }


    void ValidatingEdit::PostNcDestroy()
    {
        FUNCTION_ENTRY( "PostNcDestroy" );

        CEdit::PostNcDestroy();

        FUNCTION_EXIT;
    }


} // TA_IRS_App

