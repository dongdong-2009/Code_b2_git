/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/bus/signs/RATISCallBannerLibrary/src/ratisapprovaldialog.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class is used to display a dialog that allows the
  * the user to approve or reject RATIS messages.
  *
  */

#if !defined(AFX_RATISAPPROVALDIALOG_H__1CEE00C8_973B_4505_BFD0_4CDDD820309A__INCLUDED_)
#define AFX_RATISAPPROVALDIALOG_H__1CEE00C8_973B_4505_BFD0_4CDDD820309A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "resource.h"

namespace TA_IRS_Bus
{
    class RATISCallEntry; // Forward declaration

    class RatisApprovalDialog : public CDialog
    {
    // Construction
    public:

        /**
         *  Constructs a new dialog with the given call entry being displayed
         */
        RatisApprovalDialog(RATISCallEntry* callEntry, CWnd* pParent = NULL);


        /**
         *  Sets the call entry object that is being displayed by this dialog.
         */
        void setCallEntry(RATISCallEntry* callEntry);


        /**
         *  Gets the call entry object that is being displayed by this dialog
         */
        RATISCallEntry& getCallEntry();


        /**
         *  Refreshes the dialog's text fields to reflect what is in the
         *  call entry object, disables the continue button, and puts the
         *  status bar into a neutral state.
         */
        void refreshFields();


        /**
         *  Shows a message box with the given error message.
         */
        void showErrorMessage(const std::string& message);

    // Dialog Data
	    //{{AFX_DATA(RatisApprovalDialog)
	enum { IDD = IDD_RATIS_APPROVAL };
	CButton	m_emergencyRadio;
	CButton	m_normalRadio;
	CButton	m_priority6Radio;
	CEdit	m_destinationCtrl;
	CEdit	m_priorityCtrl;
	CEdit	m_endTimeCtrl;
	CEdit	m_startTimeCtrl;
	CEdit	m_typeCtrl;
	CEdit	m_ratisTagCtrl;
	CEdit	m_contentCtrl;
	UINT	m_priority;
	//}}AFX_DATA


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(RatisApprovalDialog)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:

	    // Generated message map functions
	    //{{AFX_MSG(RatisApprovalDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnApprove();
	afx_msg void OnReject();
	afx_msg void OnUpdatePriority();
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:

        // The call entry whose details are currently being displayed
        RATISCallEntry* m_callEntry;

    };

} // namespace TA_IRS_Bus

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RATISAPPROVALDIALOG_H__1CEE00C8_973B_4505_BFD0_4CDDD820309A__INCLUDED_)
