/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/GroupPatchPage.h $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2010/05/10 17:22:58 $
  * Last modified by:  $Author: builder $
  *
  */
// GroupPatchPage.h : header file
//

#ifndef __GROUPPATCHPAGE_H__
#define __GROUPPATCHPAGE_H__

#pragma once

#include <string>
#include <vector>

#include "bus/pa/messagesubscriber/src/SimpleMessageSubscriber.h"
#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioCorbaDef.h"
#include "bus/mfc_extensions/src/auto_sort_list_ctrl/AutoSortListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSearchPage dialog

class CGroupPatchPage : public CDialog
{
	DECLARE_DYNCREATE(CGroupPatchPage)

// Construction
public:
    typedef TA_Base_Bus::IRadioCorbaDef::SubscriberDetailsNotification SubscriberNotification;

	CGroupPatchPage();
	~CGroupPatchPage();

	void onUpdateRights();
	void Init();

// Dialog Data
	//{{AFX_DATA(CGroupPatchPage)
	enum { IDD = IDD_FACILITYGROUPPATCH };
	CButton	m_removeButton;
	CButton	m_patchButton;
    TA_Base_Bus::AutoSortListCtrl  m_groupList;
	TA_Base_Bus::AutoSortListCtrl  m_patchList;
	//TD19766
	CToolTipCtrl   m_tip;     
	CWnd* pwnd;
	//TD19766
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGroupPatchPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);  //TD19766
	//}}AFX_VIRTUAL
	afx_msg void OnCancel();
	afx_msg void OnOK();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGroupPatchPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnPatch();
	afx_msg void OnRemove();
	afx_msg void OnDestroy();
	LRESULT OnSubscriberNotification(UINT wParam, LONG lParam);
	LRESULT OnAddItemToPatchGroup(UINT wParam, LONG lParam); // limin++ CL-21358
    //}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

    /**
     * @return true if there is a valid selection for creating a patch group
     */
    bool hasValidPatchSelection() const;	
	bool CanRemove(std::string patchrasubs);
	bool DeletePatchFromDB(std::string patchrasubs);
	bool insertPatchinDB(std::string displayName,std::string profileName);
	void UpdateGroups();
    void ResetCreateState();
	void ResetRemoveState();
	void ResetState();
//	void ClearMemory();
	void GetCurrentTime();
	std::string GetOwnerITSI(std::string patchname);
	std::string GetLocalProfileName();
	DWORD getEntityLocationKey();
	int findPatchIndexByTsi(std::string tsi);
	void updatePatchGroup();
	void initSortInfo();

	/**
     * processUpdate
     *
     * Processes a subscriber update notification
     * @param update - data defining the update
     */
    void processUpdate(const SubscriberNotification& update);
private:
	// Subscriber for User/Group subscriber updates
	TA_IRS_Bus::SimpleMessageSubscriber<SubscriberNotification>* m_messageSubscriber;      

	std::vector<char *>m_vAttachedData;
	std::vector<std::string>m_vRestoreGroup;
	std::vector<std::string>m_vRestoreTSI;
	std::vector<std::string> m_vlistPatchMem;
	std::vector<std::string> m_vlistPatchTsi;
//	char * sql;
	char timestamp[50];

	/**
    * getLocalDatabaseName
    *
    * returns the name of the local database
    */
    std::string getLocalDatabaseName();		//TD18317, change Radio form MView to AQ, modified by zhanghongzhi

};

#endif // __GROUPPATCHPAGE_H__
