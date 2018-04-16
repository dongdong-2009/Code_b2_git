/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/reporting/crystal_reports_viewer/src/crutilDlg.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// crutilDlg.h : header file
//
//{{AFX_INCLUDES()
#include "crystalreportviewer9.h"
#include "webbrowser2.h"
//}}AFX_INCLUDES

#if !defined(AFX_CRUTILDLG_H__1F8F7DE9_746D_4CFC_8E51_04EE1216D238__INCLUDED_)
#define AFX_CRUTILDLG_H__1F8F7DE9_746D_4CFC_8E51_04EE1216D238__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CrystalReportsHelper.hpp"
#include "StatusBarWithProgress.h"

#ifdef TRANSACTIVE_BUILD
#include "bus/generic_gui/src/TransActiveDialog.h"

namespace TA_Base_Bus
{
    class IGUIAccess;
}

namespace TA_Base_App
{
    namespace TA_Reporting
    {


#endif // TRANSACTIVE_BUILD

		/////////////////////////////////////////////////////////////////////////////
		// CCrutilDlg dialog

		#ifdef TRANSACTIVE_BUILD
		class CCrutilDlg : public TA_Base_Bus::TransActiveDialog
		#else
		class CCrutilDlg : public CDialog
		#endif
		{
		// Construction
		public:
			bool m_bViewHistory;
			#ifdef TRANSACTIVE_BUILD
			CCrutilDlg(TA_Base_Bus::IGUIAccess* genericGUICallback, CWnd* pParent = NULL);	// standard constructor
			#else
			CCrutilDlg(CWnd* pParent = NULL);	// standard constructor
			#endif
			~CCrutilDlg();
			CString m_cstrReportName;
			CrystalReportsHelper* m_pCrystalReports;
			bool m_bShowMaxWindow;

			// true for show toolbar, false for not.
			bool m_bShowToolBar;
			// true for show statusbar, false for not.
			bool m_bShowStatusBar;

			//TD18095, jianghp, to fix the performance of showing manager application
			virtual void init();
		// Dialog Data
			//{{AFX_DATA(CCrutilDlg)
			enum { IDD = IDD_CRUTIL_DIALOG };
			CCrystalReportViewer9	m_ReportViewer;
			CWebBrowser2	m_HistoryReportViewer;

			// Because this Crystal Reports Viewer component does not currently support
			// a close report method. Consequently, we create another objects of CrystalReportHelpler
			// and CCrystalReportViewer9, initlize them, and use it as an blank CrystalReport viewer.
			// Usually these objects will be set to "hide", but when user click "Close" menu item, these 
			// objects will be set to "show", also in this case, the m_pCrystalReports and m_ReportViewer
			// will set to "hide".
			CrystalReportsHelper* m_pCrystalReportsBlank;
			CCrystalReportViewer9	m_ReportViewerBlank;

			// store the file state: closed or not.
			bool m_bFileClosed;
			//}}AFX_DATA

			// Statusbar with a progress control inside it.
			CStatusBarWithProgress m_ProgressCtrl;

			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CCrutilDlg)
			protected:
			virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
			//}}AFX_VIRTUAL

		/**
            * repaintDlgAfterOperate
            *
            * This is called when user operated the ReportManager, like use the original way to open report
            * file from ReportManager's menu, or double click the report file node on the report tree, etc.
            * For that Crysta lReport Viewer or Web Viewer will mess up ReportManager's HMI after these 
			* actions, we need to call repaintDlgAfterOperate after that.
            */
		//	void repaintDlgAfterOperate();
		// Implementation


		protected:
			HICON m_hIcon;

			// Generated message map functions
			//{{AFX_MSG(CCrutilDlg)
			virtual BOOL OnInitDialog();
			afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
			afx_msg void OnPaint();
			afx_msg HCURSOR OnQueryDragIcon();
            afx_msg void OnSize(UINT nType, int cx, int cy);
			afx_msg void OnFileExit();
			afx_msg void OnHelpAbout();
			afx_msg void OnFileOpen();
			afx_msg void OnFileClose();
			afx_msg void OnReportManagerHelp();
			afx_msg void OnUpdateShowToolBar(CCmdUI* pCmdUI);
			afx_msg void OnUpdateShowStatusBar(CCmdUI* pCmdUI);
			afx_msg void OnDestroy();
			afx_msg void OnViewHistory();
			afx_msg void OnViewRefresh();
			afx_msg void OnClose();
			afx_msg void OnTimer (UINT TimerVal); // for progress control bar's step
	afx_msg void OnPrintButtonClicked(BOOL FAR* UseDefault);
	afx_msg void OnRefreshButtonClicked(BOOL FAR* UseDefault);//cl-18123--lkm
	afx_msg void OnUpdateViewHistory(CCmdUI* pCmdUI);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
			afx_msg LRESULT OnKickIdle(WPARAM, LPARAM);	
			afx_msg void OnUpdateKeyIndicator(CCmdUI* pCmdUI);

			DECLARE_MESSAGE_MAP()
		private:
			void saveWindowSize();
			void restoreWindowSize();
			void displayAboutBox();

			// the fucntion spectified how to step the progress bar
			void stepProgressBar( int _currentPos);
            
			// called by OnSize, use to set the correct position of CrystalReport viewer and Statusbar.
			void repaintCurrentDialog( UINT nType, int cx, int cy );
			void ensureStatusBarPositionCorrect();
		};

#ifdef TRANSACTIVE_BUILD
	} // TA_Reporting
} // TA_Base_App
#endif // TRANSACTIVE_BUILD

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CRUTILDLG_H__1F8F7DE9_746D_4CFC_8E51_04EE1216D238__INCLUDED_)
