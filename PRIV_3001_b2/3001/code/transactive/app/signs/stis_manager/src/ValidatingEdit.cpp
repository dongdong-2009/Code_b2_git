/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_TIP/4669/transactive/app/signs/ttis_manager/src/ValidatingEdit.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/10/31 12:15:01 $
  * Last modified by:  $Author: grace.koh $
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
    BEGIN_MESSAGE_MAP( ValidatingEdit, CEdit )
        // {{AFX_MSG_MAP(ValidatingEdit)
        ON_WM_CHAR()
        // }}AFX_MSG_MAP
    END_MESSAGE_MAP()

    void ValidatingEdit::setAllowedCharacters( CString allowed )
    {
        FUNCTION_ENTRY( "setDisallowedCharacters" );

        m_allowed = allowed;

        FUNCTION_EXIT;
    }


    void ValidatingEdit::OnChar( UINT nChar, UINT nRepCnt, UINT nFlags )
    {
        FUNCTION_ENTRY( "OnChar" );

        // if the character is in the m_allowed string
        if( m_allowed.Find( nChar ) != -1 )
        {
            // allow it
            CEdit::OnChar( nChar, nRepCnt, nFlags );
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

