/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/StationDvaStatusDlg.h $
  * @author Jade Lee
  * @version $Revision: #5 $
  * Last modification: $DateTime: 2014/05/16 18:00:29 $
  * Last modified by: $Author: qi.huang $
  * 
  * StationDvaStatusDlg handles GUI component of DVA station broadcast status dialog
  */

#if !defined(AFX_STATIONSTATUSDLG_H__1B97A351_62B5_41E4_94D5_450D78023FC3__INCLUDED_)
#define AFX_STATIONSTATUSDLG_H__1B97A351_62B5_41E4_94D5_450D78023FC3__INCLUDED_

#include "app/pa/pamanager/src/ListCtrlSort.h"
#include "app/pa/PAManager/src/DvaStatusPage.h"

class StationDvaStatusModel;
class DvaMessageChangeSubscriber;


class StationDvaStatusDlg : public DvaStatusPage
{
// Construction
public:

    typedef std::list<unsigned long>  MessageIdentifiers;

    /**
     * StationDvaStatusDlg (constructor)
     *
     * @param broadcastId broadcast ID for this status dialog (aka Status Name)
     *
     * @param dvaMessageChangeSubscriber reference to the subscriber listening out
     *          for DVA message udpates
     *
     * @param isOcc true if at OCC
     *
     */
    StationDvaStatusDlg( bool isOcc, const std::string& broadcastId, 
        DvaMessageChangeSubscriber& dvaMessageChangeSubscriber, CWnd* pParent = NULL );   // standard constructor

    virtual ~StationDvaStatusDlg();      

    /**
     * getStationDvaStatusModel
     *
     * Returns the model attached to this dialog
     *
     */
    StationDvaStatusModel&  getStationDvaStatusModel();

    /**
     * getStatusModelPointer
     *
     * @see DvaStatusPage::getStatusModelPointer base class definition for more details
     *
     */
    virtual GenericStatusModel* getStatusModelPointer();
  
    
    /**
     * onCurrentStatusNameDeleted
     *
     * Extends functionality of base class implementation (see that for more details)
     *
     */
    void    onCurrentStatusNameDeleted();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(StationDvaStatusDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
    

// Implementation
protected:

    // Returns list control showing broadcast failures
    CListCtrlSort& getFailuresListControl() { return m_failuresLst; }
    // Returns list control showing broadcast (success/progress)
    CListCtrlSort& getProgressListControl() { return m_progressLst; }
    
    /**
     * getNewBroadcastIdTextBoxRef (simple accessor)
     *
     */
    virtual CEdit& getNewBroadcastIdTextBoxRef() { return m_newBroadcastIdTb; }

    /**
     * getCurrentBroadcastIdTextBoxRef (simple accessor)
     *
     */
    virtual CEdit& getCurrentBroadcastIdTextBoxRef() { return m_currentBroadcastIdTb; }

    void removeAnnouncement();

	// Generated message map functions
    //{{AFX_MSG(StationDvaStatusDlg)
    virtual BOOL OnInitDialog();
    afx_msg LRESULT OnDvaMessageUpdate(UINT wParam, LONG lParam);
    afx_msg LRESULT OnSysCommand(UINT wParam, LONG lParam);
	afx_msg void OnButtonClose();
    afx_msg void OnButtonHelp();
	afx_msg void OnButtonRetryFailures();
	afx_msg void OnButtonRetryAll();
	afx_msg void OnButtonAbort();
	afx_msg void OnChangeNewStatusName();
    afx_msg void OnDestroy();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    /**
     * destroyStatusModel
     *
     * @see DvaStatusPage::destroyStatusModel base class definition for more details
     *
     */
    virtual void    destroyStatusModel();


    /**
     * getRetryAllButton (base class implementation, see for more details)
     */
    virtual CButton& getRetryAllButton() { return m_retryAllButton; }
    
    /**
     * getRetryFailedButton (base class implementation, see for more details)
     */
    virtual CButton& getRetryFailedButton() { return m_retryFailuresButton; }
    
    /**
     * getAbortButton (base class implementation, see for more details)
     */
    virtual CButton& getAbortButton() { return m_abortButton; }

    /**
     * getStatusTextListBox
     *
     * @see GenericStatusPage::getStatusTextListBox base class definition for more details
     *
     */
    virtual CListBox& getStatusTextListBox() { return m_statusAreaText; }

private:

    /**
     * fillOutBroadcastConfiguration
     *
     * Fills out the entries present in the DVA Broadcast Configuration area
     * The function queries the model (which will query the agent) to get
     * the relevant data, also updates selected messages list
     *
     * @return messages 
     */
    void    fillOutBroadcastConfiguration();
 
// Dialog Data
	//{{AFX_DATA(StationDvaStatusDlg)
	enum { IDD = IDD_STATION_BROADCAST_DVA_STATUS };
	CButton	m_retryFailuresButton;
	CButton	m_retryAllButton;
	CEdit	m_currentBroadcastIdTb;
	CEdit	m_newBroadcastIdTb;

// ********** Replaced #8854
//	CListCtrl	m_progressLst;
//	CListCtrl	m_failuresLst;
// **********  
	CListCtrlSort	m_progressLst;
	CListCtrlSort	m_failuresLst;

	CEdit	m_stopTb;
	CEdit	m_startTb;
	CEdit	m_priorityTb;
	CEdit	m_periodTb;
	CEdit	m_dwellTb;
	CEdit	m_chimeTb;
    CButton m_closeButton;
    CButton m_abortButton;
	int	 m_dwellSecs;
    CButton m_btnCycSun;
    CButton m_btnCycMon;
    CButton m_btnCycTue;
    CButton m_btnCycWed;
    CButton m_btnCycThu;
    CButton m_btnCycFri;
    CButton m_btnCycSat;
    CString m_chimeText;
    CString m_periodSecs;
	CString m_startTimeText;
	CString m_endTimeText;
    CListBox m_statusAreaText;
	//}}AFX_DATA

    StationDvaStatusModel* m_stationStatusModel;

    std::string m_openingBroadcastId;

    std::vector<unsigned long> m_vecDvaMsgKeys;

    // Only Station Dva Status dialogs need respond to ad-hoc message changes
    DvaMessageChangeSubscriber& m_dvaMessageChangeSubscriber;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATIONSTATUSDLG_H__1B97A351_62B5_41E4_94D5_450D78023FC3__INCLUDED_)
