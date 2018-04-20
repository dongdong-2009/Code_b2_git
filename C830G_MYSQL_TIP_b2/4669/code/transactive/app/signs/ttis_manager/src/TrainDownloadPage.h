/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/ttis_manager/src/TrainDownloadPage.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This is the top level train download page.
  *
  */

#if !defined(AFX_TRAINDOWNLOADPAGE_H__E0B4209F_70BF_4F47_8A7C_B4652315C575__INCLUDED_)
#define AFX_TRAINDOWNLOADPAGE_H__E0B4209F_70BF_4F47_8A7C_B4652315C575__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrainDownloadPage.h : header file
//

#include "TrainDownloadTab.h"
#include "REBProgressManager.h"

/////////////////////////////////////////////////////////////////////////////
// CTrainDownloadPage dialog

namespace TA_IRS_App
{
    class CTrainDownloadPage : public CDialog
    {
    // Construction
    public:
        CTrainDownloadPage(CWnd* pParent = NULL);   // standard constructor

        void Init(REBProgressManager & mgr);

    // Dialog Data
        // {{AFX_DATA(CTrainDownloadPage)
        enum { IDD = IDD_TRAIN_DOWNLOAD };
        CTrainDownloadTab    m_trainDownloadTab;
        // }}AFX_DATA


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CTrainDownloadPage)
        public:
        virtual BOOL PreTranslateMessage(MSG* pMsg);
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:

        // Generated message map functions
        // {{AFX_MSG(CTrainDownloadPage)
            // NOTE: the ClassWizard will add member functions here
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()

        /**
         * OnOK
         *
         * This method has been implemented to hide accidental calls when
         * the Enter key is pressed. It can be overriden with new behaviour if required.
         */
        virtual afx_msg void OnOK();

        /**
         * OnCancel
         *
         * This method has been implemented to hide accidental calls when
         * the ESC key is pressed. It can be overriden with new behaviour if required.
         */
        virtual afx_msg void OnCancel();
    };

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} //TA_IRS_App

#endif // !defined(AFX_TRAINDOWNLOADPAGE_H__E0B4209F_70BF_4F47_8A7C_B4652315C575__INCLUDED_)
