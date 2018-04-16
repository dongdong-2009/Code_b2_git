/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Huang Qi
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
#ifndef OTHERDVAMSGLISTPAGE_H__INCLUDED_
#define OTHERDVAMSGLISTPAGE_H__INCLUDED_

#include "app/pa/PAManager/src/PaDVAMsgListCtrl.h"
#include "app/pa/PAManager/src/ICurSelDVAMsgObserver.h"

class PaRuntimeModel;

class OtherDVAMsgListPage : public CPropertyPage, public ICurSelDVAMsgObserver
{
	DECLARE_DYNAMIC(OtherDVAMsgListPage)

public:
	OtherDVAMsgListPage( PaRuntimeModel& refBroadcastModel );
    virtual ~OtherDVAMsgListPage();
    
    virtual void processDVAMsgSelChanged( int nType );

    std::string getCaption();
    unsigned int getTemplateId();
    void enableEditDVAMsgDescription( bool bEnable = true );
    void searchDVAMsg();

private:
    OtherDVAMsgListPage();
    const OtherDVAMsgListPage& operator=( const OtherDVAMsgListPage& );
    OtherDVAMsgListPage( const OtherDVAMsgListPage& );

// Dialog Data
	enum { IDD = IDD_PROPPAGE_OTHER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
    BOOL OnInitDialog();
    virtual afx_msg void OnBnClickedBtnListAll();
    virtual afx_msg void OnBnClickedOtherDesEdit();
    virtual afx_msg void OnEnChangeSearchKeyword();
    // wParam : ItemData, lParam : retry times (can retry once)
    virtual afx_msg LRESULT OnSelQuickBroadcastDVAMsg( WPARAM wParam, LPARAM lParam );
    virtual afx_msg LRESULT OnUpdateDVAMsgList( WPARAM wParam, LPARAM lParam );

private:
    CButton m_btnDVADescEdit;
    CEdit m_edBroadcastDes;
    PaDVAMsgListCtrl m_slcBroadcastList;
    bool m_bEnableDVAMsgDescEdit;
    PaRuntimeModel& m_refBroadcastModel;
};

#endif // OTHERDVAMSGLISTPAGE_H__INCLUDED_
