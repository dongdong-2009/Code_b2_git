/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/stis_manager/src/STISManagerDlg.h $
  * @author:  Robin Ashcroft
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Main Page for STIS manager
  *
  */

#if !defined(AFX_STISMANAGERDLG_H__30CA1571_AA66_49B1_B323_4D083B677043__INCLUDED_)
#define AFX_STISMANAGERDLG_H__30CA1571_AA66_49B1_B323_4D083B677043__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/signs/stis_manager/src/PIDController.h"
#include "bus/generic_gui/src/TransActiveDialog.h"
#include "core/utilities/src/RunParams.h"
#include "resource.h"
#include "MainTab.h"

namespace TA_IRS_App
{

    class CSTISManagerDlg : public TA_Base_Bus::TransActiveDialog, public TA_Base_Core::RunParamUser
    {
    public:
	    CSTISManagerDlg(TA_Base_Bus::IGUIAccess& controlClass);	// standard constructor
	    ~CSTISManagerDlg();
	    void onRunParamChange( const std::string& name, const std::string& paramvalue );

		void InitAll();
    protected:

        // Dialog Data
	    enum { IDD = IDD_STISMANAGER_DIALOG };
	    MainTab	m_mainTab;
	    //{{AFX_VIRTUAL(CSTISManagerDlg)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

        /**
         * OnSetWindowPosition (virtual extension)
         *
         * This method is called whenever GenericGui gets a new window position
         * from control station
         * 
         */
        virtual afx_msg LRESULT OnSetWindowPosition(WPARAM wParam, LPARAM lParam);

	    // Generated message map functions
	    //{{AFX_MSG(CSTISManagerDlg)
	    virtual BOOL OnInitDialog();
	    afx_msg void OnPaint();
	    afx_msg HCURSOR OnQueryDragIcon();
	    afx_msg void OnFileExit();
		afx_msg void OnHelpAbout();
	    afx_msg void OnHelpStismanagerhelp();
	    afx_msg void onHelpButton();
		afx_msg void onBtnClose();
		afx_msg void OnShowWindow( BOOL bShow, UINT nStatus );
		afx_msg void OnAppExit();
		virtual void OnClose();
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:

		/**
		 * Attempts to set the schematic into an active state
		 *  If fails, application must be automatically closed (for safety reasons)
		 * 
		 */
		void activateSchematic();

		std::string convertRunParam(const std::string& org);
		std::vector<std::string> tokenizeString(std::string theString, const std::string& separatorList );
		bool ModifyWindowPos(void);
        HICON m_hIcon;
        PIDController m_pidController;
		bool m_initReady;
		bool m_shouldRepostionDialog;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_STISMANAGERDLG_H__30CA1571_AA66_49B1_B323_4D083B677043__INCLUDED_)
