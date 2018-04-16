/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Robin Ashcroft
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
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
#include "bus\generic_gui\src\TransActiveDialog.h"
#include "core\utilities\src\RunParams.h"
#include "resource.h"
#include "MainTab.h"

namespace TA_IRS_App
{

    class CSTISManagerDlg : public TA_Base_Bus::TransActiveDialog, public TA_Base_Core::RunParamUser
    {
    public:
	    bool ModifyWindowPos();
	    CSTISManagerDlg(TA_Base_Bus::IGUIAccess& controlClass);	// standard constructor
	    ~CSTISManagerDlg();
	    
        bool wantToShow() { return m_wantToShow; };

        /** 
          * onRunParamChange
          *
          * Called for a runparam change. selection and deselection.
          *
          */
	    void onRunParamChange( const std::string& name, const std::string& value );

		////haipeng added for the perfomance issue
		void InitAll();
		//haipeng added for the perfomance issue

    protected:

        // Dialog Data
	    //{{AFX_DATA(CSTISManagerDlg)
	    enum { IDD = IDD_STISMANAGER_DIALOG };
	    MainTab	m_mainTab;
	    //}}AFX_DATA


	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CSTISManagerDlg)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	    //}}AFX_VIRTUAL

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
	    afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg void OnActivateApp( BOOL bActive, DWORD hTask );
	    afx_msg void onHelpButton();
		afx_msg void onBtnClose();
		afx_msg void onClose();
		afx_msg void OnShowWindow( BOOL bShow, UINT nStatus );
		afx_msg LRESULT OnInitSTIS( WPARAM wParam, LPARAM lParam);
		afx_msg void OnAppExit();
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:

		/**
		 * Attempts to set the schematic into an active state
		 *  If fails, application must be automatically closed (for safety reasons)
		 * 
		 */
		void activateSchematic();

		//TD 15349
		//zhou yuan++
		std::string convertRunParam(const std::string& org);
		//zhou yuan++

		std::vector<std::string> tokenizeString(std::string theString, const std::string& separatorList );

        HICON m_hIcon;
	
		// Indicates whether the GUI is meant to be visible at the moment
		bool m_wantToShow;

        // Set to true once the application is first shown
        bool m_initialDisplay;

        PIDController m_pidController;

		bool m_initReady; //haipeng added
		//void offsetWindowPos(CWnd& wnd, long x, long y);//lkm
		//16350 libo 
		BOOL m_bMaxFlag;
		BOOL m_bFirstShow;
		//16350 libo
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_STISMANAGERDLG_H__30CA1571_AA66_49B1_B323_4D083B677043__INCLUDED_)
