/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_log_viewer/src/TisLogViewerDlg.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  *
  */

#if !defined(AFX_TISLOGVIEWERDLG_H__B1FC56F1_91F3_4680_9193_F5B9A34FE350__INCLUDED_)
#define AFX_TISLOGVIEWERDLG_H__B1FC56F1_91F3_4680_9193_F5B9A34FE350__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/signs/tis_log_viewer/src/DisplayLogListCtrl.h"
#include "app/signs/tis_log_viewer/src/PidDisplayDialog.h"
#include "app/signs/tis_log_viewer/src/ReinstateDisplayRequestDialog.h"
#include "app/signs/tis_log_viewer/src/IRequestSelectionListener.h"
#include "app/signs/tis_log_viewer/src/ILoadProgressDisplay.h"

#include "bus/generic_gui/src/TransActiveDialog.h"
#include "core/data_access_interface/tis_agent/src/IDisplayRequest.h"

/////////////////////////////////////////////////////////////////////////////
// TisLogViewerDlg dialog


namespace TA_Base_Bus
{
    class IGUIAccess;
}

namespace TA_IRS_App
{
    class DisplayRequestSender;
}

namespace TA_IRS_App
{

    class TisLogViewerDlg : public TA_Base_Bus::TransActiveDialog,
                            public IRequestSelectionListener,   // notified of pid selection
                            public ILoadProgressDisplay         // notified of load progress
    {
    // Construction
    public:

	    TisLogViewerDlg(TA_Base_Bus::IGUIAccess& genericGUICallback,CWnd* pParent = NULL);	// standard constructor

        ~TisLogViewerDlg();

		void init();
        /** 
          * requestSelectionChanged
          *
          * Gets called when a display request is selected or deselected.
          *
          * @param requestType     The type of display request that is selected.
          * @param isReinstatable  True if the display request can be reinstated, false otherwise.
          */
        virtual void requestSelectionChanged(RequestType requestType, bool isReinstatable);


        /** 
          * showProgressBar
          *
          * Called to show the progress bar.
          * If the bar is being shown, initialise it with some values.
          *
          * @param show true if the bar should be shown
          * @param min  the minimum value (0 usually does fine)
          * @param max  the maximum value
          * @param step the amount to increment
          * @param pos  the initial position of the bar (between min and max).
          */
        virtual void showProgressBar( bool show,
                                      int min = 0,
                                      int max = 100,
                                      int step = 1,
                                      int pos = 0);


        /** 
          * stepProgressBar
          *
          * Increment the progress bar position
          */
        virtual void stepProgressBar();


		//resize the window
		//void ModifyWindowPos();


        // Dialog Data
	    //{{AFX_DATA(TisLogViewerDlg)
	    enum { IDD = IDD_TISLOGVIEWER_DIALOG };
	    CProgressCtrl	m_progressBar;
	    CEdit	m_messageContent;
	    DisplayLogListCtrl	m_requestList;
    	//}}AFX_DATA

	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(TisLogViewerDlg)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	    //}}AFX_VIRTUAL

    protected:
	    // Generated message map functions
	    //{{AFX_MSG(TisLogViewerDlg)
	    virtual BOOL OnInitDialog();
        afx_msg void OnFileExit();
	    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	    afx_msg void OnClose();
	    afx_msg void OnHelpAbout();
        afx_msg void OnHelp();
	    afx_msg void OnCloseButton();
	    afx_msg void OnViewPids();
	    afx_msg void OnReinstate();
        afx_msg LRESULT onRightsChanged(WPARAM wParam, LPARAM lParam);
        afx_msg void OnDestroy();
		afx_msg void OnClickRequestList(NMHDR* pNMHDR, LRESULT* pResult); // TD14405 
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:      

        // the dialog that displays the destination PIDs
        PidDisplayDialog m_pidDisplayDialog;

        ReinstateDisplayRequestDialog* m_reinstateDisplayRequestDialog;

        // rights
        bool m_canDisplayTTISFreeText;
        bool m_canDisplayTTISPredefined;
        bool m_canDisplaySTISFreeText;
        bool m_canDisplaySTISPredefined;
        bool m_canSubmitRatis;

        HICON m_hIcon;

        // current request selection
        IRequestSelectionListener::RequestType m_currentSelection;

        // the status of the progress bar.
        // IsWindowVisible wasnt behaving as expected
        bool m_progressBarVisible;

        // True if the currently selected display request is reinstatable.
        bool m_isReinstatable;  
        
        unsigned long m_locationKey;    // TES891

        DisplayRequestSender* m_displayRequestSender;
    };

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_App

#endif // !defined(AFX_TISLOGVIEWERDLG_H__B1FC56F1_91F3_4680_9193_F5B9A34FE350__INCLUDED_)
