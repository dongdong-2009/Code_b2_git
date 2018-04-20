/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/DynamicGroupPage.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_DYNAMICGROUPPAGE_H__80D11B8E_C34E_454E_8718_192CAE1A2F68__INCLUDED_)
#define AFX_DYNAMICGROUPPAGE_H__80D11B8E_C34E_454E_8718_192CAE1A2F68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DynamicGroupPage.h : header file
//

#include "bus/trains/TrainSelectionComponent/src/MFCTrainSelectorGUI.h"
#include "bus/trains/TrainSelectionComponent/src/TrainSelectorGUI.h"
#include "bus/mfc_extensions/src/tab_control_with_disable/TabCtrlWithDisableEx.h"
#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"

using TA_IRS_Bus::TrainListCtrl;
using TA_IRS_Bus::TrainGroupComboBox;
using TA_Base_Bus::CTabCtrlWithDisableEx;
using TA_Base_Bus::ListCtrlSelNoFocus;

/////////////////////////////////////////////////////////////////////////////
// CDynamicGroupPage dialog

class CDynamicGroupPage : public CDialog,
                          public TA_IRS_Bus::ITrainSelectionListener
{
// Construction
public:
	CDynamicGroupPage(CWnd* pParent = NULL);   // standard constructor

	void onUpdateRights();

	void Init();
	
    /** 
      * trainSelectionChanged
      *
      * Gets called when the train selection is changed. (ITrainSelectionListener)
      *
      * @param trainsAreSelected   true if at least one train is selected
      */
    virtual void trainSelectionChanged(bool trainsAreSelected);

    /**
     * updateExistingDynamicGroupMembers
     * 
     * If this train is in the dynamic group, update the TSI
     */
    void updateExistingDynamicGroupMembers( CString& oldTsiNumber,
                                            CString& oldServiceNumber,
                                            CString& oldLocation,
                                            CString& newTsiNumber,
                                            CString& newServiceNumber,
                                            CString& newLocation);


// Dialog Data
	//{{AFX_DATA(CDynamicGroupPage)
	enum { IDD = IDD_DYNAMIC_GROUP };
	CButton	m_removeButton;
    CButton	m_resetButton;
    CButton	m_addButton;
	ListCtrlSelNoFocus	m_groupList;
    CTabCtrlWithDisableEx	m_locationTab;
	TrainGroupComboBox	m_selectionName;
	TrainListCtrl	m_outboundTrainList;
    TrainListCtrl	m_inboundTrainList;
    CStatic	m_inboundLabel;
    CStatic	m_outboundLabel;
    CButton	m_clearAllTrains;
	CButton	m_selectAllTrains;
    CButton	m_allInboundTrains;
	CButton	m_allOutboundTrains;
    CButton	m_saveTrainGroup;
	CButton	m_deleteTrainGroup;
	//}}AFX_DATA



// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDynamicGroupPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual afx_msg void OnCancel();
	virtual afx_msg void OnOK();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDynamicGroupPage)
	virtual BOOL OnInitDialog();
    afx_msg void OnDestroy();
	afx_msg void OnSavetrainselection();
	afx_msg void OnDeletetrainselection();
	afx_msg void OnAdd();
	afx_msg void OnReset();
	afx_msg void OnRemove();
	afx_msg void OnDblclkTrainList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void onAllTrains();
	afx_msg void onAllInboundTrains();
	afx_msg void onAllOutboundTrains();
	afx_msg void onClearAllTrains();
	afx_msg void onSelChangeLocationTab(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void onClickGroupList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void onItemChangedGroupList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
    afx_msg LRESULT OnInboundTrain(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnOutboundTrain(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnComboChanged(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnAddGroupMember(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:

    /**
     * setupTrainSelectionComponent sets up the train selection component area
     */
    void setupTrainSelectionComponent();

    /** 
      * enableButtons
      *
      * Gets called when the group list selection is changed,
      * train selection is changed, or when rights are updated
      */
    void enableButtons();


    CString isIDInGroupList(CString& tsi);
	void    addTestData();

    // rights - should anything on this page be enabled
    bool m_dynamicGroupPageEnabled;
    bool m_makeTrainCallEnabled;
    // whether any trains are selected
    bool m_trainsAreSelected;

    // train selection component
    TA_IRS_Bus::TrainSelectorGUI* m_trainSelectorGUI;
    // whether the train selector has been set
    bool m_trainSelectorSet;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DYNAMICGROUPPAGE_H__80D11B8E_C34E_454E_8718_192CAE1A2F68__INCLUDED_)
