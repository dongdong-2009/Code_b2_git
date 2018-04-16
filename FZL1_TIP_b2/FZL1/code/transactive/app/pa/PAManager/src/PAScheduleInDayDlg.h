/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File$
  * @author Huang Qi
  * @version $Revision$
  * Last modification: $DateTime$
  * Last modified by: $Author$
  * 
  * subclass control of Pa Schedule in day dialog
  *
  */
#ifndef PABROADCASTLISTCTRL__INCLUDED_
#define PABROADCASTLISTCTRL__INCLUDED_

#include "app/pa/PAManager/src/ScheduleBroadcastModel.h"

namespace TA_Base_Core
{
    class IPaScheduleBroadcast;
}

class PAScheduleInDayDlg : public CDialog, public IScheduleBroadcastGUI
{
public:
	PAScheduleInDayDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~PAScheduleInDayDlg();
    void adjustWndPos();

private:
    void initialiseScheduleList();
    void onSelchangeListSchedules();
    virtual int insertScheduleToList( unsigned long ulKey, const std::string& strName, const std::string& strStartTime, const std::string& strStatus );
    virtual int updateBroadcastParamContent( const std::string& strMsg, const std::string& strCylicTimes, const std::vector<std::string>& vecZones );
    virtual int updateScheduleStateToList( unsigned long ulSchedulekey, const std::string& strStatus );
    virtual int deleteScheduleFromList( unsigned long ulSchedulekey );
    virtual unsigned long getScheduleKeyByListItemIndex( int nIndex );
    virtual void clearScheduleList();

public:
// Dialog Data
	enum { IDD = IDD_DLG_SCHEDULE_IN_DAY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    virtual BOOL OnInitDialog();
    virtual afx_msg void OnMoving( UINT unNoUse, LPRECT pRect );
    virtual afx_msg void OnCancel();
    virtual afx_msg void OnOK();
    virtual afx_msg void OnClose();
    virtual afx_msg void OnItemchangedListScheduleInDay(NMHDR *pNMHDR, LRESULT *pResult);
    virtual afx_msg void OnItemdeletedListScheduleInDay(NMHDR *pNMHDR, LRESULT *pResult);
    virtual afx_msg void OnClickedBtnDeleteScheduleInDay();
    virtual afx_msg void OnClickedBtnHide();
    virtual afx_msg LRESULT OnScheduleBroadcastUpdate( WPARAM wParam, LPARAM lParam );

	DECLARE_MESSAGE_MAP()

private:
    // Member variables
    ScheduleBroadcastModel* m_pModel;
    CFont m_fntList;

    // Controls
    CListCtrl m_lstScheduleBroadcasts;
    CListBox m_lbZones;
    CButton m_btnDelSchedule;
};

#endif // ifndef PABROADCASTLISTCTRL__INCLUDED_