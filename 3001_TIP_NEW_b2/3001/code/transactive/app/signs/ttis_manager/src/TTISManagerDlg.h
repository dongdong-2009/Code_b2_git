/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/ttis_manager/src/TTISManagerDlg.h $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * The top level dialog for the application
  *
  */

#if !defined(AFX_TISMANAGERDLG_H__D2FECDD9_F817_4CC8_8F95_4D4BF4E136AB__INCLUDED_)
#define AFX_TISMANAGERDLG_H__D2FECDD9_F817_4CC8_8F95_4D4BF4E136AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/utilities/src/RunParams.h"

#include "bus/generic_gui/src/TransActiveDialog.h"

#include "app/signs/ttis_manager/src/MainTab.h"


/////////////////////////////////////////////////////////////////////////////
// TTISManagerDlg dialog

namespace TA_IRS_App
{


    class TTISManagerDlg : public TA_Base_Bus::TransActiveDialog,
                           public TA_Base_Core::RunParamUser
    {
    // Construction
    public:
        TTISManagerDlg(TA_Base_Bus::IGUIAccess& controlClass);    // standard constructor

        virtual void onRunParamChange(const std::string& name, const std::string& value);

        void InitAll();

    // Dialog Data
        // {{AFX_DATA(TTISManagerDlg)
        enum { IDD = IDD_TISMANAGER_DIALOG };
        CMainTab    m_mainTab;
        // }}AFX_DATA

        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(TTISManagerDlg)
        public:
        virtual BOOL PreTranslateMessage(MSG* pMsg);
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        HICON m_hIcon;

        // Generated message map functions
        // {{AFX_MSG(TTISManagerDlg)
        virtual BOOL OnInitDialog();
        afx_msg void OnPaint();
        afx_msg HCURSOR OnQueryDragIcon();
        afx_msg void OnFileExit();
        afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
        afx_msg void OnClose();
        afx_msg void OnHelpAbout();
        afx_msg void OnHelp();
        afx_msg void OnCloseButton();
        afx_msg void OnShowWindow( BOOL bShow, UINT nStatus );
        afx_msg LRESULT OnInitTTIS( WPARAM wParam, LPARAM lParam);
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()

        CToolBar    m_wndToolBar;

    private:
        void initToolbar();
        // TD 15349
        // zhou yuan++
        std::string convertRunParam(const std::string& org);
        // zhou yuan++

        bool m_initReady;
    };

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_TISMANAGERDLG_H__D2FECDD9_F817_4CC8_8F95_4D4BF4E136AB__INCLUDED_)
