/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/ttis_manager/src/TrainDisplayTab.h $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * The tab that holds the pre-defined and free text message lists
  *
  */

#if !defined(AFX_TRAINDISPLAYTAB_H__DDB5049C_E10E_4C2E_86D5_6D057E900636__INCLUDED_)
#define AFX_TRAINDISPLAYTAB_H__DDB5049C_E10E_4C2E_86D5_6D057E900636__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrainDisplayTab.h : header file
//

#include "bus\mfc_extensions\src\tab_dialog_control\TabDialogCtrl.h"
#include "TrainSelectFreeTextPage.h"
#include "TrainSelectPredefinedPage.h"
#include "app/signs/ttis_manager/src/IMessageSelectionListener.h"


/////////////////////////////////////////////////////////////////////////////
// CTrainDisplayTab window

namespace TA_IRS_App
{
    class CTrainDisplayTab : public CTabDialogCtrl
    {
    // Construction
    public:
        CTrainDisplayTab();

        virtual ~CTrainDisplayTab();

        void Init();

        /**
          * getPredefinedPage
          *
          * get a reference to the pre-defined message page
          *
          * @return a reference to the pre-defined message page
          */
        CTrainSelectPredefinedPage& getPredefinedPage();


        /**
          * getFreeTextPage
          *
          * get a reference to the free text page
          *
          * @return a reference to the free text page
          */
        CTrainSelectFreeTextPage& getFreeTextPage();

        // TD 15349
        // zhou yuan++
        bool findAndSelectStationMessage(const std::string& messageName);
        void setMessageSelectionListener(IMessageSelectionListener* messageSelectionListener);
        // ++zhou yuan

    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CTrainDisplayTab)
        // }}AFX_VIRTUAL

    // Implementation

        // Generated message map functions
    protected:
        // {{AFX_MSG(CTrainDisplayTab)
        afx_msg BOOL OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
        // }}AFX_MSG

        DECLARE_MESSAGE_MAP()

    private:
        CTrainSelectPredefinedPage m_predefinedPage;
        CTrainSelectFreeTextPage m_trainSelectFreeTextPage;

    };

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_TRAINDISPLAYTAB_H__DDB5049C_E10E_4C2E_86D5_6D057E900636__INCLUDED_)
