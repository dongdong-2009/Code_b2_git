#pragma once

#include "app/pa/PAManager/src/PaScheduleBroadcastListCtrl.h"

// ScheduleBroadcastDlg dialog

class PaScheduleManager;

class ScheduleBroadcastDlg : public CDialog
{
	DECLARE_DYNAMIC(ScheduleBroadcastDlg)

public:
	ScheduleBroadcastDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ScheduleBroadcastDlg();

    void setOperationMode( int nMode );
    void setScheduleTime( SYSTEMTIME& stStartTime, SYSTEMTIME& stEndTime );

// Dialog Data
    enum { IDD = IDD_DLG_SCHEDULE_BROADCAST };

protected:
    int m_nOperationMode;
    CTime m_tmStartTimeEdit;
    CTime m_tmEndTimeEdit;

protected:
	virtual BOOL OnInitDialog();
    virtual afx_msg void OnBnClickedBtnConfirm();
    virtual afx_msg void OnBnClickedBtnCancel();

    void InitialiseComboBox();
    void updateToViewMode();
    void updateEditModeCtrls();
    void insertToList();
    void modifyExistInList();
    void initialiseScheduleList();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

protected:
    PaScheduleBroadcastListCtrl m_lsBroadcastSchedule;
    CEdit m_edScheduleName;
    CButton m_btnConfirm;
    CButton m_btnCancel;
    CButton m_btnDeleteSchedule;
    CEdit m_edBroadcastMsg;
    CEdit m_edBroadcastZones;
};
