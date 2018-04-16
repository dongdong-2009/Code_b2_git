/**
  * The source code in this file is the property of
  * MI Consulting Group (Australia) and is not for
  * redistribution in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_display_manager/src/RecordingUnitDlg.h $
  * @author Matthew Atkinson
  * @version $Revision: #2 $
  * Last Modification: $DateTime: 2012/02/06 16:15:14 $
  * Last Modified by:
  *
  * The RecordingUnitDlg class allows the user to record, play, rewind, fast forward,
  * and pause camera and sequence images. This class is not suitable for sub-classing.
  */

#if !defined(AFX_RECORDINGUNITDLG_H__7E0B74A5_F58B_11D4_B17C_0050BAB0C9A1__INCLUDED_)
#define AFX_RECORDINGUNITDLG_H__7E0B74A5_F58B_11D4_B17C_0050BAB0C9A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Application includes.
#include "Resource.h"
#include "TheToolTipCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// RecordingUnitDlg dialog

class RecordingUnitDlg : public CDialog
{
// Public operations.
public:
    // Standard constructor.
    RecordingUnitDlg(CString& recUnitName, short recordingUnitID, CString& monitorName,
                      short monitorID, CWnd* pParent = NULL);


// Dialog Data
    // {{AFX_DATA(RecordingUnitDlg)
    enum { IDD = IDD_RECORDINGUNIT_DIALOG };
    CString    m_recordingUnitName;
    CString    m_monitorName;
    CButton    m_stop;
    CButton    m_record;
    CButton    m_rewind;
    CButton    m_play;
    CButton    m_fastForward;
    CButton    m_pause;
    // }}AFX_DATA

    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(RecordingUnitDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    // }}AFX_VIRTUAL

public:

  /**
    * SetMonitor.
    *
    * Set the monitor name and ID.
    */

    void SetMonitor( CString& monitorName, short monitorID );

  /**
    * SetRecordingUnit.
    *
    * Set the recording unit name and ID.
    */

    void SetRecordingUnit( CString& recordingUnitName, short recordingUnitID );


// Protected operations.
protected:

    // Generated message map functions
    // {{AFX_MSG(RecordingUnitDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnOK();
    afx_msg void OnCancel();
    afx_msg void OnStop();
    afx_msg void OnRecord();
    afx_msg void OnRewind();
    afx_msg void OnPlay();
    afx_msg void OnFastForward();
    afx_msg void OnPause();
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
    * Set all the tool tips for the dialog controller buttons.
    */

    void SetToolTips();


// Private data.
private:
    HICON m_hIcon;                                // Dialog icon.

    // For the recording unit dialog controller buttons.
    HICON m_stopIcon;
    HICON m_recordIcon;
    HICON m_rewindIcon;
    HICON m_playIcon;
    HICON m_fastForwardIcon;
    HICON m_pauseIcon;

    CTheToolTipCtrl m_ctlToolTip;                // For the dialog tool tips.

    short m_recordingUnitID;                    // For the resource id of the recording unit.
    short m_monitorID;                            // For the resource id of the monitor.
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RECORDINGUNITDLG_H__7E0B74A5_F58B_11D4_B17C_0050BAB0C9A1__INCLUDED_)
