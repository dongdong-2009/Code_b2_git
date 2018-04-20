/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
// CallStackPage.h : header file
//

#ifndef __CALLSTACKCURRENTPAGE_H__
#define __CALLSTACKCURRENTPAGE_H__

#include "app/radio/radio_manager_gui/src/resource.h"
#include "app/radio/radio_manager_gui/src/RadioCallStackListCtrl.h"
#include "core/utilities/src/RunParams.h"
#include <string>


/////////////////////////////////////////////////////////////////////////////
// CCallStackCurrentPage dialog

class CCallStackCurrentPage : public CDialog, public TA_Base_Core::RunParamUser
{
	DECLARE_DYNAMIC(CCallStackCurrentPage)

// Construction
public:

	CCallStackCurrentPage();
	~CCallStackCurrentPage();

	void onUpdateRights();
	void setRadioSessionReference(long radioSessionRef);
	void setRadioEntityName(std::string radioEntityName);

	void Init();
	
	/**
	 * @return true if the answer button should be enabled - which means
	 *	have the right to answer a call a call is currently selected
	 */
	bool getShouldEnableAnswerButton();

// Dialog Data
    //{{AFX_DATA(CCallStackCurrentPage)
    enum { IDD = IDD_CALLSTACK_CURRENT };
    CRichEditCtrl    m_richedtTIP;
    CButton    m_acceptButton;
    CButton    m_recallButton;
    TA_IRS_App::CRadioCallStackListCtrl    m_incomingCallList;
    TA_IRS_App::CRadioCallStackListCtrl    m_heldCallList;
    //}}AFX_DATA


// Overrides
    // ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCallStackCurrentPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();
	virtual void OnOK();
	virtual void OnDestroy();
	//}}AFX_VIRTUAL


    /*
     * onRunParamChange (interface implementation)
     *
     * @see RunParamUser::onRunParamChange for more details
     *
     */
	virtual void onRunParamChange(const std::string& name, const std::string& value);

// Implementation

private:

    /**
     * Accepts (answers) the specified call
     */
    void acceptRadioCall(long callID);


    /**
     * Answers the outstanding call OCC request from the given train
     *
     * @param trainId    the train to answer
     */
    void acceptTrainCall(TA_IRS_Bus::CommonTypes::TrainIdType trainId);


    /**
     * Updates the enabled state of the "Accept" button, based on current
     *  selection status and privilege settings
     */
    void updateAcceptButtonState();

	//int  FindCall(CListCtrl& callList,UINT callId);
	//void UpdateCurrentCallDetails(CListCtrl* pCallList);

    /**
     * answerRunParamCall
     *
     * Answers the call defines by the call ID currently stored in runparams
     *
     */
    void answerRunParamCall();

public:

	afx_msg void OnAccept();

protected:

	CFont m_Font;
	// Generated message map functions
	//{{AFX_MSG(CCallStackCurrentPage)
	virtual BOOL OnInitDialog();
	afx_msg void onRecall();
	afx_msg void onDblclkCalllistIn(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void onDblclkCalllistHold(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void onItemchangedCalllistHold(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void onItemchangedCalllistIn(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg LRESULT OnCallIdChange(UINT wParam, LONG lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

};

#endif  // __CALLSTACKCURRENTPAGE_H__
