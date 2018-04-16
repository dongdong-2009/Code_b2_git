/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/ttis_manager/src/ValidatingEdit.h $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
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
        // Maximum number of characters possible for a free text message
        static const int MAX_FREE_TEXT_CHARACTERS;
        static const char* FREE_TEXT_INVALID_CHARACTERS;

    public:

        ValidatingEdit(CWnd* pParent = NULL){}   // standard constructor

        virtual ~ValidatingEdit(){}

        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(ValidatingEdit)
        public:
        virtual void PostNcDestroy();
        // }}AFX_VIRTUAL


        // Generated message map functions
        protected:
        // {{AFX_MSG(ValidatingEdit)
        afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
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
        void setDisallowedCharacters(CString disallowed);

    private:

        CString m_disallowed;
    };


} // TA_IRS_App

#endif
