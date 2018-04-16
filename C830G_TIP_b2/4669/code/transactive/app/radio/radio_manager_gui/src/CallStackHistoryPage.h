/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/CallStackHistoryPage.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// CallStackHistoryPage.h : header file
//

#ifndef __CALLSTACKHISTORYPAGE_H__
#define __CALLSTACKHISTORYPAGE_H__


#include "core/data_access_interface/radio/src/RACallStackHistory.h"
#include "bus\mfc_extensions\src\list_ctrl_selection_without_focus\ListCtrlSelNoFocus.h"


/////////////////////////////////////////////////////////////////////////////
// CCallStackHistoryPage dialog

class CCallStackHistoryPage : public CDialog
{
	DECLARE_DYNCREATE(CCallStackHistoryPage)

// Construction
public:

	CCallStackHistoryPage();
	~CCallStackHistoryPage();

	void addCallToHistory(const long callID);
	void addCallToHistory(const time_t& timestamp,
						  TA_Base_Core::IRACallStackHistory::OriginType originType,
						  TA_Base_Core::IRACallStackHistory::CallType callType,
						  const std::string& identifier,
						  const std::string& location,
						  const std::string& itsi,
						  bool isEmergency); // td17294
	

	void onUpdateRights();

	void Init();
	/** 
      * addCallToStack
      *
      * Adds a call with the supplied information to the history stack.
      *
      * @param time       The time of the call
      * @param name       The name of the caller
      * @param origin     The origin of the call
	  * @param callType   The type of call
      *
      */
//	void addCallToStack(std::string time, std::string name, std::string origin,
//						CRadioSession::ERadioCallType callType);

// Dialog Data
	//{{AFX_DATA(CCallStackHistoryPage)
	enum { IDD = IDD_CALLSTACK_HISTORY };
	CComboBox	m_locationCombo;
	TA_Base_Bus::ListCtrlSelNoFocus	m_HistoryCallList; // td17294
	BOOL	m_inCheckBox;
	BOOL	m_outCheckBox;
	BOOL	m_confCheckBox;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCallStackHistoryPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	afx_msg void OnCancel();
	afx_msg void OnOK();

// Implementation
private:
	// Generated message map functions
	//{{AFX_MSG(CCallStackHistoryPage)
	virtual BOOL OnInitDialog();
	afx_msg void onOriginInChecked();
	afx_msg void onOriginOutChecked();
	afx_msg void onConferenceChecked();
	afx_msg void onLocationChanged();
	afx_msg void onRedialSelected();
	afx_msg void onRedialLastOutgoing();
	afx_msg void onRedialLastIncoming();
	afx_msg void onItemChangedHistoryCalllist(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void onDblclkHistoryCalllist(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	void getLocationAndProfile();
	void addCallToHistoryList(const time_t& timestamp,TA_Base_Core::IRACallStackHistory::OriginType originType,
							  TA_Base_Core::IRACallStackHistory::CallType callType,const std::string& identifier,
							  const std::string& location,bool isEmergency);
	void loadHistory(bool resetLocations=true);
	int  getCallCount(bool isOriginIn);
	void addExampleCalls();

	std::string  m_profile;
	unsigned int m_location;


	DECLARE_MESSAGE_MAP()

};


#endif // __CALLSTACKHISTORYPAGE_H__
