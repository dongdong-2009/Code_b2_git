/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/ttis_manager/src/MainTab.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This creates and manages each of the three main tabs at the top of the app.
  *
  */

#if !defined(AFX_MAINTAB_H__AA6E12D4_2385_4AB9_B425_FF4D61EDEDF0__INCLUDED_)
#define AFX_MAINTAB_H__AA6E12D4_2385_4AB9_B425_FF4D61EDEDF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MainTab.h : header file
//

#include "TrainDisplayPage.h"
#include "TrainTimeSchedulePage.h"
#include "TrainDownloadPage.h"
#include "REBProgressManager.h"

#include "bus/mfc_extensions/src/tab_control_with_disable/TabCtrlWithDisableEx.h"
#include "bus/mfc_extensions/src/tab_dialog_control/TabDialogCtrl.h"

class REBProgressManager;
/////////////////////////////////////////////////////////////////////////////
// CMainTab window
namespace TA_IRS_App
{
    /**
     * Simple template class that holds a pointer to a dialog and an
     * indication if it is enabled or not.
     */
    template<typename Dialog> class DialogTabState
    {
    public:
        DialogTabState( const Dialog dialog, bool enabled )
            : m_dialog( dialog )
            , m_enabled( enabled )
        {
        };
        virtual ~DialogTabState()
        {
        };
        const Dialog m_dialog;
        bool isEnabled()
        {
            return this->m_enabled;
        };
        void setEnabled( bool enabled )
        {
            this->m_enabled = enabled;
        };
    private:
        bool m_enabled;
    };

    class CMainTab : public CTabDialogCtrl
    {
    // Construction
    public:
        CMainTab();

        void Init(REBProgressManager & mgr);

    // Operations
    public:

    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CMainTab)
        public:
        virtual BOOL PreTranslateMessage(MSG* pMsg);
        // }}AFX_VIRTUAL

        CTrainDisplayPage* getTrainDisplayPage();

    // Implementation
    public:
        virtual ~CMainTab();

        // Generated message map functions
    protected:
        virtual void PreSubclassWindow();
        // {{AFX_MSG(CMainTab)
            // NOTE - the ClassWizard will add and remove member functions here.
        afx_msg BOOL OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
        // }}AFX_MSG

        virtual  void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

        // override to draw text only; eg, colored text or different font
        virtual  void OnDrawText(CDC& dc, CRect rc, CString sText, BOOL bDisabled);

        DECLARE_MESSAGE_MAP()

    private:

        // TD16095
        // zhou yuna++
        int m_previousSelect;
        // ++zhou yuan

        DialogTabState<CTrainDisplayPage*> m_trainDisplayPage;
        DialogTabState<CTrainDownloadPage*> m_trainDownloadPage;
        DialogTabState<CTrainTimeSchedulePage*> m_trainTimeSchedulePage;
    };

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_MAINTAB_H__AA6E12D4_2385_4AB9_B425_FF4D61EDEDF0__INCLUDED_)
