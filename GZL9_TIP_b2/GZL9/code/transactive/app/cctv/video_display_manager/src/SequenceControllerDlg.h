/**
  * The source code in this file is the property of
  * MI Consulting Group (Australia) and is not for
  * redistribution in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/SequenceControllerDlg.h $
  * @author Matthew Atkinson
  * @version $Revision: #1 $
  * Last Modification: $DateTime: 2012/06/12 13:35:44 $
  * Last Modified by:
  *
  * The SequenceControllerDlg class allows the user to halt, play, pause and navigate to
  * and fro along any give sequence of cameras. This class is not suitable for sub-classing.
  */

#if !defined(AFX_SEQUENCECONTROLLERDLG_H__0C85F44B_FD5A_11D4_B17C_0050BAB0C9A1__INCLUDED_)
#define AFX_SEQUENCECONTROLLERDLG_H__0C85F44B_FD5A_11D4_B17C_0050BAB0C9A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Application includes.
#include "Resource.h"
#include "TheToolTipCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// SequenceControllerDlg dialog

class SequenceControllerDlg : public CDialog
{
// Operations.
public:
    // Standard constructor
    SequenceControllerDlg( CWnd* pParent = NULL);


    void setMonitor(CString& monitor);

    void setSequence(CString& sequence);

// Dialog Data
    // {{AFX_DATA(SequenceControllerDlg)
    enum { IDD = IDD_SEQUENCECONTROLLER_DIALOG };
    CString    m_sequenceName;
    CString    m_monitorName;
    CButton    m_previousElement;
    CButton    m_play;
    CButton    m_nextElement;
    CButton    m_pause;
    // }}AFX_DATA

    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(SequenceControllerDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    // }}AFX_VIRTUAL


// Protected Operations.
protected:
    // Generated message map functions
    // {{AFX_MSG(SequenceControllerDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnOK();
    afx_msg void OnCancel();
    afx_msg void OnPreviousElementButton();
    afx_msg void OnPlayCheck();
    afx_msg void OnNextElementButton();
    afx_msg void OnPauseCheck();
    // }}AFX_MSG
    DECLARE_MESSAGE_MAP()


  /**
    * LoadButtonIcons.
    *
    * Load all the controller button icons.
    */

    void LoadButtonIcons();


  /**
    * SetToolTips.
    *
    * Set the tool tips for the dialog controller buttons.
    */

    void SetToolTips();


// Private data.
private:
    HICON m_hIcon;                                // Dialog icon.

    HICON m_previousElementIcon;
    HICON m_playIcon;
    HICON m_nextElementIcon;
    HICON m_pauseIcon;

    CTheToolTipCtrl m_ctlToolTip;                // For the dialog tool tips.
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEQUENCECONTROLLERDLG_H__0C85F44B_FD5A_11D4_B17C_0050BAB0C9A1__INCLUDED_)
