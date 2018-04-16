/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_TIP/4669/transactive/app/signs/ttis_manager/src/ValidatingEdit.h $
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

#if !defined(VALIDATING_EDIT_H)
#define VALIDATING_EDIT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_IRS_App
{
    class ValidatingEdit : public CEdit
    {
    public:

        ValidatingEdit( CWnd* pParent = NULL ) {} // standard constructor

        virtual ~ValidatingEdit() {}

        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(ValidatingEdit)
    public:
        virtual void PostNcDestroy();
        // }}AFX_VIRTUAL


        // Generated message map functions
    protected:
        // {{AFX_MSG(ValidatingEdit)
        afx_msg void OnChar( UINT nChar, UINT nRepCnt, UINT nFlags );
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()


    public:

        /**
          * setDisallowedCharacters
          *
          * Any character in this string is prevented from being
          * entered in the box.
          *
          * @param disallowed A string of invalid characters
          */
        void setAllowedCharacters( CString allowed );

    private:

        CString m_allowed;
    };


} // TA_IRS_App

#endif
